#include "paie_calcul.h"

#include <QDate>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <cmath>

Bulletin calculerBulletin(const QString &matricule, int mois, int annee)
{
    Bulletin b;

    // ==============================================================
    // ETAPE 1 : RECHERCHE D'INFORMATIONS RELATIVES A L'EMPLOYE
    // ==============================================================

    // --- 1.1 Entrer le matricule, le mois et l'annee ------------
    // Depend de : les 3 parametres de la fonction
    // Produit   : rien (arrete la fonction si une valeur est invalide)
    if (matricule.trimmed().isEmpty()) {
        b.erreur = "Le matricule est obligatoire";
        return b;
    }
    if (mois < 1 || mois > 12 || !QDate(annee, mois, 1).isValid()) {
        b.erreur = "Mois ou annee invalide";
        return b;
    }

    // --- 1.2 Rechercher l'employe (EMPLOYE) et son poste (POSTE) -
    // Depend de : 1.1 (matricule valide)
    // Produit   : le curseur q, positionne sur la ligne de l'employe
    //             colonnes : 0 Nom, 1 dateEmbauche, 2 Libelle,
    //                        3 SalaireBase, 4 TauxHeureSup
    QSqlQuery q;
    q.prepare("SELECT e.Nom, e.dateEmbauche, p.Libelle, p.SalaireBase, p.TauxHeureSup "
              "FROM EMPLOYE e JOIN POSTE p ON e.codePoste = p.codePoste "
              "WHERE e.Matricule = ?");
    q.addBindValue(matricule);

    if (!q.exec()) {
        b.erreur = "Erreur SQL : " + q.lastError().text();
        return b;
    }
    if (!q.next()) {
        b.erreur = "Employé non trouvé";
        return b;
    }

    // --- 1.3 Afficher le matricule, le nom et le libelle du poste
    // Depend de : 1.2 (q.next() a reussi)
    // Produit   : b.matricule, b.nom, b.poste
    b.matricule = matricule;
    b.nom       = q.value(0).toString();
    b.poste     = q.value(2).toString();


    // ==============================================================
    // ETAPE 2 : CALCUL DU BRUT
    // ==============================================================

    // --- 2.1 Lire SalaireBase (POSTE), HeuresSup et JoursAbsence (POINTAGE)

    // 2.1.a SalaireBase
    // Depend de : 1.2 (q, colonne 3)
    // Produit   : b.salaireBase
    bool salaireValide = false;
    b.salaireBase = q.value(3).toDouble(&salaireValide);     // <-- A CORRIGER (voir note plus bas)

    if (!salaireValide || !std::isfinite(b.salaireBase)
        || b.salaireBase < 0) {
        b.erreur = "Salaire de base invalide";
        return b;
    }

    // 2.1.b TauxHeureSup
    // Depend de : 1.2 (q, colonne 4)
    // Produit   : tauxHeureSup
    bool tauxValide = false;
    const double tauxHeureSup = q.value(4).toDouble(&tauxValide);

    if (!tauxValide || !std::isfinite(tauxHeureSup)
        || tauxHeureSup < 0) {
        b.erreur = "Taux horaire supplementaire invalide";
        return b;
    }

    // 2.1.c HeuresSup et JoursAbsence (table POINTAGE)
    // Depend de : 1.1 (matricule, mois, annee)
    // Produit   : heuresSup (utilise en 2.4), joursAbsence (utilise en 3.1)
    QSqlQuery p;
    p.prepare("SELECT HeuresSup, JoursAbsence FROM POINTAGE "
              "WHERE Matricule = ? AND Mois = ? AND Annee = ?");
    p.addBindValue(matricule);
    p.addBindValue(mois);
    p.addBindValue(annee);

    if (!p.exec()) {
        b.erreur = "Erreur SQL : " + p.lastError().text();
        return b;
    }
    if (!p.next()) {
        b.erreur = "Aucun pointage pour cet employé sur cette période";
        return b;
    }

    // On copie les valeurs AVANT le 2e next() (qui deplace le curseur)
    const QVariant valeurHeuresSup    = p.value(0);
    const QVariant valeurJoursAbsence = p.value(1);

    if (p.next()) {
        b.erreur = "Plusieurs pointages existent pour cette periode";
        return b;
    }

    bool heuresValides = false;
    const double heuresSup = valeurHeuresSup.toDouble(&heuresValides);

    if (!heuresValides || !std::isfinite(heuresSup)
        || heuresSup < 0) {
        b.erreur = "Nombre d'heures supplementaires invalide";
        return b;
    }

    bool absenceValide = false;
    const double joursAbsence = valeurJoursAbsence.toDouble(&absenceValide);

    if (!absenceValide || !std::isfinite(joursAbsence)
        || joursAbsence < 0 || joursAbsence > 30) {
        b.erreur = "Nombre de jours d'absence invalide";
        return b;
    }

    // --- 2.2 Anciennete = annee en cours - annee d'embauche -----
    // Depend de : 1.2 (q, colonne 1) et du parametre annee
    // Produit   : b.anciennete
    const QDate dateEmbauche = q.value(1).toDate();

    if (!dateEmbauche.isValid()) {
        b.erreur = "Date d'embauche invalide";
        return b;
    }
    if (annee < dateEmbauche.year()) {
        b.erreur = "L'annee du bulletin est anterieure a l'embauche";
        return b;
    }

    b.anciennete = annee - dateEmbauche.year();

    // --- 2.3 Prime : anciennete >= 5 ans -> 5 % du SalaireBase, sinon 0
    // Depend de : 2.2 (b.anciennete) et 2.1.a (b.salaireBase)
    // Produit   : b.prime
    b.prime = (b.anciennete >= 5) ? b.salaireBase * 0.05 : 0.0;

    // --- 2.4 montantHeuresSup = HeuresSup x TauxHeureSup --------
    // Depend de : 2.1.c (heuresSup) et 2.1.b (tauxHeureSup)
    // Produit   : b.montantHeuresSup
    b.montantHeuresSup = heuresSup * tauxHeureSup;

    // --- 2.5 Brut = SalaireBase + prime + montantHeuresSup ------
    // Depend de : 2.1.a, 2.3, 2.4
    // Produit   : b.brut
    b.brut = b.salaireBase + b.prime + b.montantHeuresSup;

    if (!std::isfinite(b.prime)
        || !std::isfinite(b.montantHeuresSup)
        || !std::isfinite(b.brut)) {
        b.erreur = "Erreur : resultat du calcul brut invalide";
        return b;
    }


    // ==============================================================
    // ETAPE 3 : CALCUL DU NET
    // ==============================================================

    // --- 3.1 Retenue absence = JoursAbsence x (SalaireBase / 30) -
    // Depend de : 2.1.c (joursAbsence) et 2.1.a (b.salaireBase)
    // Produit   : b.retenueAbsence
    b.retenueAbsence = joursAbsence * (b.salaireBase / 30.0);

    // --- 3.2 Retenue sociale = 1 % du Brut ----------------------
    // Depend de : 2.5 (b.brut)
    // Produit   : b.retenueSociale
    b.retenueSociale = 0.01 * b.brut;

    // --- 3.3 Net a payer = Brut - retenue absence - retenue sociale
    // Depend de : 2.5, 3.1, 3.2
    // Produit   : b.net, et b.ok = true (bulletin valide)
    b.net = b.brut - b.retenueAbsence - b.retenueSociale;

    b.ok = true;
    return b;
}



QList<LignePoste> masseSalarialeParPoste(int mois, int annee, double &total)
{
    // ==============================================================
    // 6.1 ENTRER LE MOIS ET L'ANNEE
    // ==============================================================
    // Depend de : les parametres mois et annee (saisis dans l'interface)
    // Produit   : resultat (liste vide) et total = 0
    // (pas de validation ici : elle est faite dans calculerBulletin)
    QList<LignePoste> resultat;
    total = 0;

    // ==============================================================
    // 6.2 POUR CHAQUE POSTE : EFFECTIF + SOMME DES SALAIRES NETS
    // ==============================================================

    // --- 6.2.a Lister les postes ---------------------------------
    // Depend de : table POSTE
    // Produit   : le curseur postes (codePoste, Libelle), trie par libelle
    QSqlQuery postes("SELECT codePoste, Libelle FROM POSTE ORDER BY Libelle");

    while (postes.next()) {

        // Une ligne de resultat par poste
        // Produit : ligne.libelle (effectif, pointes, masse partent de 0)
        LignePoste ligne;
        ligne.libelle = postes.value(1).toString();

        // --- 6.2.b Lister les employes de ce poste ---------------
        // Depend de : 6.2.a (postes.value(0) = codePoste courant)
        // Produit   : le curseur emp (une ligne = un matricule)
        QSqlQuery emp;
        emp.prepare("SELECT Matricule FROM EMPLOYE WHERE codePoste = ?");
        emp.addBindValue(postes.value(0));
        emp.exec();

        while (emp.next()) {

            // --- 6.2.c Effectif : compter chaque employe du poste -
            // Depend de : 6.2.b
            // Produit   : ligne.effectif
            ligne.effectif++;

            // --- 6.2.d Salaire net : reutiliser le calcul du bulletin
            // Depend de : calculerBulletin (etapes 1, 2, 3), mois, annee
            // Produit   : b (bulletin de l'employe)
            Bulletin b = calculerBulletin(emp.value(0).toString(), mois, annee);

            // --- 6.2.e Somme des nets du poste -------------------
            // Depend de : 6.2.d (b.ok et b.net)
            // Produit   : ligne.pointes et ligne.masse
            if (b.ok) {
                ligne.pointes++;
                ligne.masse += b.net;
            }
        }

        // --- 6.2.f Enregistrer la ligne du poste -----------------
        // Depend de : 6.2.c et 6.2.e
        // Produit   : une entree dans resultat
        resultat.append(ligne);

        // ==========================================================
        // 6.3 TOTAL GENERAL (cumule poste apres poste)
        // ==========================================================
        // Depend de : ligne.masse (6.2.e)
        // Produit   : total (parametre par reference, lu par l'interface)
        total += ligne.masse;
    }

    return resultat;
}
#include "paie_calcul.h"

#include <QDate>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <cmath>

Bulletin calculerBulletin(const QString &matricule, int mois, int annee)
{
    Bulletin b;

    // ETAPE 1 : RECHERCHE D'INFORMATIONS RELATIVES A L'EMPLOYE
    if (matricule.trimmed().isEmpty()) {
        b.erreur = "Le matricule est obligatoire";
        return b;
    }
    if (mois < 1 || mois > 12 || !QDate(annee, mois, 1).isValid()) {
        b.erreur = "Mois ou annee invalide";
        return b;
    }

    //1.2 Rechercher l'employe (EMPLOYE) et son poste (POSTE) -
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

    //1.3 Afficher le matricule, le nom et le libelle du poste
    b.matricule = matricule;
    b.nom       = q.value(0).toString();
    b.poste     = q.value(2).toString();


    // ETAPE 2 : CALCUL DU BRUT

    //2.1 Lire SalaireBase (POSTE), HeuresSup et JoursAbsence (POINTAGE)

    // 2.1.a SalaireBase
    bool salaireValide = false;
    b.salaireBase = q.value(3).toDouble(&salaireValide); 

    if (!salaireValide || !std::isfinite(b.salaireBase)
        || b.salaireBase < 0) {
        b.erreur = "Salaire de base invalide";
        return b;
    }

    // 2.1.b TauxHeureSup
    bool tauxValide = false;
    const double tauxHeureSup = q.value(4).toDouble(&tauxValide);

    if (!tauxValide || !std::isfinite(tauxHeureSup)
        || tauxHeureSup < 0) {
        b.erreur = "Taux horaire supplementaire invalide";
        return b;
    }

    // 2.1.c HeuresSup et JoursAbsence (table POINTAGE)
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

    // Prend les valeurs AVANT le 2e next() (qui deplace le curseur)
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

    //2.2 Anciennete = annee en cours - annee d'embauche
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

    //2.3 Prime : anciennete >= 5 ans
    b.prime = (b.anciennete >= 5) ? b.salaireBase * 0.05 : 0.0;

    // --- 2.4 montantHeuresSup
    b.montantHeuresSup = heuresSup * tauxHeureSup;

    // --- 2.5 Brut = SalaireBase + prime + montantHeuresSup ------
    b.brut = b.salaireBase + b.prime + b.montantHeuresSup;

    if (!std::isfinite(b.prime)
        || !std::isfinite(b.montantHeuresSup)
        || !std::isfinite(b.brut)) {
        b.erreur = "Erreur : resultat du calcul brut invalide";
        return b;
    }


    // ETAPE 3 : CALCUL DU NET
    b.retenueAbsence = joursAbsence * (b.salaireBase / 30.0);

    //3.2 Retenue sociale
    b.retenueSociale = 0.01 * b.brut;

    //3.3 Net a payer
    b.net = b.brut - b.retenueAbsence - b.retenueSociale;

    b.ok = true;
    return b;
}



QList<LignePoste> masseSalarialeParPoste(int mois, int annee, double &total)
{
    // 6.1 ENTRER LE MOIS ET L'ANNEE
    QList<LignePoste> resultat;
    total = 0;

    QSqlQuery postes("SELECT codePoste, Libelle FROM POSTE ORDER BY Libelle");

    while (postes.next()) {
        LignePoste ligne;
        ligne.libelle = postes.value(1).toString();
        QSqlQuery emp;
        emp.prepare("SELECT Matricule FROM EMPLOYE WHERE codePoste = ?");
        emp.addBindValue(postes.value(0));
        emp.exec();

        while (emp.next()) {
            ligne.effectif++;
            Bulletin b = calculerBulletin(emp.value(0).toString(), mois, annee);
            if (b.ok) {
                ligne.pointes++;
                ligne.masse += b.net;
            }
        }
        resultat.append(ligne);
        total += ligne.masse;
    }

    return resultat;
}
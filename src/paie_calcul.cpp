#include "paie_calcul.h"

#include <QDate>
#include <QSqlError>
#include <QSqlQuery>

Bulletin calculerBulletin(const QString &matricule, int mois, int annee)
{
    Bulletin b;

    // Etape 1 : employé + poste (jointure)
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

    b.matricule   = matricule;
    b.nom         = q.value(0).toString();
    QDate dateEmbauche = q.value(1).toDate();
    b.poste       = q.value(2).toString();
    b.salaireBase = q.value(3).toDouble();
    const double tauxHeureSup = q.value(4).toDouble();

    // Pointage du mois
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
    const double heuresSup   = p.value(0).toDouble();
    const double joursAbsence = p.value(1).toDouble();

    // Etape 2 : brut
    // Ancienneté = année en cours - année d'embauche (énoncé).
    // Pour utiliser l'année du bulletin à la place : remplacer par "annee".
    b.anciennete = QDate::currentDate().year() - dateEmbauche.year();
    b.prime = (b.anciennete >= 5) ? 0.05 * b.salaireBase : 0.0;
    b.montantHeuresSup = heuresSup * tauxHeureSup;
    b.brut = b.salaireBase + b.prime + b.montantHeuresSup;

    // Etape 3 : net
    b.retenueAbsence  = joursAbsence * (b.salaireBase / 30.0);
    b.retenueSociale  = 0.01 * b.brut;
    b.net = b.brut - b.retenueAbsence - b.retenueSociale;

    b.ok = true;
    return b;
}

QList<LignePoste> masseSalarialeParPoste(int mois, int annee, double &total)
{
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
            if (b.ok) {              // employé sans pointage : ignoré dans la somme
                ligne.pointes++;
                ligne.masse += b.net;
            }
        }
        total += ligne.masse;
        resultat.append(ligne);
    }
    return resultat;
}

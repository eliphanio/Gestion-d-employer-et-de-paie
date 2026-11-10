#pragma once
#include <QList>
#include <QString>

// Résultat du calcul d'un bulletin de paie
struct Bulletin {
    bool ok = false;      
    QString erreur;

    QString matricule, nom, poste;
    int anciennete = 0;

    double salaireBase = 0;
    double prime = 0;
    double montantHeuresSup = 0;
    double brut = 0;
    double retenueAbsence = 0;
    double retenueSociale = 0;
    double net = 0;
};

// Une ligne de la masse salariale (un poste)
struct LignePoste {
    QString libelle;
    int effectif = 0; 
    int pointes = 0;   
    double masse = 0;  
};

// Calcule le bulletin d'un employé pour un mois/année (connexion MySQL par défaut)
Bulletin calculerBulletin(const QString &matricule, int mois, int annee);

// Masse salariale d'un mois, poste par poste (réutilise calculerBulletin)
QList<LignePoste> masseSalarialeParPoste(int mois, int annee, double &total);

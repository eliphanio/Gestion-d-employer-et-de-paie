#pragma once
#include <QDialog> 
#include <QWidget> 
#include <QList>

#include "paie_calcul.h"

class QComboBox;
class QLabel;
class QPushButton;
class QTableWidget;


// ---------- Menu 4 : Edition d'un bulletin de paie ----------
class BulletinDialog : public QWidget
{
    Q_OBJECT
public:
    explicit BulletinDialog(QWidget *parent = nullptr);

private slots:
    void afficher();
    void exporterPdf();

private:
    QString htmlBulletin() const;

    QComboBox *cbMatricule;
    QComboBox *cbMois;
    QComboBox *cbAnnee;
    QLabel *lblInfo;
    QTableWidget *table;
    QLabel *lblNet;

    Bulletin dernier;
    int dernierMois = 0;
    int dernierAnnee = 0;
};

// ---------- Menu 5 : Masse salariale d'un mois par poste ----------
class MasseSalarialeDialog : public QWidget
{
    Q_OBJECT
public:
    explicit MasseSalarialeDialog(QWidget *parent = nullptr);

private slots:
    void calculer();
    void ouvrirGraphique();

private:
    QComboBox *cbMois;
    QComboBox *cbAnnee;
    QTableWidget *table;
    QLabel *lblTotal;
    QPushButton *btnGraphique;

    QList<LignePoste> lignes;
    int moisCalcule = 0;
    int anneeCalculee = 0;
};

// ---------- Menu 6 : Employés par poste avec l'effectif ----------
class EmployesParPosteDialog : public QWidget
{
    Q_OBJECT
public:
    explicit EmployesParPosteDialog(QWidget *parent = nullptr);

private slots:
    void charger();

private:
    QComboBox *cbPoste;
    QTableWidget *table;
    QLabel *lblEffectif;
};

// ---------- Bonus : histogramme de la masse salariale par poste ----------
class GraphiqueMasseDialog : public QDialog
{
    Q_OBJECT
public:
    GraphiqueMasseDialog(
        const QList<LignePoste> &lignes, int mois, int annee,
        QWidget *parent = nullptr
    );
};

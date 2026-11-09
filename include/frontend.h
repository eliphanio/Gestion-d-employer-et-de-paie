
#pragma once

#include <QMainWindow>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QStackedWidget>

#include "paie_calcul.h"

class QWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    // =================================================
    // NAVIGATION
    // =================================================

    QStackedWidget *pages = nullptr;

    QPushButton *btnBulletin = nullptr;
    QPushButton *btnMasse = nullptr;

    // =================================================
    // PAGE 1 : BULLETIN DE PAIE
    // =================================================

    QLineEdit *matriculeEdit = nullptr;
    QLineEdit *nomEdit = nullptr;
    QLineEdit *posteEdit = nullptr;

    QComboBox *moisBulletin = nullptr;
    QComboBox *anneeBulletin = nullptr;

    QPushButton *btnRechercher = nullptr;
    QPushButton *btnCalculer = nullptr;
    QPushButton *btnImprimer = nullptr;
    QPushButton *btnReinitialiser = nullptr;

    QLabel *salaireBaseLabel = nullptr;
    QLabel *primeLabel = nullptr;
    QLabel *heuresSupLabel = nullptr;
    QLabel *brutLabel = nullptr;
    QLabel *retenueAbsenceLabel = nullptr;
    QLabel *retenueSocialeLabel = nullptr;
    QLabel *netLabel = nullptr;

    QTableWidget *tableauBulletin = nullptr;

    // =================================================
    // PAGE 2 : MASSE SALARIALE
    // =================================================

    QComboBox *moisMasse = nullptr;
    QComboBox *anneeMasse = nullptr;

    QPushButton *btnCalculerMasse = nullptr;

    QTableWidget *tableauMasse = nullptr;
    QLabel *totalMasseLabel = nullptr;

    // =================================================
    // CONSTRUCTION DES PAGES
    // =================================================

    QWidget *creerPageBulletin();
    QWidget *creerPageMasse();

    // =================================================
    // ACTIONS
    // =================================================

    void afficherBulletin();
    void calculerMasse();
    void reinitialiserBulletin();
    void imprimerBulletin();

    // =================================================
    // UTILITAIRES
    // =================================================

    void initialiserPeriodes();
    QString montant(double valeur) const;
};

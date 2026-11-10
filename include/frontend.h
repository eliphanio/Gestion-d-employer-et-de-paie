#pragma once

#include <QWidget>
#include <QString>

#include "paie_calcul.h"

class QLineEdit;
class QComboBox;
class QPushButton;
class QLabel;
class QTableWidget;

// =====================================================
// PAGE : BULLETIN DE PAIE
// =====================================================
class BulletinDialog : public QWidget
{
    Q_OBJECT

public:
    explicit BulletinDialog(QWidget *parent = nullptr);

private slots:
    void afficherBulletin();
    void reinitialiserBulletin();
    void imprimerBulletin();

private:
    void viderResultats();

    bool bulletinValide = false;

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
};

// =====================================================
// PAGE : MASSE SALARIALE
// =====================================================
class MasseSalarialeDialog : public QWidget
{
    Q_OBJECT

public:
    explicit MasseSalarialeDialog(QWidget *parent = nullptr);

private slots:
    void calculerMasse();

private:
    QComboBox *moisMasse = nullptr;
    QComboBox *anneeMasse = nullptr;

    QPushButton *btnCalculerMasse = nullptr;

    QTableWidget *tableauMasse = nullptr;
    QLabel *totalMasseLabel = nullptr;
};

// =====================================================
// PAGE : EMPLOYÉS PAR POSTE
// =====================================================
class EmployesParPosteDialog : public QWidget
{
    Q_OBJECT

public:
    explicit EmployesParPosteDialog(QWidget *parent = nullptr);

private slots:
    void chargerEmployes();

private:
    void chargerPostes();

    QComboBox *posteCombo = nullptr;
    QTableWidget *tableau = nullptr;
    QLabel *effectifLabel = nullptr;
};
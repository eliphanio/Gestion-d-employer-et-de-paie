#ifndef POINTAGE_H
#define POINTAGE_H

#include <QWidget>
#include <QSqlDatabase>
#include <QString>

class QComboBox;
class QSpinBox;
class QDoubleSpinBox;
class QPushButton;
class QLineEdit;
class QTableWidget;

class PointagePage : public QWidget
{
    Q_OBJECT

public:
    explicit PointagePage(QWidget *parent = nullptr);

private:
    QComboBox *comboEmploye;
    QComboBox *comboMois;

    QSpinBox *spinAnnee;

    QDoubleSpinBox *spinHeuresSup;
    QDoubleSpinBox *spinJoursAbsence;

    QPushButton *btnAjouter;
    QPushButton *btnModifier;
    QPushButton *btnSupprimer;

    QLineEdit *searchPointage;

    QTableWidget *tablePointage;

    QString matriculeSelectionne;
    int moisSelectionne = 0;
    int anneeSelectionnee = 0;

    void creerInterface();

    void chargerEmployes();
    void afficherPointages();

    void ajouterPointage();
    void modifierPointage();
    void supprimerPointage();

    void rechercherPointage();
    void selectionnerPointage();

    bool verifierConnexion();

};

#endif // POINTAGE_H

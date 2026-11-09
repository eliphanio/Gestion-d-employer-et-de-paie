#include "pointage.h"
#include "database.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QAbstractItemView>
#include <QGridLayout>
#include <QDate>
#include <QStringList>

// Constructeur
PointagePage::PointagePage(QWidget *parent)
    : QWidget(parent)
{
    creerInterface();

    chargerEmployes();

    afficherPointages();
}

// Vérification de la connexion
bool PointagePage::verifierConnexion()
{
    QSqlDatabase db = Database::getConnection();

    if (!db.isOpen()) {
        if (!Database::connect()) {
            QMessageBox::critical(
                this,
                "Erreur",
                "Impossible de se connecter à la base de données."
                );
            return false;
        }
    }

    return true;
}

// Création de l'interface
void PointagePage::creerInterface()
{
    QVBoxLayout *layoutPrincipal = new QVBoxLayout(this);

    QLabel *titre = new QLabel("Gestion du pointage mensuel");
    titre->setStyleSheet(
        "font-size: 20px;"
        "font-weight: bold;"
        "color: #243447;"
        "padding: 8px;"
        );

    layoutPrincipal->addWidget(titre);

    // Formulaire
    QGroupBox *formulaire =
        new QGroupBox("Informations du pointage");

    QGridLayout *layoutFormulaire = new QGridLayout(formulaire);

    comboEmploye = new QComboBox();
    comboMois = new QComboBox();

    QStringList mois = {
        "Janvier", "Février", "Mars", "Avril",
        "Mai", "Juin", "Juillet", "Août",
        "Septembre", "Octobre", "Novembre", "Décembre"
    };

    for (int i = 0; i < mois.size(); i++) {
        comboMois->addItem(mois[i], i + 1);
    }

    spinAnnee = new QSpinBox();
    spinAnnee->setRange(2000, 2100);
    spinAnnee->setValue(QDate::currentDate().year());

    spinHeuresSup = new QDoubleSpinBox();
    spinHeuresSup->setRange(0, 999999);
    spinHeuresSup->setDecimals(2);
    spinHeuresSup->setSuffix(" h");

    spinJoursAbsence = new QDoubleSpinBox();
    spinJoursAbsence->setRange(0, 31);
    spinJoursAbsence->setDecimals(2);
    spinJoursAbsence->setSuffix(" j");

    layoutFormulaire->addWidget(new QLabel("Employé :"), 0, 0);
    layoutFormulaire->addWidget(comboEmploye, 0, 1);

    layoutFormulaire->addWidget(new QLabel("Mois :"), 1, 0);
    layoutFormulaire->addWidget(comboMois, 1, 1);

    layoutFormulaire->addWidget(new QLabel("Année :"), 2, 0);
    layoutFormulaire->addWidget(spinAnnee, 2, 1);

    layoutFormulaire->addWidget(new QLabel("Heures sup. :"), 3, 0);
    layoutFormulaire->addWidget(spinHeuresSup, 3, 1);

    layoutFormulaire->addWidget(new QLabel("Jours d'absence :"), 4, 0);
    layoutFormulaire->addWidget(spinJoursAbsence, 4, 1);

    layoutPrincipal->addWidget(formulaire);

    // Boutons
    QHBoxLayout *layoutBoutons = new QHBoxLayout();

    btnAjouter = new QPushButton("Ajouter");
    btnModifier = new QPushButton("Modifier");
    btnSupprimer = new QPushButton("Supprimer");

    btnAjouter->setStyleSheet(
        "background-color: #1729ff;"
        "color: white;"
        "font-weight: bold;"
        "padding: 9px;"
        "border-radius: 5px;"
        );

    btnModifier->setStyleSheet(
        "background-color: #607d8b;"
        "color: white;"
        "font-weight: bold;"
        "padding: 9px;"
        "border-radius: 5px;"
        );

    btnSupprimer->setStyleSheet(
        "background-color: #d9534f;"
        "color: white;"
        "font-weight: bold;"
        "padding: 9px;"
        "border-radius: 5px;"
        );

    layoutBoutons->addWidget(btnAjouter);
    layoutBoutons->addWidget(btnModifier);
    layoutBoutons->addWidget(btnSupprimer);

    layoutPrincipal->addLayout(layoutBoutons);

    // Recherche
    searchPointage = new QLineEdit();

    searchPointage->setPlaceholderText(
        "Rechercher par matricule ou par nom..."
        );

    layoutPrincipal->addWidget(searchPointage);

    // Tableau
    tablePointage = new QTableWidget();

    tablePointage->setColumnCount(6);

    tablePointage->setHorizontalHeaderLabels({
        "Matricule",
        "Nom",
        "Mois",
        "Année",
        "Heures sup.",
        "Jours d'absence"
    });

    tablePointage->horizontalHeader()
        ->setSectionResizeMode(QHeaderView::Stretch);

    tablePointage->setSelectionBehavior(
        QAbstractItemView::SelectRows
        );

    tablePointage->setSelectionMode(
        QAbstractItemView::SingleSelection
        );

    tablePointage->setEditTriggers(
        QAbstractItemView::NoEditTriggers
        );

    layoutPrincipal->addWidget(tablePointage);

    // Connexion des boutons
    connect(btnAjouter, &QPushButton::clicked,
            this, &PointagePage::ajouterPointage);

    connect(btnModifier, &QPushButton::clicked,
            this, &PointagePage::modifierPointage);

    connect(btnSupprimer, &QPushButton::clicked,
            this, &PointagePage::supprimerPointage);

    connect(searchPointage, &QLineEdit::textChanged,
            this, &PointagePage::rechercherPointage);

    connect(tablePointage, &QTableWidget::cellClicked,
            this, [this](int row, int column) {
                Q_UNUSED(column);
                selectionnerPointage();
            });
}

// Charger les employés
void PointagePage::chargerEmployes()
{
    if (!verifierConnexion())
        return;

    QSqlDatabase db = Database::getConnection();

    QSqlQuery query(db);

    query.prepare(
        "SELECT Matricule, Nom "
        "FROM EMPLOYE "
        "ORDER BY Nom ASC"
        );

    if (!query.exec()) {
        qDebug() << "Erreur chargement employés :"
                 << query.lastError().text();
        return;
    }

    comboEmploye->clear();

    while (query.next()) {

        QString matricule = query.value("Matricule").toString();
        QString nom = query.value("Nom").toString();

        comboEmploye->addItem(
            matricule + " - " + nom,
            matricule
            );
    }
}

// Afficher tous les pointages
void PointagePage::afficherPointages()
{
    if (!verifierConnexion())
        return;

    QSqlDatabase db = Database::getConnection();

    QSqlQuery query(db);

    query.prepare(
        "SELECT p.Matricule, e.Nom, "
        "p.Mois, p.Annee, "
        "p.HeuresSup, p.JoursAbsence "
        "FROM POINTAGE p "
        "INNER JOIN EMPLOYE e "
        "ON p.Matricule = e.Matricule "
        "ORDER BY p.Annee DESC, p.Mois DESC"
        );

    if (!query.exec()) {
        qDebug() << "Erreur affichage pointage :"
                 << query.lastError().text();
        return;
    }

    tablePointage->setRowCount(0);

    int ligne = 0;

    while (query.next()) {

        tablePointage->insertRow(ligne);

        for (int colonne = 0; colonne < 6; colonne++) {

            tablePointage->setItem(
                ligne,
                colonne,
                new QTableWidgetItem(
                    query.value(colonne).toString()
                    )
                );
        }

        ligne++;
    }
}

// Ajouter un pointage
void PointagePage::ajouterPointage()
{
    if (!verifierConnexion())
        return;

    if (comboEmploye->currentIndex() < 0) {
        QMessageBox::warning(
            this, "Attention",
            "Veuillez sélectionner un employé."
            );
        return;
    }

    QSqlDatabase db = Database::getConnection();

    QSqlQuery query(db);

    query.prepare(
        "INSERT INTO POINTAGE "
        "(Matricule, Mois, Annee, HeuresSup, JoursAbsence) "
        "VALUES (:matricule, :mois, :annee, :heures, :absence)"
        );

    query.bindValue(":matricule", comboEmploye->currentData());
    query.bindValue(":mois", comboMois->currentData());
    query.bindValue(":annee", spinAnnee->value());
    query.bindValue(":heures", spinHeuresSup->value());
    query.bindValue(":absence", spinJoursAbsence->value());

    if (!query.exec()) {

        QMessageBox::warning(
            this,
            "Erreur",
            "Impossible d'ajouter le pointage.\n"
                + query.lastError().text()
            );

        return;
    }

    QMessageBox::information(
        this, "Succès",
        "Pointage ajouté avec succès."
        );

    afficherPointages();
}
void PointagePage::selectionnerPointage()
{
    int ligne = tablePointage->currentRow();

    if (ligne < 0)
        return;

    matriculeSelectionne =
        tablePointage->item(ligne, 0)->text();

    moisSelectionne =
        tablePointage->item(ligne, 2)->text().toInt();

    anneeSelectionnee =
        tablePointage->item(ligne, 3)->text().toInt();

    int indexEmploye =
        comboEmploye->findData(matriculeSelectionne);

    if (indexEmploye >= 0)
        comboEmploye->setCurrentIndex(indexEmploye);

    int indexMois = comboMois->findData(moisSelectionne);

    if (indexMois >= 0)
        comboMois->setCurrentIndex(indexMois);

    spinAnnee->setValue(anneeSelectionnee);

    spinHeuresSup->setValue(
        tablePointage->item(ligne, 4)->text().toDouble()
        );

    spinJoursAbsence->setValue(
        tablePointage->item(ligne, 5)->text().toDouble()
        );
}
void PointagePage::modifierPointage()
{
    if (matriculeSelectionne.isEmpty()) {
        QMessageBox::warning(
            this, "Attention",
            "Veuillez sélectionner un pointage dans le tableau."
            );
        return;
    }

    if (!verifierConnexion())
        return;

    QSqlDatabase db = Database::getConnection();

    QSqlQuery query(db);

    query.prepare(
        "UPDATE POINTAGE SET "
        "Matricule = :nouveauMatricule, "
        "Mois = :nouveauMois, "
        "Annee = :nouvelleAnnee, "
        "HeuresSup = :heures, "
        "JoursAbsence = :absence "
        "WHERE Matricule = :ancienMatricule "
        "AND Mois = :ancienMois "
        "AND Annee = :ancienneAnnee"
        );

    query.bindValue(":nouveauMatricule",
                    comboEmploye->currentData());

    query.bindValue(":nouveauMois",
                    comboMois->currentData());

    query.bindValue(":nouvelleAnnee",
                    spinAnnee->value());

    query.bindValue(":heures",
                    spinHeuresSup->value());

    query.bindValue(":absence",
                    spinJoursAbsence->value());

    query.bindValue(":ancienMatricule",
                    matriculeSelectionne);

    query.bindValue(":ancienMois",
                    moisSelectionne);

    query.bindValue(":ancienneAnnee",
                    anneeSelectionnee);

    if (!query.exec()) {
        QMessageBox::warning(
            this, "Erreur",
            query.lastError().text()
            );
        return;
    }

    if (query.numRowsAffected() == 0) {
        QMessageBox::warning(
            this, "Attention",
            "Aucune modification effectuée."
            );
        return;
    }

    QMessageBox::information(
        this, "Succès",
        "Pointage modifié avec succès."
        );

    matriculeSelectionne.clear();

    afficherPointages();
}
void PointagePage::supprimerPointage()
{
    if (matriculeSelectionne.isEmpty()) {
        QMessageBox::warning(
            this, "Attention",
            "Veuillez sélectionner un pointage."
            );
        return;
    }

    QMessageBox::StandardButton reponse =
        QMessageBox::question(
            this,
            "Confirmation",
            "Voulez-vous vraiment supprimer ce pointage ?",
            QMessageBox::Yes | QMessageBox::No
            );

    if (reponse != QMessageBox::Yes)
        return;

    if (!verifierConnexion())
        return;

    QSqlDatabase db = Database::getConnection();

    QSqlQuery query(db);

    query.prepare(
        "DELETE FROM POINTAGE "
        "WHERE Matricule = :matricule "
        "AND Mois = :mois "
        "AND Annee = :annee"
        );

    query.bindValue(":matricule", matriculeSelectionne);
    query.bindValue(":mois", moisSelectionne);
    query.bindValue(":annee", anneeSelectionnee);

    if (!query.exec()) {
        QMessageBox::critical(
            this, "Erreur",
            query.lastError().text()
            );
        return;
    }

    matriculeSelectionne.clear();

    QMessageBox::information(
        this, "Succès",
        "Pointage supprimé avec succès."
        );

    afficherPointages();
}
void PointagePage::rechercherPointage()
{
    if (!verifierConnexion())
        return;

    QString recherche = searchPointage->text().trimmed();

    QSqlDatabase db = Database::getConnection();

    QSqlQuery query(db);

    query.prepare(
        "SELECT p.Matricule, e.Nom, "
        "p.Mois, p.Annee, "
        "p.HeuresSup, p.JoursAbsence "
        "FROM POINTAGE p "
        "INNER JOIN EMPLOYE e "
        "ON p.Matricule = e.Matricule "
        "WHERE p.Matricule LIKE :recherche "
        "OR e.Nom LIKE :recherche "
        "ORDER BY p.Annee DESC, p.Mois DESC"
        );

    query.bindValue(
        ":recherche",
        "%" + recherche + "%"
        );

    if (!query.exec()) {
        qDebug() << "Erreur recherche :"
                 << query.lastError().text();
        return;
    }

    tablePointage->setRowCount(0);

    int ligne = 0;

    while (query.next()) {

        tablePointage->insertRow(ligne);

        for (int colonne = 0; colonne < 6; colonne++) {

            tablePointage->setItem(
                ligne,
                colonne,
                new QTableWidgetItem(
                    query.value(colonne).toString()
                    )
                );
        }

        ligne++;
    }

    matriculeSelectionne.clear();
}

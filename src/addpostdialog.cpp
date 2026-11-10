#include "addpostdialog.h"
#include "ui_addpostdialog.h"
#include "QMessageBox"
#include "QSqlQuery"
#include "database.h"
#include "QSqlError"

addPostDialog::addPostDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::addPostDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBoxAddPoste, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
}

addPostDialog::~addPostDialog()
{
    delete ui;
}



void addPostDialog::on_buttonBoxAddPoste_accepted()
{
    // 1. Récupérer les valeurs du formulaire
    QString libelle = ui->nomDuPosteLineEdit->text();
    QString salaire = ui->salaireDeBaseLineEdit->text();
    QString taux = ui->tauxDeHeureSupLineEdit->text();

    // 2. Vérifier les champs
    if (libelle.isEmpty() || salaire.isEmpty() || taux.isEmpty())
    {
        QMessageBox::warning(this, "Erreur",
                             "Veuillez remplir tous les champs.");
        return;
    }

    // 3. Préparer la requête SQL
    QSqlQuery query(Database::getConnection());

    query.prepare(
        "INSERT INTO POSTE "
        "(codePoste, Libelle, SalaireBase, TauxHeureSup) "
        "VALUES (:code, :libelle, :salaire, :taux)"
        );

    // 4. Associer les valeurs
    query.bindValue(":libelle", libelle);
    query.bindValue(":salaire", salaire);
    query.bindValue(":taux", taux);

    // 5. Exécuter la requête
    if (query.exec())
    {
        QMessageBox::information(this, "Succès", "Poste ajouté avec succès.");

        // Vider les champs
        ui->nomDuPosteLineEdit->clear();
        ui->salaireDeBaseLineEdit->clear();
        ui->tauxDeHeureSupLineEdit->clear();
    }
    else
    {
        QMessageBox::critical(this, "Erreur SQL", query.lastError().text());
    }
    accept();
}


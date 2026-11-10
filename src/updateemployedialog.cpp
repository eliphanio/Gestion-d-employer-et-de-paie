#include "updateemployedialog.h"
#include "ui_updateemployedialog.h"
#include "QMessageBox"
#include "database.h"
#include "QSqlQuery"
#include "QSqlError"

updateEmployeDialog::updateEmployeDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::updateEmployeDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBoxUpdateEmploye, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
}

updateEmployeDialog::~updateEmployeDialog()
{
    delete ui;
}

void updateEmployeDialog::rechercherEmploye()
{
    QString matricule = ui->matriculeLineEdit->text().trimmed();

    if (matricule.isEmpty()) {
        QMessageBox::warning(this, "Attention",
                             "Veuillez sélectionner un employé.");
        return;
    }

    if (!Database::connect())
        return;

    QSqlQuery query(Database::getConnection());

    query.prepare(
        "SELECT Nom, Adresse, codePoste "
        "FROM EMPLOYE "
        "WHERE Matricule = :matricule"
        );

    query.bindValue(":matricule", matricule);

    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur SQL",
                              query.lastError().text());
        return;
    }

    if (query.next()) {
        ui->nouvelleAdresseLineEdit->setText(query.value("Adresse").toString());

    } else {
        QMessageBox::warning(this, "Introuvable",
                             "Cet employé n'existe pas.");
    }
}

void updateEmployeDialog::on_buttonBoxUpdateEmploye_accepted()
{
        QString matricule = ui->matriculeLineEdit->text().trimmed();

        QString adresse = ui->nouvelleAdresseLineEdit->text().trimmed();

        if (matricule.isEmpty() ||  adresse.isEmpty()) {
            QMessageBox::warning(this, "Attention",
                                 "Veuillez rechercher un employé et remplir tous les champs.");
            return;
        }

        if (!Database::connect())
            return;

        QSqlQuery query(Database::getConnection());

        query.prepare(
            "UPDATE EMPLOYE SET "
            "Adresse = :adresse "
            "WHERE Matricule = :matricule"
            );

        query.bindValue(":adresse", adresse);
        query.bindValue(":matricule", matricule);

        if (query.exec()) {
            if (query.numRowsAffected() >= 0) {
                QMessageBox::information(this, "Succès",
                                         "Employé modifié avec succès.");
                accept();
            }
        } else {
            if (!query.exec()) {
                qDebug() << "Erreur SQL :" << query.lastError().text();
                qDebug() << "Requête :" << query.lastQuery();
                qDebug() << "Valeurs :" << query.boundValues();

                QMessageBox::critical(
                    this,
                    "Erreur SQL",
                    query.lastError().text()
                    );
            };
        }
}


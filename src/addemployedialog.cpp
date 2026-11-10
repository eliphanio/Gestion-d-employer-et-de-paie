#include "addemployedialog.h"
#include "ui_addemployedialog.h"
#include "comboboxhelper.h"
#include "database.h"
#include "QMessageBox"
#include "QDate"
#include "comboboxhelper.h"
#include "QSqlQuery"
#include "QSqlError"

addEmployeDialog::addEmployeDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::addEmployeDialog)
{
    ui->setupUi(this);
    ComboBoxHelper::chargerPostes(ui->posteEmployLineEdit);
    connect(ui->buttonBoxAddEmploye, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
}

addEmployeDialog::~addEmployeDialog()
{
    delete ui;
}

void addEmployeDialog::on_buttonBoxAddEmploye_accepted()
{
    QString matricule = Database::genererMatricule();

    if (matricule.isEmpty()) {
        QMessageBox::critical(this, "Erreur",
                              "Impossible de générer le matricule.");
        return;
    }


    QString dateEmbauche =
        QDate::currentDate().toString("yyyy-MM-dd");

    QString nom = ui->nomEmployeLineEdit->text();
    QString adresse = ui->adresseLineEdit->text();

    QString codePoste =
        ui->posteEmployLineEdit->currentData().toString();

    if (matricule.isEmpty() || nom.isEmpty() ||
        adresse.isEmpty() || codePoste.isEmpty())
    {
        QMessageBox::warning(this, "Erreur",
                             "Veuillez remplir tous les champs.");
        return;
    }

    if (!Database::connect())
        return;

    QSqlQuery query(Database::getConnection());

    query.prepare(
        "INSERT INTO EMPLOYE "
        "(Matricule, Nom, Adresse, dateEmbauche, codePoste) "
        "VALUES (:matricule, :nom, :adresse, :date, :poste)"
        );

    query.bindValue(":matricule", matricule);
    query.bindValue(":nom", nom);
    query.bindValue(":adresse", adresse);
    query.bindValue(":date", dateEmbauche);
    query.bindValue(":poste", codePoste);

    if (query.exec())
    {
        QMessageBox::information(
            this, "Succès",
            "Employé ajouté avec succès.\nMatricule : " + matricule
            );

        accept();
    }
    else
    {
        QMessageBox::critical(
            this, "Erreur SQL",
            query.lastError().text()
            );
    }
}


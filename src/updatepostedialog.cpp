#include "updatepostedialog.h"
#include "ui_updatepostedialog.h"
#include "comboboxhelper.h"
#include "QMessageBox"
#include "database.h"
#include "QSqlQuery"
#include "QSqlError"


updatePosteDialog::updatePosteDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::updatePosteDialog)
{
    ui->setupUi(this);

    ComboBoxHelper::chargerPostes(ui->nomDuPosteComboBox);

    connect(ui->buttonBoxUpdatePoste, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
}

updatePosteDialog::~updatePosteDialog()
{
    delete ui;
}

void updatePosteDialog::rechercherPoste()
{
    QString codePoste = ui->nomDuPosteComboBox->currentData().toString();

    if (codePoste.isEmpty()) {
        QMessageBox::warning(this, "Attention",
                             "Veuillez sélectionner un poste.");
        return;
    }

    if (!Database::connect())
        return;

    QSqlQuery query(Database::getConnection());

    query.prepare(
        "SELECT Libelle, SalaireBase, TauxHeureSup "
        "FROM POSTE WHERE codePoste = :code"
        );

    query.bindValue(":code", codePoste);

    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur SQL",
                              query.lastError().text());
        return;
    }

    if (query.next()) {
        ui->nouveauSalaireLineEdit->setText(query.value("SalaireBase").toString());
        ui->nouveauTauxHeureSupLineEdit->setText(query.value("TauxHeureSup").toString());
    } else {
        QMessageBox::warning(this, "Introuvable",
                             "Ce poste n'existe pas.");
    }
}

void updatePosteDialog::on_buttonBoxUpdatePoste_accepted()
{
    QString codePoste = ui->nomDuPosteComboBox->currentData().toString();

    QString salaire = ui->nouveauSalaireLineEdit->text().trimmed();
    QString taux = ui->nouveauTauxHeureSupLineEdit->text().trimmed();

    if (codePoste.isEmpty() ||
        salaire.isEmpty() || taux.isEmpty()) {
        QMessageBox::warning(this, "Attention",
                             "Veuillez rechercher un poste et remplir tous les champs.");
        return;
    }

    bool salaireValide, tauxValide;
    double salaireBase = salaire.toDouble(&salaireValide);
    double tauxHeureSup = taux.toDouble(&tauxValide);

    if (!salaireValide || !tauxValide ||
        salaireBase < 0 || tauxHeureSup < 0) {
        QMessageBox::warning(this, "Erreur",
                             "Les valeurs du salaire et du taux doivent être numériques et positives.");
        return;
    }

    if (!Database::connect())
        return;

    QSqlQuery query(Database::getConnection());

    query.prepare(
        "UPDATE POSTE SET "
        "SalaireBase = :salaire, "
        "TauxHeureSup = :taux "
        "WHERE codePoste = :code"
        );

    query.bindValue(":salaire", salaireBase);
    query.bindValue(":taux", tauxHeureSup);
    query.bindValue(":code", codePoste);

    if (query.exec()) {
        QMessageBox::information(this, "Succès",
                                 "Poste modifié avec succès.");
        accept();
    } else {
        QMessageBox::critical(this, "Erreur SQL",
                              query.lastError().text());
    }
}



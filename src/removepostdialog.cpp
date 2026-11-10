#include "removepostdialog.h"
#include "ui_removepostdialog.h"
#include "comboboxhelper.h"
#include "QWidget"
#include "QMessageBox"
#include "database.h"
#include "QSqlQuery"
#include "QSqlError"

removePostDialog::removePostDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::removePostDialog)
{
    ui->setupUi(this);
    ComboBoxHelper::chargerPostes(ui->nomDuPosteComboBox);
    connect(ui->buttonBoxRemovePoste, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
}

removePostDialog::~removePostDialog()
{
    delete ui;
}

void removePostDialog::on_buttonBoxRemovePoste_accepted()
{

        QString codePoste = ui->nomDuPosteComboBox->currentData().toString();

        // Vérifier la sélection
        if (codePoste.isEmpty()) {
            QMessageBox::warning(this, "Attention",
                                 "Veuillez sélectionner un poste.");
            return;
        }

        if (!Database::connect())
            return;

        // Vérifier si le poste existe
        QSqlQuery verification(Database::getConnection());

        verification.prepare(
            "SELECT codePoste FROM POSTE WHERE codePoste = ?"
            );

        verification.addBindValue(codePoste);

        if (!verification.exec()) {
            QMessageBox::critical(this, "Erreur SQL",
                                  verification.lastError().text());
            return;
        }

        if (!verification.next()) {
            QMessageBox::warning(this, "Introuvable",
                                 "Ce poste n'existe pas.");
            return;
        }

        // Demander confirmation
        QMessageBox::StandardButton reponse = QMessageBox::question(
            this,
            "Confirmation",
            "Voulez-vous vraiment supprimer le poste " + codePoste + " ?",
            QMessageBox::Yes | QMessageBox::No
            );

        if (reponse != QMessageBox::Yes)
            return;

        // Supprimer le poste
        QSqlQuery query(Database::getConnection());

        query.prepare("DELETE FROM POSTE WHERE codePoste = ?");
        query.addBindValue(codePoste);


        if (query.exec()) {
            QMessageBox::information(this, "Succès",
                                     "Poste supprimé avec succès.");
            accept();
        } else {
            QString erreur = query.lastError().text();

            if (erreur.contains("fk_employe_poste") ||
                erreur.contains("foreign key constraint")) {

                QMessageBox::warning(
                    this,
                    "Suppression impossible",
                    "Ce poste ne peut pas être supprimé car il est "
                    "associé à un ou plusieurs employés.\n\n"
                    "Veuillez d'abord affecter ces employés à un autre poste."
                    );

            } else {

                QMessageBox::critical(
                    this,
                    "Erreur",
                    "Une erreur est survenue lors de la suppression du poste."
                    );

                qDebug() << "Erreur SQL :" << erreur;
            }
        }
}


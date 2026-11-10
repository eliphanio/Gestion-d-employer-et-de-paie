#include "removeemployedialog.h"
#include "ui_removeemployedialog.h"
#include "comboboxhelper.h"
#include "database.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QStringList>

removeEmployeDialog::removeEmployeDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::removeEmployeDialog)
{
    ui->setupUi(this);
    ComboBoxHelper::chargerMatricules(ui->matriculeEmployComboBox);
    connect(ui->buttonBoxRemoveEmploye, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
}

removeEmployeDialog::~removeEmployeDialog()
{
    delete ui;
}

void removeEmployeDialog::on_buttonBoxRemoveEmploye_accepted()
{
    QString matricule =
        ui->matriculeEmployComboBox->currentData().toString();

    if (matricule.isEmpty()) {
        QMessageBox::warning(
            this,
            "Attention",
            "Veuillez sélectionner un employé."
            );
        return;
    }

    if (!Database::connect())
        return;

    // 1. Vérifier si l'employé possède des pointages
    QSqlQuery verification(Database::getConnection());

    verification.prepare(
        "SELECT Mois, Annee "
        "FROM POINTAGE "
        "WHERE Matricule = ? "
        "ORDER BY Annee DESC, Mois DESC"
        );

    verification.addBindValue(matricule);

    if (!verification.exec()) {
        QMessageBox::critical(
            this,
            "Erreur",
            "Impossible de vérifier les pointages de cet employé."
            );

        qDebug() << verification.lastError().text();
        return;
    }

    // Récupérer les périodes de pointage
    QStringList pointages;

    while (verification.next()) {
        int mois = verification.value("Mois").toInt();
        int annee = verification.value("Annee").toInt();

        pointages << QString("Mois %1 / %2").arg(mois).arg(annee);
    }

    // 2. Empêcher la suppression si des pointages existent
    if (!pointages.isEmpty()) {

        QString message =
            "Impossible de supprimer cet employé.\n\n"
            "Cet employé possède déjà des pointages "
            "enregistrés pour les périodes suivantes :\n\n";

        message += pointages.join("\n");

        message +=
            "\n\nLa suppression est refusée afin de conserver "
            "l'historique de pointage.";

        QMessageBox::warning(
            this,
            "Suppression impossible",
            message
            );

        return;
    }

    // 3. Demander confirmation
    QMessageBox::StandardButton reponse =
        QMessageBox::question(
            this,
            "Confirmation",
            "Voulez-vous vraiment supprimer l'employé "
                + matricule + " ?",
            QMessageBox::Yes | QMessageBox::No
            );

    if (reponse != QMessageBox::Yes)
        return;

    // 4. Supprimer l'employé
    QSqlQuery query(Database::getConnection());

    query.prepare(
        "DELETE FROM EMPLOYE WHERE Matricule = ?"
        );

    query.addBindValue(matricule);

    if (query.exec()) {

        QMessageBox::information(
            this,
            "Succès",
            "L'employé a été supprimé avec succès."
            );

        accept();

    } else {

        QMessageBox::critical(
            this,
            "Suppression impossible",
            "Une erreur est survenue lors de la suppression "
            "de cet employé. Il est peut-être encore associé "
            "à d'autres données."
            );

        qDebug() << "Erreur SQL :"
                 << query.lastError().text();
    }
}


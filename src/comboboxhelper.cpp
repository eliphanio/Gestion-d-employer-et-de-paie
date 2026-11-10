#include "comboboxhelper.h"
#include "database.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

bool ComboBoxHelper::chargerPostes(QComboBox *comboBox)
{
    if (!comboBox)
        return false;

    if (!Database::connect())
        return false;

    QSqlQuery query(Database::getConnection());

    query.prepare(
        "SELECT codePoste, Libelle FROM POSTE ORDER BY Libelle ASC"
        );

    if (!query.exec()) {
        QMessageBox::critical(
            comboBox,
            "Erreur SQL",
            query.lastError().text()
            );
        return false;
    }

    comboBox->clear();

    while (query.next()) {
        QString code = query.value("codePoste").toString();
        QString libelle = query.value("Libelle").toString();

        comboBox->addItem(libelle, code);
    }

    return true;
}



#include "database.h"
#include <QFile>
#include <QSqlError>
#include <QCoreApplication>
#include <QDir>
#include <QSqlQuery>
#include <QDebug>

bool Database::connect()
{
    // Vérifier que le pilote MySQL est disponible
    if (!QSqlDatabase::isDriverAvailable("QODBC")) {
        qDebug() << "Pilote QODBC indisponible.";
        qDebug() << "Pilotes disponibles :"
                 << QSqlDatabase::drivers();
        return false;
    }

    // Créer la connexion
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

    db.setDatabaseName(
        "Driver={MySQL ODBC 26.10 Unicode Driver};"
        "Server=127.0.0.1;"
        "Port=3306;"
        "Database=gestion_paie;"
        "User=root;"
        "Password=;"
        );

    // Ouvrir la connexion
    if (!db.open()) {
        qDebug() << "Connexion échouée :"
                 << db.lastError().text();
        return false;
    }

    qDebug() << "Connexion MySQL réussie !";
    return true;
}

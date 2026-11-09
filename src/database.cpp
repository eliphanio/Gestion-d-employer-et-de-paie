#include "database.h"
#include <QSqlError>
#include <QDebug>

QSqlDatabase Database::db;

bool Database::connect()
{
    if (!QSqlDatabase::isDriverAvailable("QODBC")) {
        qDebug() << "Pilote QODBC indisponible.";
        return false;
    }

    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase("QODBC");
    }

    db.setDatabaseName(
        "Driver={MySQL ODBC 26.10 Unicode Driver};"
        "Server=127.0.0.1;"
        "Port=3306;"
        "Database=gestion_paie;"
        "User=root;"
        "Password=;"
        );

    if (!db.isOpen() && !db.open()) {
        qDebug() << "Erreur :" << db.lastError().text();
        return false;
    }

    qDebug() << "Connexion MySQL réussie !";
    return true;
}

QSqlDatabase Database::getConnection()
{
    return db;
}
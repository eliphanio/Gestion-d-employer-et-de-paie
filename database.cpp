#include "database.h"
#include <QFile>
#include <QSqlError>
#include <QCoreApplication>
#include <QDir>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlDatabase>

bool Database::connect()
{
    // Vérifier si le pilote QMYSQL est disponible
    if (!QSqlDatabase::isDriverAvailable("QMYSQL")) {
        qDebug() << "Pilote QMYSQL indisponible.";
        qDebug() << "Pilotes disponibles :"
                 << QSqlDatabase::drivers();
        return false;
    }

    // Créer la connexion à MariaDB
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");

    // Paramètres de connexion
    db.setHostName("127.0.0.1");
    db.setPort(3306);
    db.setDatabaseName("gestion_paie");
    db.setUserName("gestion_app");
    db.setPassword("Gestion2026!");

    // Ouvrir la connexion
    if (!db.open()) {
        qDebug() << "Connexion échouée :"
                 << db.lastError().text();
        return false;
    }

    qDebug() << "Connexion à MariaDB réussie !";

    return true;
}

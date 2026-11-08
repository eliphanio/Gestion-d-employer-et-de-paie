
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
    if (!QSqlDatabase::isDriverAvailable("QMYSQL")) {
        qDebug() << "Pilote QMYSQL indisponible.";
        qDebug() << "Pilotes disponibles :"
                 << QSqlDatabase::drivers();
        return false;
    }

    // Créer la connexion
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");

    db.setDatabaseName("gestion_paie");

    db.setHostName("127.0.0.1");
    db.setPort(3306);
    db.setUserName("root");
    db.setPassword("");


    // Ouvrir la connexion
    if (!db.open()) {
        qDebug() << "Connexion échouée :"
                 << db.lastError().text();
        return false;
    }

    qDebug() << "Connexion MySQL réussie !";
    return true;
}

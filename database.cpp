
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
        "Driver={MySQL ODBC 26.7 Unicode Driver};"
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



bool Database::initializeSchema()
{
    QString chemin = QDir(QCoreApplication::applicationDirPath())
    .filePath("database/schema.sql");

    QFile file(chemin);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Impossible d'ouvrir schema.sql :"
                 << file.errorString();
        qDebug() << "Chemin recherché :" << chemin;
        return false;
    }

    QString script = QString::fromUtf8(file.readAll());
    file.close();

    const QStringList statements =
        script.split(';', Qt::SkipEmptyParts);

    QSqlQuery query;

    for (const QString &statement : statements) {
        QString sql = statement.trimmed();

        if (sql.isEmpty()) {
            continue;
        }

        if (!query.exec(sql)) {
            qDebug() << "Erreur SQL :"
                     << query.lastError().text();
            qDebug() << "Requête :" << sql;
            return false;
        }
    }

    qDebug() << "Tables initialisées avec succès.";
    return true;
}

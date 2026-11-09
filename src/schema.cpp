
#include "database.h"
#include <QFile>
#include <QSqlError>
#include <QCoreApplication>
#include <QDir>
#include <QSqlQuery>
#include <QDebug>


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

    QSqlQuery query(Database::getConnection());

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

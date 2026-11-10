#include "database.h"
#include <QSqlError>
#include <QDebug>
#include <QFile>
#include <QCoreApplication>
#include <QSqlQuery>
#include <QStringList>
#include <QIODevice>
#include <QHash>

QSqlDatabase Database::db;

namespace {

// Lit une valeur dans le fichier .env, sinon dans les variables d'environnement
QString valeurEnv(const QString &cle)
{
    static QHash<QString, QString> env;
    static bool charge = false;

    if (!charge) {
        charge = true;

        QStringList chemins = { QCoreApplication::applicationDirPath() + "/.env" };
#ifdef PROJECT_DIR
        chemins << QString(PROJECT_DIR) + "/.env";
#endif

        for (const QString &chemin : chemins) {
            QFile f(chemin);
            if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                continue;
            }

            while (!f.atEnd()) {
                const QString ligne = QString::fromUtf8(f.readLine()).trimmed();
                if (ligne.isEmpty() || ligne.startsWith('#')) {
                    continue;
                }
                const int i = ligne.indexOf('=');
                if (i <= 0) {
                    continue;
                }
                env.insert(ligne.left(i).trimmed(), ligne.mid(i + 1).trimmed());
            }
            break;
        }
    }

    if (env.contains(cle)) {
        return env.value(cle);
    }
    return qEnvironmentVariable(cle.toUtf8().constData());
}

} // namespace

bool Database::connect()
{
    if (!QSqlDatabase::isDriverAvailable("QODBC")) {
        qDebug() << "Pilote QODBC indisponible.";
        return false;
    }

    if (valeurEnv("DB_USER").isEmpty()) {
        qDebug() << "Fichier .env introuvable ou incomplet (DB_USER manquant).";
        return false;
    }

    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase("QODBC");
    }

    db.setDatabaseName(
        QString("Driver={%1};Server=%2;Port=%3;Database=%4;User=%5;Password=%6;")
            .arg(valeurEnv("DB_DRIVER"),
                 valeurEnv("DB_HOST"),
                 valeurEnv("DB_PORT"),
                 valeurEnv("DB_NAME"),
                 valeurEnv("DB_USER"),
                 valeurEnv("DB_PASSWORD")));

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
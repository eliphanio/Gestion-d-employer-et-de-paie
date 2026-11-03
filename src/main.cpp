#include "mainwindow.h"

#include "database.h"
#include <QMessageBox>

#include <QApplication>
#include <QLocale>
#include <QLabel>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Se connecter avant d'afficher la fenêtre principale
    if (!Database::connect()) {
        QMessageBox::critical(
            nullptr,
            "Erreur",
            "Connexion à la base de donnée échouée"
            );

        return 1;

    }

    if (!Database::initializeSchema()) {
        QMessageBox::critical(
            nullptr,
            "Erreur",
            "Impossible de créer les tables."
            );
        return 1;
    }

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "GestionEmployer_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    MainWindow w;
    w.show();
    return QApplication::exec();
}

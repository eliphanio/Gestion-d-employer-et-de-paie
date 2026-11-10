#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "frontend.h"
#include "addpostdialog.h"
#include "removepostdialog.h"
#include "updatepostedialog.h"
#include "addemployedialog.h"
#include "updateemployedialog.h"
#include "removeemployedialog.h"
#include "QLabel"
#include "QPushButton"
#include "QSqlQuery"
#include <QSqlError>
#include <QVBoxLayout>
#include "database.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto *layoutCentral = new QHBoxLayout(ui->centralwidget);   // à ajouter
    layoutCentral->setContentsMargins(0, 0, 0, 0);      // à ajouter
    layoutCentral->addWidget(ui->horizontalLayoutWidget);   // à ajouter

    auto *layoutContainer = new QVBoxLayout(ui->container);     // à ajouter
    layoutContainer->setContentsMargins(0, 0, 0, 0);    // à ajouter
    layoutContainer->addWidget(ui->stackedWidgetContente);  // à ajouter 

    resize(1000, 700); // à ajouter


    connect(ui->searchBar,
            &QLineEdit::textChanged,
            this,
            &MainWindow::rechercherEmploye);

    nEmploye();
    nPoste();
    affichePoste();
    afficheEmploye();
    affichePosteByEmploye();

    // Page affichée au démarrage
    ui->stackedWidgetContente->setCurrentIndex(0);

    // Navigation du sidebar
    connect(ui->menuPoste, &QPushButton::clicked,
            this, [this]() {
                ui->stackedWidgetContente->setCurrentWidget(ui->postePage);
            });

    connect(ui->menuEmploye, &QPushButton::clicked,
            this, [this]() {
                ui->stackedWidgetContente->setCurrentWidget(ui->employePage);
            });

    connect(ui->menuPointage, &QPushButton::clicked,
            this, [this]() {
                ui->stackedWidgetContente->setCurrentWidget(ui->pointagePage);
            });

    connect(ui->menuPaie, &QPushButton::clicked, this, [this]() {
        afficherPagePaie(new BulletinDialog);
    });

    connect(ui->menuMasseSalariale, &QPushButton::clicked, this, [this]() {
        afficherPagePaie(new MasseSalarialeDialog);
    });

    connect(ui->menuEmployeByPoste, &QPushButton::clicked, this, [this]() {
                ui->stackedWidgetContente->setCurrentWidget(ui->posteEmployePage);
            });

    connect(ui->menuQuitte, &QPushButton::clicked, this, &MainWindow::quitClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::nEmploye()
{
    db.open();
    QSqlQuery Query_Count(db);

    Query_Count.prepare(
        "SELECT COUNT(*) AS nEmploye FROM employe"
        );

    if (Query_Count.exec() && Query_Count.next()) {

        int nombre = Query_Count.value("nEmploye").toInt();

        ui->labelNombreEmploye->setText(
            QString("Effectif : %1").arg(nombre)
            );
    }
    db.close();
}

void MainWindow::nPoste()
{
    db.open();
    QSqlQuery Query_Count(db);

    Query_Count.prepare(
        "SELECT COUNT(*) AS nPoste FROM poste"
        );

    if (Query_Count.exec() && Query_Count.next()) {

        int nombre = Query_Count.value("nPoste").toInt();

        ui->labelNombrePoste->setText(
            QString("Effectif : %1").arg(nombre)
            );
    }
    db.close();
}

void MainWindow::afficheEmploye()
{

    db.open();

    QSqlQuery Query_Get_Data(db);

    Query_Get_Data.prepare("SELECT Matricule, Nom, Adresse, dateEmbauche FROM employe ORDER BY employe.Matricule ASC;");
    if(Query_Get_Data.exec()){

        ui->tableWidgetEmploye->setRowCount(0);
        ui->tableWidgetEmploye->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        int rowNumber = 0;

        while(Query_Get_Data.next()){
            ui->tableWidgetEmploye->insertRow(rowNumber);
            ui->tableWidgetEmploye->setItem(rowNumber, 0, new QTableWidgetItem(QString(Query_Get_Data.value("Matricule").toString())));
            ui->tableWidgetEmploye->setItem(rowNumber, 1, new QTableWidgetItem(QString(Query_Get_Data.value("Nom").toString())));
            ui->tableWidgetEmploye->setItem(rowNumber, 2, new QTableWidgetItem(QString(Query_Get_Data.value("Adresse").toString())));
            ui->tableWidgetEmploye->setItem(rowNumber, 3, new QTableWidgetItem(QString(Query_Get_Data.value("dateEmbauche").toString())));

            rowNumber = rowNumber + 1;

        }
    }
    db.close();

}

void MainWindow::affichePoste()
{
    db.open();

    QSqlQuery Query_Get_Data(db);
    Query_Get_Data.prepare("SELECT Libelle, SalaireBase, TauxHeureSup FROM poste ORDER BY poste.Libelle ASC;");
    if(Query_Get_Data.exec()){

        ui->tableWidgetPoste->setRowCount(0);
        ui->tableWidgetPoste->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        int rowNumber = 0;

        while(Query_Get_Data.next()){
            ui->tableWidgetPoste->insertRow(rowNumber);
            ui->tableWidgetPoste->setItem(rowNumber, 0, new QTableWidgetItem(QString(Query_Get_Data.value("Libelle").toString())));
            ui->tableWidgetPoste->setItem(rowNumber, 1, new QTableWidgetItem(QString::number(Query_Get_Data.value("SalaireBase").toInt())));
            ui->tableWidgetPoste->setItem(rowNumber, 2, new QTableWidgetItem(QString::number(Query_Get_Data.value("TauxHeureSup").toInt())));

            rowNumber = rowNumber + 1;

        }
    }
    db.close();
}

void MainWindow::affichePosteByEmploye()
{
    db.open();

    QSqlQuery Query_Get_Data(db);
    Query_Get_Data.prepare("SELECT employe.Matricule, employe.Nom, poste.Libelle, poste.SalaireBase, employe.dateEmbauche FROM employe INNER JOIN poste ON employe.codePoste = poste.codePoste ORDER BY poste.Libelle ASC;");
    if(Query_Get_Data.exec()){

        ui->tableWidgetPosteByEmploye->setRowCount(0);
        ui->tableWidgetPosteByEmploye->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        int rowNumber = 0;

        while(Query_Get_Data.next()){

            ui->tableWidgetPosteByEmploye->insertRow(rowNumber);
            ui->tableWidgetPosteByEmploye->setItem(rowNumber, 0, new QTableWidgetItem(QString(Query_Get_Data.value("Matricule").toString())));
            ui->tableWidgetPosteByEmploye->setItem(rowNumber, 1, new QTableWidgetItem(QString(Query_Get_Data.value("Nom").toString())));
            ui->tableWidgetPosteByEmploye->setItem(rowNumber, 2, new QTableWidgetItem(QString(Query_Get_Data.value("Libelle").toString())));
            ui->tableWidgetPosteByEmploye->setItem(rowNumber, 3, new QTableWidgetItem(QString::number(Query_Get_Data.value("SalaireBase").toInt())));
            ui->tableWidgetPosteByEmploye->setItem(rowNumber, 4, new QTableWidgetItem(QString(Query_Get_Data.value("dateEmbauche").toString())));

            rowNumber = rowNumber + 1;

        }
    }
    db.close();
}



void MainWindow::afficherPagePaie(QWidget *page)
{
    // supprime la page de paie précédente pour éviter d'en accumuler
    if (pagePaieCourante) {
        ui->stackedWidgetContente->removeWidget(pagePaieCourante);
        pagePaieCourante->deleteLater();
    }
    pagePaieCourante = page;

    ui->stackedWidgetContente->addWidget(page);
    ui->stackedWidgetContente->setCurrentWidget(page);
}


void MainWindow::rechercherEmploye()
{
    QString recherche = ui->searchBar->text().trimmed();

    // Récupérer la connexion partagée
    QSqlDatabase connexion = Database::getConnection();

    // Vérifier la connexion
    if (!connexion.isOpen()) {
        qDebug() << "Connexion MySQL fermée.";

        if (!Database::connect()) {
            qDebug() << "Impossible de se connecter à MySQL.";
            return;
        }

        connexion = Database::getConnection();
    }

    QSqlQuery query(connexion);

    query.prepare(
        "SELECT Matricule, Nom, Adresse, dateEmbauche "
        "FROM employe "
        "WHERE Nom LIKE :recherche "
        "OR Matricule LIKE :recherche "
        "ORDER BY Matricule ASC"
        );

    query.bindValue(":recherche", "%" + recherche + "%");

    if (!query.exec()) {
        qDebug() << "Erreur SQL :" << query.lastError().text();
        return;
    }

    ui->tableWidgetEmploye->setSortingEnabled(false);
    ui->tableWidgetEmploye->setRowCount(0);

    int ligne = 0;

    while (query.next()) {
        ui->tableWidgetEmploye->insertRow(ligne);

        ui->tableWidgetEmploye->setItem(ligne, 0, new QTableWidgetItem(query.value("Matricule").toString()));

        ui->tableWidgetEmploye->setItem(ligne, 1, new QTableWidgetItem(query.value("Nom").toString()));

        ui->tableWidgetEmploye->setItem(ligne, 2, new QTableWidgetItem(query.value("Adresse").toString()));

        ui->tableWidgetEmploye->setItem(ligne, 3, new QTableWidgetItem(query.value("dateEmbauche").toString()));

        ligne++;
    }

    ui->tableWidgetEmploye->setSortingEnabled(true);
}


void MainWindow::quitClicked()
{
    QApplication::quit();
}


void MainWindow::on_btnAddPost_clicked()
{
    addPostDialog dialog(this);
    dialog.exec();
}


void MainWindow::on_btnRemovePost_clicked()
{
    removePostDialog dialog(this);
    dialog.exec();
}


void MainWindow::on_btnUpdatePost_clicked()
{
    updatePosteDialog dialog(this);
    dialog.exec();
}


void MainWindow::on_btnAddEmploye_clicked()
{
    addEmployeDialog dialog(this);
    dialog.exec();
}


void MainWindow::on_btnUpdateEmploye_clicked()
{
    updateEmployeDialog dialog(this);
    dialog.exec();
}


void MainWindow::on_btnRemoveEmploye_clicked()
{
    removeEmployeDialog dialog(this);
    dialog.exec();
}

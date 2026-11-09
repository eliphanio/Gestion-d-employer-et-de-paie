#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "ecrans_paie.h"
#include "QLabel"
#include "QPushButton"
#include "QSqlQuery"
#include <QSqlError>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
        afficherPagePaie(new EmployesParPosteDialog);
    });

<<<<<<< HEAD
=======
    connect(ui->menuMasseSalariale, &QPushButton::clicked,
            this, [this]() {
                MasseSalarialeDialog dlg(this);
                dlg.exec();
            });
    connect(ui->menuEmployeByPoste, &QPushButton::clicked,
            this, [this]() {
                ui->stackedWidgetContente->setCurrentWidget(ui->posteEmployePage);
            });

>>>>>>> main
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

<<<<<<< HEAD
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
=======
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
>>>>>>> main
}

void MainWindow::quitClicked()
{
    QApplication::quit();
}
#include "mainwindow.h"
#include "./ui_mainwindow.h"
<<<<<<< HEAD
#include "frontend.h"
=======
#include "ecrans_paie.h"
#include "pointage.h"
#include "QLabel"
#include "QPushButton"
#include "QSqlQuery"
#include <QSqlError>
>>>>>>> 8069aa8 (crud employe/poste)
#include "addpostdialog.h"
#include "removepostdialog.h"
#include "updatepostedialog.h"
#include "addemployedialog.h"
#include "updateemployedialog.h"
#include "removeemployedialog.h"
#include "database.h"

#include <QApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlError>
#include <QSqlQuery>
#include <QTableWidgetItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto *layoutCentral = new QHBoxLayout(ui->centralwidget);
    layoutCentral->setContentsMargins(0, 0, 0, 0);
    layoutCentral->addWidget(ui->horizontalLayoutWidget);

    auto *layoutContainer = new QVBoxLayout(ui->container);
    layoutContainer->setContentsMargins(0, 0, 0, 0);
    layoutContainer->addWidget(ui->stackedWidgetContente);

    resize(1000, 700);

    connect(ui->searchBar, &QLineEdit::textChanged,
            this, &MainWindow::rechercherEmploye);

    // Ouvre la connexion MySQL avant les premières requêtes
    if (!Database::getConnection().isOpen()) {
        Database::connect();
    }

    nEmploye();
    nPoste();
    affichePoste();
    afficheEmploye();
    affichePosteByEmploye();

    ui->stackedWidgetContente->setCurrentIndex(0);

    connect(ui->menuPoste, &QPushButton::clicked, this, [this]() {
        ui->stackedWidgetContente->setCurrentWidget(ui->postePage);
    });

    connect(ui->menuEmploye, &QPushButton::clicked, this, [this]() {
        ui->stackedWidgetContente->setCurrentWidget(ui->employePage);
    });

<<<<<<< HEAD
    connect(ui->menuPointage, &QPushButton::clicked, this, [this]() {
        ui->stackedWidgetContente->setCurrentWidget(ui->pointagePage);
    });
=======
    connect(ui->menuPointage, &QPushButton::clicked,
            this, [this]() {
            afficherPagePaie(new PointageWidget);
        });
>>>>>>> 8069aa8 (crud employe/poste)

    connect(ui->menuPaie, &QPushButton::clicked, this, [this]() {
        afficherPagePaie(new BulletinDialog);
    });

    connect(ui->menuMasseSalariale, &QPushButton::clicked, this, [this]() {
        afficherPagePaie(new MasseSalarialeDialog);
    });

    connect(ui->menuEmployeByPoste, &QPushButton::clicked, this, [this]() {
        ui->stackedWidgetContente->setCurrentWidget(ui->posteEmployePage);
    });

    connect(ui->menuQuitte, &QPushButton::clicked,
            this, &MainWindow::quitClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::nEmploye()
{
    QSqlQuery q(Database::getConnection());

    if (q.exec("SELECT COUNT(*) FROM EMPLOYE") && q.next()) {
        ui->labelNombreEmploye->setText(
            QString("Effectif : %1").arg(q.value(0).toInt()));
    }
}

void MainWindow::nPoste()
{
    QSqlQuery q(Database::getConnection());

    if (q.exec("SELECT COUNT(*) FROM POSTE") && q.next()) {
        ui->labelNombrePoste->setText(
            QString("Effectif : %1").arg(q.value(0).toInt()));
    }
}

void MainWindow::afficheEmploye()
{
    QSqlQuery q(Database::getConnection());

    if (!q.exec("SELECT Matricule, Nom, Adresse, dateEmbauche "
                "FROM EMPLOYE ORDER BY Matricule ASC")) {
        qDebug() << "Erreur SQL :" << q.lastError().text();
        return;
    }

    ui->tableWidgetEmploye->setRowCount(0);
    ui->tableWidgetEmploye->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    int row = 0;
    while (q.next()) {
        ui->tableWidgetEmploye->insertRow(row);
        ui->tableWidgetEmploye->setItem(row, 0, new QTableWidgetItem(q.value("Matricule").toString()));
        ui->tableWidgetEmploye->setItem(row, 1, new QTableWidgetItem(q.value("Nom").toString()));
        ui->tableWidgetEmploye->setItem(row, 2, new QTableWidgetItem(q.value("Adresse").toString()));
        ui->tableWidgetEmploye->setItem(row, 3, new QTableWidgetItem(q.value("dateEmbauche").toString()));
        ++row;
    }
}

void MainWindow::affichePoste()
{
    QSqlQuery q(Database::getConnection());

    if (!q.exec("SELECT Libelle, SalaireBase, TauxHeureSup "
                "FROM POSTE ORDER BY Libelle ASC")) {
        qDebug() << "Erreur SQL :" << q.lastError().text();
        return;
    }

    ui->tableWidgetPoste->setRowCount(0);
    ui->tableWidgetPoste->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    int row = 0;
    while (q.next()) {
        ui->tableWidgetPoste->insertRow(row);
        ui->tableWidgetPoste->setItem(row, 0, new QTableWidgetItem(q.value("Libelle").toString()));
        ui->tableWidgetPoste->setItem(row, 1, new QTableWidgetItem(QString::number(q.value("SalaireBase").toInt())));
        ui->tableWidgetPoste->setItem(row, 2, new QTableWidgetItem(QString::number(q.value("TauxHeureSup").toInt())));
        ++row;
    }
}

void MainWindow::affichePosteByEmploye()
{
    QSqlQuery q(Database::getConnection());

    if (!q.exec("SELECT E.Matricule, E.Nom, P.Libelle, P.SalaireBase, E.dateEmbauche "
                "FROM EMPLOYE E INNER JOIN POSTE P ON E.codePoste = P.codePoste "
                "ORDER BY P.Libelle ASC")) {
        qDebug() << "Erreur SQL :" << q.lastError().text();
        return;
    }

    ui->tableWidgetPosteByEmploye->setRowCount(0);
    ui->tableWidgetPosteByEmploye->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    int row = 0;
    while (q.next()) {
        ui->tableWidgetPosteByEmploye->insertRow(row);
        ui->tableWidgetPosteByEmploye->setItem(row, 0, new QTableWidgetItem(q.value("Matricule").toString()));
        ui->tableWidgetPosteByEmploye->setItem(row, 1, new QTableWidgetItem(q.value("Nom").toString()));
        ui->tableWidgetPosteByEmploye->setItem(row, 2, new QTableWidgetItem(q.value("Libelle").toString()));
        ui->tableWidgetPosteByEmploye->setItem(row, 3, new QTableWidgetItem(QString::number(q.value("SalaireBase").toInt())));
        ui->tableWidgetPosteByEmploye->setItem(row, 4, new QTableWidgetItem(q.value("dateEmbauche").toString()));
        ++row;
    }
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

    QSqlDatabase connexion = Database::getConnection();

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
        "FROM EMPLOYE "
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
    if (dialog.exec() == QDialog::Accepted) {
        affichePoste();
        affichePosteByEmploye();
        nPoste();
    }
}

void MainWindow::on_btnRemovePost_clicked()
{
    removePostDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        affichePoste();
        affichePosteByEmploye();
        nPoste();
    }
}

void MainWindow::on_btnUpdatePost_clicked()
{
    updatePosteDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        affichePoste();
        affichePosteByEmploye();
        nPoste();
    }
}

void MainWindow::on_btnAddEmploye_clicked()
{
    addEmployeDialog dialog(this);
    dialog.exec();
    if (dialog.exec() == QDialog::Accepted) {
        afficheEmploye();
        affichePosteByEmploye();
        nEmploye();
    }
}

void MainWindow::on_btnUpdateEmploye_clicked()
{
    updateEmployeDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        afficheEmploye();
        affichePosteByEmploye();
        nEmploye();
    }
}

void MainWindow::on_btnRemoveEmploye_clicked()
{
    removeEmployeDialog dialog(this);
<<<<<<< HEAD
    dialog.exec();
}
=======
    if (dialog.exec() == QDialog::Accepted) {
        afficheEmploye();
        affichePosteByEmploye();
        nEmploye();
    }
}
>>>>>>> 8069aa8 (crud employe/poste)

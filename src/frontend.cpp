#include "frontend.h"

#include <QWidget>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QDate>
#include <QLocale>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>
#include <QStringList>
#include <QAbstractItemView>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTableWidgetItem>

// =====================================================
// CONSTRUCTEUR
// =====================================================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Gestion des Employés et de la Paie");
    resize(1450, 900);
    setMinimumSize(1100, 700);

    // Aucun bulletin n'est encore calculé.
    setProperty("bulletinValide", false);

    // Style général de la fenêtre.
    setStyleSheet(R"(
        QMainWindow {
            background-color: #F3F6FB;
        }

        QGroupBox {
            background-color: white;
            border: 1px solid #D5DFED;
            border-radius: 9px;
            margin-top: 14px;
            padding: 14px;
            font-size: 15px;
            font-weight: bold;
            color: #173B70;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 6px;
        }

        QLineEdit, QComboBox {
            background-color: white;
            border: 1px solid #C7D2E1;
            border-radius: 6px;
            padding: 9px;
            min-height: 22px;
            font-size: 14px;
        }

        QLineEdit:focus, QComboBox:focus {
            border: 2px solid #2874CC;
        }

        QPushButton {
            background-color: #E6ECF5;
            color: #173B70;
            border: none;
            border-radius: 6px;
            padding: 10px 15px;
            font-weight: bold;
        }

        QPushButton:hover {
            background-color: #D2E1F5;
        }

        QTableWidget {
            background-color: white;
            alternate-background-color: #F1F5FA;
            border: 1px solid #D5DFED;
            gridline-color: #D5DFED;
            font-size: 13px;
        }

        QHeaderView::section {
            background-color: #1D3D70;
            color: white;
            padding: 10px;
            font-weight: bold;
            border: none;
        }
    )");

    // Conteneur principal.
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // =================================================
    // MENU LATÉRAL
    // =================================================

    QWidget *sidebar = new QWidget;
    sidebar->setFixedWidth(240);
    sidebar->setStyleSheet(
        "background-color: #14294B;"
    );

    QVBoxLayout *sideLayout = new QVBoxLayout(sidebar);
    sideLayout->setContentsMargins(15, 35, 15, 20);
    sideLayout->setSpacing(20);

    QLabel *logo = new QLabel("PaiePro");
    logo->setAlignment(Qt::AlignCenter);
    logo->setStyleSheet(
        "color: white;"
        "font-size: 30px;"
        "font-weight: bold;"
    );

    QLabel *description = new QLabel(
        "Gérer • Calculer • Payer"
    );
    description->setAlignment(Qt::AlignCenter);
    description->setStyleSheet(
        "color: #DCE6F5;"
        "font-size: 13px;"
    );

    btnBulletin = new QPushButton("Bulletin de paie");
    btnMasse = new QPushButton("Masse salariale");

    QString menuStyle =
        "QPushButton {"
        "color: white;"
        "background: transparent;"
        "border: none;"
        "padding: 15px;"
        "text-align: left;"
        "font-size: 15px;"
        "border-radius: 8px;"
        "}"
        "QPushButton:hover {"
        "background-color: #28558F;"
        "}";

    btnBulletin->setStyleSheet(menuStyle);
    btnMasse->setStyleSheet(menuStyle);

    sideLayout->addWidget(logo);
    sideLayout->addWidget(description);
    sideLayout->addSpacing(35);
    sideLayout->addWidget(btnBulletin);
    sideLayout->addWidget(btnMasse);
    sideLayout->addStretch();

    QLabel *version = new QLabel(
        "Version 1.0\nENI Fianarantsoa"
    );
    version->setAlignment(Qt::AlignCenter);
    version->setStyleSheet("color: #AABBD3;");

    sideLayout->addWidget(version);

    // =================================================
    // PAGES CENTRALES
    // =================================================

    pages = new QStackedWidget;

    pages->addWidget(creerPageBulletin());
    pages->addWidget(creerPageMasse());

    // Les quatre listes déroulantes existent maintenant.
    initialiserPeriodes();

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(pages, 1);

    // =================================================
    // NAVIGATION
    // =================================================

    connect(
        btnBulletin,
        &QPushButton::clicked,
        this,
        [this]() {
            pages->setCurrentIndex(0);
        }
    );

    connect(
        btnMasse,
        &QPushButton::clicked,
        this,
        [this]() {
            pages->setCurrentIndex(1);
        }
    );
}


// =====================================================
// INITIALISATION DES PÉRIODES
// =====================================================

void MainWindow::initialiserPeriodes()
{
    QStringList mois = {
        "Janvier", "Février", "Mars",
        "Avril", "Mai", "Juin",
        "Juillet", "Août", "Septembre",
        "Octobre", "Novembre", "Décembre"
    };

    const QDate dateActuelle = QDate::currentDate();

    const int anneeActuelle = dateActuelle.year();

    // Éviter d'ajouter plusieurs fois les mêmes éléments.
    moisBulletin->clear();
    moisMasse->clear();

    anneeBulletin->clear();
    anneeMasse->clear();

    moisBulletin->addItems(mois);
    moisMasse->addItems(mois);

    for (int annee = anneeActuelle - 5;
         annee <= anneeActuelle + 5;
         ++annee) {

        const QString valeur = QString::number(annee);

        anneeBulletin->addItem(valeur);
        anneeMasse->addItem(valeur);
    }

    const int indexMois = dateActuelle.month() - 1;

    moisBulletin->setCurrentIndex(indexMois);
    moisMasse->setCurrentIndex(indexMois);

    const QString annee = QString::number(anneeActuelle);

    anneeBulletin->setCurrentText(annee);
    anneeMasse->setCurrentText(annee);
}


// =====================================================
// FORMATAGE DES MONTANTS
// =====================================================

QString MainWindow::montant(double valeur) const
{
    return QLocale(QLocale::French, QLocale::Madagascar)
        .toString(valeur, 'f', 0);
}


// =====================================================
// CRÉATION DE LA PAGE BULLETIN
// =====================================================

QWidget *MainWindow::creerPageBulletin()
{
    QWidget *page = new QWidget;

    QVBoxLayout *layout = new QVBoxLayout(page);

    layout->setContentsMargins(25, 20, 25, 20);
    layout->setSpacing(15);

    QLabel *titre = new QLabel(
        "Édition d'un bulletin de paie"
    );

    titre->setStyleSheet(
        "font-size: 23px;"
        "font-weight: bold;"
        "color: #173B70;"
    );

    // =================================================
    // INFORMATIONS EMPLOYÉ
    // =================================================

    QGroupBox *infos = new QGroupBox(
        "Informations de l'employé"
    );

    QGridLayout *form = new QGridLayout(infos);

    matriculeEdit = new QLineEdit;
    nomEdit = new QLineEdit;
    posteEdit = new QLineEdit;

    nomEdit->setReadOnly(true);
    posteEdit->setReadOnly(true);

    matriculeEdit->setPlaceholderText("Ex : EMP001");

    btnRechercher = new QPushButton("Rechercher");

    moisBulletin = new QComboBox;
    anneeBulletin = new QComboBox;

    form->addWidget(new QLabel("Matricule :"), 0, 0);
    form->addWidget(matriculeEdit, 0, 1);
    form->addWidget(btnRechercher, 0, 2);

    form->addWidget(new QLabel("Nom :"), 1, 0);
    form->addWidget(nomEdit, 1, 1, 1, 2);

    form->addWidget(new QLabel("Poste :"), 2, 0);
    form->addWidget(posteEdit, 2, 1, 1, 2);

    form->addWidget(new QLabel("Mois :"), 3, 0);
    form->addWidget(moisBulletin, 3, 1);

    form->addWidget(new QLabel("Année :"), 3, 2);
    form->addWidget(anneeBulletin, 3, 3);

    // =================================================
    // CALCUL DU BULLETIN
    // =================================================

    QGroupBox *calcul = new QGroupBox(
        "Calcul du bulletin"
    );

    QGridLayout *grid = new QGridLayout(calcul);

    auto ajouterLigne = [&](
        int ligne,
        const QString &nom,
        QLabel *&valeur
    ) {
        grid->addWidget(new QLabel(nom), ligne, 0);

        valeur = new QLabel("0 Ar");
        valeur->setAlignment(Qt::AlignRight);

        grid->addWidget(valeur, ligne, 1);
    };

    ajouterLigne(0, "Salaire de base :", salaireBaseLabel);
    ajouterLigne(1, "Prime d'ancienneté :", primeLabel);
    ajouterLigne(2, "Heures supplémentaires :", heuresSupLabel);
    ajouterLigne(3, "Brut :", brutLabel);
    ajouterLigne(4, "Retenue absence :", retenueAbsenceLabel);
    ajouterLigne(5, "Retenue sociale :", retenueSocialeLabel);
    ajouterLigne(6, "Net à payer :", netLabel);

    netLabel->setStyleSheet(
        "color: #087A35;"
        "font-size: 19px;"
        "font-weight: bold;"
    );

    // =================================================
    // APERÇU DU BULLETIN
    // =================================================

    QGroupBox *apercu = new QGroupBox(
        "Aperçu du bulletin"
    );

    QVBoxLayout *apercuLayout = new QVBoxLayout(apercu);

    tableauBulletin = new QTableWidget(7, 2);

    tableauBulletin->setHorizontalHeaderLabels({
        "Rubrique",
        "Montant (Ar)"
    });

    tableauBulletin->horizontalHeader()
        ->setSectionResizeMode(0, QHeaderView::Stretch);

    tableauBulletin->horizontalHeader()
        ->setSectionResizeMode(1, QHeaderView::Stretch);

    tableauBulletin->verticalHeader()->hide();

    tableauBulletin->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );

    tableauBulletin->setAlternatingRowColors(true);

    QStringList rubriques = {
        "Salaire de base",
        "Prime d'ancienneté",
        "Heures supplémentaires",
        "BRUT",
        "Retenue absence",
        "Retenue sociale",
        "NET À PAYER"
    };

    for (int i = 0; i < rubriques.size(); ++i) {

        tableauBulletin->setItem(
            i,
            0,
            new QTableWidgetItem(rubriques[i])
        );

        tableauBulletin->setItem(
            i,
            1,
            new QTableWidgetItem("0")
        );
    }

    apercuLayout->addWidget(tableauBulletin);

    // =================================================
    // BOUTONS
    // =================================================

    btnCalculer = new QPushButton(
        "Calculer le bulletin"
    );

    btnImprimer = new QPushButton("Imprimer");

    btnReinitialiser = new QPushButton(
        "Réinitialiser"
    );

    btnCalculer->setStyleSheet(
        "background-color: #168447;"
        "color: white;"
        "padding: 10px;"
        "border-radius: 6px;"
    );

    btnImprimer->setStyleSheet(
        "background-color: #1858AA;"
        "color: white;"
        "padding: 10px;"
        "border-radius: 6px;"
    );

    QHBoxLayout *actions = new QHBoxLayout;

    actions->addWidget(btnReinitialiser);
    actions->addStretch();
    actions->addWidget(btnImprimer);
    actions->addWidget(btnCalculer);

    // =================================================
    // ORGANISATION FINALE
    // =================================================

    QHBoxLayout *contenu = new QHBoxLayout;

    contenu->addWidget(calcul, 1);
    contenu->addWidget(apercu, 1);

    layout->addWidget(titre);
    layout->addWidget(infos);
    layout->addLayout(contenu, 1);
    layout->addLayout(actions);

    // =================================================
    // CONNEXIONS
    // =================================================

    connect(
        btnRechercher,
        &QPushButton::clicked,
        this,
        &MainWindow::afficherBulletin
    );

    connect(
        btnCalculer,
        &QPushButton::clicked,
        this,
        &MainWindow::afficherBulletin
    );

    connect(
        btnReinitialiser,
        &QPushButton::clicked,
        this,
        &MainWindow::reinitialiserBulletin
    );

    connect(
        btnImprimer,
        &QPushButton::clicked,
        this,
        &MainWindow::imprimerBulletin
    );

    // Un changement de matricule ou de période invalide
    // le bulletin précédemment calculé.
    connect(
        matriculeEdit,
        &QLineEdit::textChanged,
        this,
        [this]() {
            setProperty("bulletinValide", false);
        }
    );

    connect(
        moisBulletin,
        &QComboBox::currentIndexChanged,
        this,
        [this](int) {
            setProperty("bulletinValide", false);
        }
    );

    connect(
        anneeBulletin,
        &QComboBox::currentIndexChanged,
        this,
        [this](int) {
            setProperty("bulletinValide", false);
        }
    );

    return page;
}


// =====================================================
// CRÉATION DE LA PAGE MASSE SALARIALE
// =====================================================

QWidget *MainWindow::creerPageMasse()
{
    QWidget *page = new QWidget;

    QVBoxLayout *layout = new QVBoxLayout(page);

    layout->setContentsMargins(25, 20, 25, 20);
    layout->setSpacing(20);

    QLabel *titre = new QLabel(
        "Masse salariale d'un mois par poste"
    );

    titre->setStyleSheet(
        "font-size: 23px;"
        "font-weight: bold;"
        "color: #173B70;"
    );

    // =================================================
    // SÉLECTION DE LA PÉRIODE
    // =================================================

    QGroupBox *periode = new QGroupBox(
        "Période de calcul"
    );

    QHBoxLayout *periodeLayout = new QHBoxLayout(periode);

    moisMasse = new QComboBox;
    anneeMasse = new QComboBox;

    btnCalculerMasse = new QPushButton(
        "Calculer la masse salariale"
    );

    periodeLayout->addWidget(new QLabel("Mois :"));
    periodeLayout->addWidget(moisMasse);

    periodeLayout->addWidget(new QLabel("Année :"));
    periodeLayout->addWidget(anneeMasse);

    periodeLayout->addStretch();
    periodeLayout->addWidget(btnCalculerMasse);

    // =================================================
    // TABLEAU DES RÉSULTATS
    // =================================================

    QGroupBox *resultats = new QGroupBox(
        "Résultats par poste"
    );

    QVBoxLayout *resultatsLayout = new QVBoxLayout(resultats);

    tableauMasse = new QTableWidget(0, 4);

    tableauMasse->setHorizontalHeaderLabels({
        "Poste",
        "Effectif",
        "Pointés",
        "Masse salariale (Ar)"
    });

    tableauMasse->horizontalHeader()
        ->setSectionResizeMode(QHeaderView::Stretch);

    tableauMasse->verticalHeader()->hide();

    tableauMasse->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );

    tableauMasse->setAlternatingRowColors(true);

    resultatsLayout->addWidget(tableauMasse);

    // =================================================
    // TOTAL GÉNÉRAL
    // =================================================

    totalMasseLabel = new QLabel(
        "TOTAL GÉNÉRAL : 0 Ar"
    );

    totalMasseLabel->setAlignment(Qt::AlignRight);

    totalMasseLabel->setStyleSheet(
        "font-size: 21px;"
        "font-weight: bold;"
        "color: #087A35;"
        "padding: 15px;"
    );

    layout->addWidget(titre);
    layout->addWidget(periode);
    layout->addWidget(resultats, 1);
    layout->addWidget(totalMasseLabel);

    connect(
        btnCalculerMasse,
        &QPushButton::clicked,
        this,
        &MainWindow::calculerMasse
    );

    return page;
}


// =====================================================
// CALCUL ET AFFICHAGE DU BULLETIN
// =====================================================

void MainWindow::afficherBulletin()
{
    // Invalider le précédent résultat avant le nouveau calcul.
    setProperty("bulletinValide", false);

    const QString matricule = matriculeEdit->text().trimmed();

    if (matricule.isEmpty()) {
        QMessageBox::warning(
            this,
            "Matricule manquant",
            "Veuillez saisir le matricule de l'employé."
        );

        matriculeEdit->setFocus();
        return;
    }

    const int mois = moisBulletin->currentIndex() + 1;

    bool anneeValide = false;

    const int annee = anneeBulletin->currentText()
                          .toInt(&anneeValide);

    if (mois < 1 || mois > 12 || !anneeValide || annee <= 0) {
        QMessageBox::warning(
            this,
            "Période invalide",
            "Veuillez sélectionner un mois et une année valides."
        );

        return;
    }

    // Appel de la fonction métier.
    Bulletin b = calculerBulletin(matricule, mois, annee);

    if (!b.ok) {

        // Effacer les anciennes informations pour éviter
        // d'afficher un ancien bulletin comme résultat actuel.
        nomEdit->clear();
        posteEdit->clear();

        salaireBaseLabel->setText("0 Ar");
        primeLabel->setText("0 Ar");
        heuresSupLabel->setText("0 Ar");
        brutLabel->setText("0 Ar");
        retenueAbsenceLabel->setText("0 Ar");
        retenueSocialeLabel->setText("0 Ar");
        netLabel->setText("0 Ar");

        for (int i = 0; i < tableauBulletin->rowCount(); ++i) {
            QTableWidgetItem *item = tableauBulletin->item(i, 1);

            if (item != nullptr) {
                item->setText("0");
            }
        }

        QMessageBox::warning(
            this,
            "Erreur de calcul",
            b.erreur.isEmpty()
                ? "Impossible de calculer le bulletin."
                : b.erreur
        );

        return;
    }

    // =================================================
    // INFORMATIONS DE L'EMPLOYÉ
    // =================================================

    nomEdit->setText(b.nom);
    posteEdit->setText(b.poste);

    // =================================================
    // RÉSULTATS FINANCIERS
    // =================================================

    salaireBaseLabel->setText(
        montant(b.salaireBase) + " Ar"
    );

    primeLabel->setText(
        montant(b.prime) + " Ar"
    );

    heuresSupLabel->setText(
        montant(b.montantHeuresSup) + " Ar"
    );

    brutLabel->setText(
        montant(b.brut) + " Ar"
    );

    retenueAbsenceLabel->setText(
        montant(b.retenueAbsence) + " Ar"
    );

    retenueSocialeLabel->setText(
        montant(b.retenueSociale) + " Ar"
    );

    netLabel->setText(
        montant(b.net) + " Ar"
    );

    // =================================================
    // REMPLISSAGE DU TABLEAU
    // =================================================

    const double valeurs[] = {
        b.salaireBase,
        b.prime,
        b.montantHeuresSup,
        b.brut,
        b.retenueAbsence,
        b.retenueSociale,
        b.net
    };

    for (int i = 0; i < 7; ++i) {

        QTableWidgetItem *item = tableauBulletin->item(i, 1);

        if (item == nullptr) {
            item = new QTableWidgetItem;
            tableauBulletin->setItem(i, 1, item);
        }

        item->setText(montant(valeurs[i]));
    }

    // Le bulletin devient imprimable uniquement après
    // un calcul réussi.
    setProperty("bulletinValide", true);

    QMessageBox::information(
        this,
        "Succès",
        "Le bulletin de paie a été calculé."
    );
}


// =====================================================
// CALCUL DE LA MASSE SALARIALE
// =====================================================

void MainWindow::calculerMasse()
{
    const int mois = moisMasse->currentIndex() + 1;

    bool anneeValide = false;

    const int annee = anneeMasse->currentText()
                          .toInt(&anneeValide);

    if (mois < 1 || mois > 12 || !anneeValide || annee <= 0) {
        QMessageBox::warning(
            this,
            "Période invalide",
            "Veuillez sélectionner un mois et une année valides."
        );

        return;
    }

    double total = 0;

    QList<LignePoste> resultat =
        masseSalarialeParPoste(mois, annee, total);

    // Supprimer les anciennes lignes.
    tableauMasse->setRowCount(0);

    for (const LignePoste &ligne : resultat) {

        const int row = tableauMasse->rowCount();

        tableauMasse->insertRow(row);

        tableauMasse->setItem(
            row,
            0,
            new QTableWidgetItem(ligne.libelle)
        );

        tableauMasse->setItem(
            row,
            1,
            new QTableWidgetItem(
                QString::number(ligne.effectif)
            )
        );

        tableauMasse->setItem(
            row,
            2,
            new QTableWidgetItem(
                QString::number(ligne.pointes)
            )
        );

        tableauMasse->setItem(
            row,
            3,
            new QTableWidgetItem(
                montant(ligne.masse)
            )
        );
    }

    totalMasseLabel->setText(
        "TOTAL GÉNÉRAL : " + montant(total) + " Ar"
    );

    if (resultat.isEmpty()) {
        QMessageBox::information(
            this,
            "Résultat",
            "Aucun résultat trouvé pour cette période."
        );
    }
}


// =====================================================
// RÉINITIALISATION
// =====================================================

void MainWindow::reinitialiserBulletin()
{
    // Le bulletin n'est plus valide.
    setProperty("bulletinValide", false);

    matriculeEdit->clear();
    nomEdit->clear();
    posteEdit->clear();

    salaireBaseLabel->setText("0 Ar");
    primeLabel->setText("0 Ar");
    heuresSupLabel->setText("0 Ar");
    brutLabel->setText("0 Ar");
    retenueAbsenceLabel->setText("0 Ar");
    retenueSocialeLabel->setText("0 Ar");
    netLabel->setText("0 Ar");

    for (int i = 0; i < tableauBulletin->rowCount(); ++i) {

        QTableWidgetItem *item = tableauBulletin->item(i, 1);

        if (item != nullptr) {
            item->setText("0");
        }
    }

    matriculeEdit->setFocus();
}


// =====================================================
// IMPRESSION DU BULLETIN
// =====================================================

void MainWindow::imprimerBulletin()
{
    // Vérifier qu'un calcul a réellement réussi.
    if (!property("bulletinValide").toBool()) {

        QMessageBox::warning(
            this,
            "Impression",
            "Veuillez d'abord calculer un bulletin valide."
        );

        return;
    }

    QString html = R"(
        <html>
        <head>
            <meta charset="UTF-8">
        </head>
        <body>
            <h2 align="center">ENI FIANARANTSOA</h2>
            <h3 align="center">BULLETIN DE PAIE</h3>
    )";

    // Échapper les valeurs pour éviter qu'un caractère
    // spécial soit interprété comme du HTML.
    html += "<p>Matricule : "
            + matriculeEdit->text().toHtmlEscaped()
            + "</p>";

    html += "<p>Nom : "
            + nomEdit->text().toHtmlEscaped()
            + "</p>";

    html += "<p>Poste : "
            + posteEdit->text().toHtmlEscaped()
            + "</p>";

    html += "<p>Période : "
            + moisBulletin->currentText().toHtmlEscaped()
            + " / "
            + anneeBulletin->currentText().toHtmlEscaped()
            + "</p>";

    html += R"(
        <table border="1"
               cellspacing="0"
               cellpadding="7"
               width="100%">
            <tr>
                <th>Rubrique</th>
                <th>Montant (Ar)</th>
            </tr>
    )";

    for (int i = 0; i < tableauBulletin->rowCount(); ++i) {

        QTableWidgetItem *rubrique = tableauBulletin->item(i, 0);
        QTableWidgetItem *valeur = tableauBulletin->item(i, 1);

        if (rubrique == nullptr || valeur == nullptr) {
            continue;
        }

        html += "<tr><td>"
                + rubrique->text().toHtmlEscaped()
                + "</td><td align='right'>"
                + valeur->text().toHtmlEscaped()
                + "</td></tr>";
    }

    html += R"(
        </table>
        </body>
        </html>
    )";

    QTextDocument document;
    document.setHtml(html);

    QPrinter printer(QPrinter::HighResolution);

    QPrintDialog dialog(&printer, this);

    dialog.setWindowTitle("Imprimer le bulletin de paie");

    if (dialog.exec() == QDialog::Accepted) {
        document.print(&printer);
    }
}
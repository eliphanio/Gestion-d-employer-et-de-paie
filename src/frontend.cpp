#include "frontend.h"
#include "database.h"
#include <QDebug>
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
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

// =====================================================
// OUTILS COMMUNS (visibles uniquement dans ce fichier)
// =====================================================

namespace {

const char *STYLE_PAGE = R"(
    #pagePaie {
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
)";

void appliquerStyle(QWidget *page)
{
    page->setObjectName("pagePaie");
    // Nécessaire pour qu'un QWidget personnalisé affiche son fond via le stylesheet.
    page->setAttribute(Qt::WA_StyledBackground, true);
    page->setStyleSheet(STYLE_PAGE);
}

QString montant(double valeur)
{
    return QLocale(QLocale::French, QLocale::Madagascar)
        .toString(valeur, 'f', 0);
}

void remplirPeriodes(QComboBox *mois, QComboBox *annee)
{
    const QStringList listeMois = {
        "Janvier", "Février", "Mars",
        "Avril", "Mai", "Juin",
        "Juillet", "Août", "Septembre",
        "Octobre", "Novembre", "Décembre"
    };

    const QDate dateActuelle = QDate::currentDate();
    const int anneeActuelle = dateActuelle.year();

    mois->clear();
    annee->clear();

    mois->addItems(listeMois);

    for (int a = anneeActuelle - 5; a <= anneeActuelle + 5; ++a) {
        annee->addItem(QString::number(a));
    }

    mois->setCurrentIndex(dateActuelle.month() - 1);
    annee->setCurrentText(QString::number(anneeActuelle));
}

QSqlDatabase connexionOuverte()
{
    QSqlDatabase connexion = Database::getConnection();

    if (!connexion.isOpen()) {
        if (!Database::connect()) {
            return QSqlDatabase();
        }
        connexion = Database::getConnection();
    }

    return connexion;
}

} // namespace


// =====================================================
// =====================================================
//                 BULLETIN DE PAIE
// =====================================================
// =====================================================

BulletinDialog::BulletinDialog(QWidget *parent)
    : QWidget(parent)
{
    appliquerStyle(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(25, 20, 25, 20);
    layout->setSpacing(15);

    QLabel *titre = new QLabel("Édition d'un bulletin de paie");
    titre->setStyleSheet(
        "font-size: 23px;"
        "font-weight: bold;"
        "color: #173B70;"
    );

    // ---------- Informations employé ----------

    QGroupBox *infos = new QGroupBox("Informations de l'employé");
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

    // ---------- Calcul du bulletin ----------

    QGroupBox *calcul = new QGroupBox("Calcul du bulletin");
    QGridLayout *grid = new QGridLayout(calcul);

    auto ajouterLigne = [&](int ligne, const QString &nom, QLabel *&valeur) {
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

    // ---------- Aperçu ----------

    QGroupBox *apercu = new QGroupBox("Aperçu du bulletin");
    QVBoxLayout *apercuLayout = new QVBoxLayout(apercu);

    tableauBulletin = new QTableWidget(7, 2);
    tableauBulletin->setHorizontalHeaderLabels({"Rubrique", "Montant (Ar)"});
    tableauBulletin->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableauBulletin->verticalHeader()->hide();
    tableauBulletin->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableauBulletin->setAlternatingRowColors(true);
    tableauBulletin->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tableauBulletin->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tableauBulletin->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableauBulletin->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    const QStringList rubriques = {
        "Salaire de base",
        "Prime d'ancienneté",
        "Heures supplémentaires",
        "BRUT",
        "Retenue absence",
        "Retenue sociale",
        "NET À PAYER"
    };

    for (int i = 0; i < rubriques.size(); ++i) {
        tableauBulletin->setItem(i, 0, new QTableWidgetItem(rubriques[i]));
        tableauBulletin->setItem(i, 1, new QTableWidgetItem("0"));
    }

    apercuLayout->addWidget(tableauBulletin);

    // ---------- Boutons ----------

    btnCalculer = new QPushButton("Calculer le bulletin");
    btnImprimer = new QPushButton("Imprimer");
    btnReinitialiser = new QPushButton("Réinitialiser");

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

    // ---------- Organisation finale ----------

    QHBoxLayout *contenu = new QHBoxLayout;
    contenu->addWidget(calcul, 1);
    contenu->addWidget(apercu, 1);

    layout->addWidget(titre);
    layout->addWidget(infos);
    layout->addLayout(contenu, 1);
    layout->addLayout(actions);

    // ---------- Périodes (les combos existent maintenant) ----------

    remplirPeriodes(moisBulletin, anneeBulletin);

    // ---------- Connexions ----------

    connect(btnRechercher, &QPushButton::clicked,
            this, &BulletinDialog::afficherBulletin);

    connect(btnCalculer, &QPushButton::clicked,
            this, &BulletinDialog::afficherBulletin);

    connect(btnReinitialiser, &QPushButton::clicked,
            this, &BulletinDialog::reinitialiserBulletin);

    connect(btnImprimer, &QPushButton::clicked,
            this, &BulletinDialog::imprimerBulletin);

    // Un changement de matricule ou de période invalide le bulletin calculé.
    connect(matriculeEdit, &QLineEdit::textChanged,
            this, [this]() { bulletinValide = false; });

    connect(moisBulletin, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) { bulletinValide = false; });

    connect(anneeBulletin, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) { bulletinValide = false; });
}

void BulletinDialog::viderResultats()
{
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
}

void BulletinDialog::afficherBulletin()
{
    bulletinValide = false;

    const QString matricule = matriculeEdit->text().trimmed();

    if (matricule.isEmpty()) {
        QMessageBox::warning(this, "Matricule manquant",
                             "Veuillez saisir le matricule de l'employé.");
        matriculeEdit->setFocus();
        return;
    }

    const int mois = moisBulletin->currentIndex() + 1;

    bool anneeValide = false;
    const int annee = anneeBulletin->currentText().toInt(&anneeValide);

    if (mois < 1 || mois > 12 || !anneeValide || annee <= 0) {
        QMessageBox::warning(this, "Période invalide",
                             "Veuillez sélectionner un mois et une année valides.");
        return;
    }

    Bulletin b = calculerBulletin(matricule, mois, annee);

    if (!b.ok) {
        viderResultats();

        QMessageBox::warning(
            this,
            "Erreur de calcul",
            b.erreur.isEmpty() ? "Impossible de calculer le bulletin." : b.erreur
        );
        return;
    }

    nomEdit->setText(b.nom);
    posteEdit->setText(b.poste);

    salaireBaseLabel->setText(montant(b.salaireBase) + " Ar");
    primeLabel->setText(montant(b.prime) + " Ar");
    heuresSupLabel->setText(montant(b.montantHeuresSup) + " Ar");
    brutLabel->setText(montant(b.brut) + " Ar");
    retenueAbsenceLabel->setText(montant(b.retenueAbsence) + " Ar");
    retenueSocialeLabel->setText(montant(b.retenueSociale) + " Ar");
    netLabel->setText(montant(b.net) + " Ar");

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

    // Imprimable uniquement après un calcul réussi.
    bulletinValide = true;

    QMessageBox::information(this, "Succès",
                             "Le bulletin de paie a été calculé.");
}

void BulletinDialog::reinitialiserBulletin()
{
    bulletinValide = false;

    matriculeEdit->clear();
    viderResultats();

    matriculeEdit->setFocus();
}

void BulletinDialog::imprimerBulletin()
{
    if (!bulletinValide) {
        QMessageBox::warning(this, "Impression",
                             "Veuillez d'abord calculer un bulletin valide.");
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

    html += "<p>Matricule : " + matriculeEdit->text().toHtmlEscaped() + "</p>";
    html += "<p>Période : "
            + moisBulletin->currentText().toHtmlEscaped()
            + " / "
            + anneeBulletin->currentText().toHtmlEscaped()
            + "</p>";
    html += "<p>Nom : " + nomEdit->text().toHtmlEscaped() + "</p>";
    html += "<p>Poste : " + posteEdit->text().toHtmlEscaped() + "</p>";

    html += R"(
        <table border="1" cellspacing="0" cellpadding="7" width="100%">
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

    html += "</table></body></html>";

    QTextDocument document;
    document.setHtml(html);

    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle("Imprimer le bulletin de paie");

    if (dialog.exec() == QDialog::Accepted) {
        document.print(&printer);
    }
}

// =====================================================
// =====================================================
//                 MASSE SALARIALE
// =====================================================
// =====================================================

MasseSalarialeDialog::MasseSalarialeDialog(QWidget *parent)
    : QWidget(parent)
{
    appliquerStyle(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(25, 20, 25, 20);
    layout->setSpacing(20);

    QLabel *titre = new QLabel("Masse salariale d'un mois par poste");
    titre->setStyleSheet(
        "font-size: 23px;"
        "font-weight: bold;"
        "color: #173B70;"
    );

    // ---------- Période ----------

    QGroupBox *periode = new QGroupBox("Période de calcul");
    QHBoxLayout *periodeLayout = new QHBoxLayout(periode);

    moisMasse = new QComboBox;
    anneeMasse = new QComboBox;

    btnCalculerMasse = new QPushButton("Calculer la masse salariale");

    periodeLayout->addWidget(new QLabel("Mois :"));
    periodeLayout->addWidget(moisMasse);
    periodeLayout->addWidget(new QLabel("Année :"));
    periodeLayout->addWidget(anneeMasse);
    periodeLayout->addStretch();
    periodeLayout->addWidget(btnCalculerMasse);

    // ---------- Résultats ----------

    QGroupBox *resultats = new QGroupBox("Résultats par poste");
    QVBoxLayout *resultatsLayout = new QVBoxLayout(resultats);

    tableauMasse = new QTableWidget(0, 4);
    tableauMasse->setHorizontalHeaderLabels({
        "Poste", "Effectif", "Pointés", "Masse salariale (Ar)"
    });
    tableauMasse->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableauMasse->verticalHeader()->hide();
    tableauMasse->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableauMasse->setAlternatingRowColors(true);

    resultatsLayout->addWidget(tableauMasse);

    // ---------- Total ----------

    totalMasseLabel = new QLabel("TOTAL GÉNÉRAL : 0 Ar");
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

    remplirPeriodes(moisMasse, anneeMasse);

    connect(btnCalculerMasse, &QPushButton::clicked,
            this, &MasseSalarialeDialog::calculerMasse);
}

void MasseSalarialeDialog::calculerMasse()
{
    const int mois = moisMasse->currentIndex() + 1;

    bool anneeValide = false;
    const int annee = anneeMasse->currentText().toInt(&anneeValide);

    if (mois < 1 || mois > 12 || !anneeValide || annee <= 0) {
        QMessageBox::warning(this, "Période invalide",
                             "Veuillez sélectionner un mois et une année valides.");
        return;
    }

    double total = 0;

    QList<LignePoste> resultat = masseSalarialeParPoste(mois, annee, total);

    tableauMasse->setRowCount(0);

    for (const LignePoste &ligne : resultat) {
        const int row = tableauMasse->rowCount();
        tableauMasse->insertRow(row);

        tableauMasse->setItem(row, 0, new QTableWidgetItem(ligne.libelle));
        tableauMasse->setItem(row, 1, new QTableWidgetItem(QString::number(ligne.effectif)));
        tableauMasse->setItem(row, 2, new QTableWidgetItem(QString::number(ligne.pointes)));
        tableauMasse->setItem(row, 3, new QTableWidgetItem(montant(ligne.masse)));
    }

    totalMasseLabel->setText("TOTAL GÉNÉRAL : " + montant(total) + " Ar");

    if (resultat.isEmpty()) {
        QMessageBox::information(this, "Résultat",
                                 "Aucun résultat trouvé pour cette période.");
    }
}


// =====================================================
// =====================================================
//                 EMPLOYÉS PAR POSTE
// =====================================================
// =====================================================

EmployesParPosteDialog::EmployesParPosteDialog(QWidget *parent)
    : QWidget(parent)
{
    appliquerStyle(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(25, 20, 25, 20);
    layout->setSpacing(20);

    QLabel *titre = new QLabel("Liste des employés par poste");
    titre->setStyleSheet(
        "font-size: 23px;"
        "font-weight: bold;"
        "color: #173B70;"
    );

    // ---------- Filtre ----------

    QGroupBox *filtre = new QGroupBox("Filtre");
    QHBoxLayout *filtreLayout = new QHBoxLayout(filtre);

    posteCombo = new QComboBox;
    effectifLabel = new QLabel("Effectif : 0");

    filtreLayout->addWidget(new QLabel("Poste :"));
    filtreLayout->addWidget(posteCombo, 1);
    filtreLayout->addStretch();
    filtreLayout->addWidget(effectifLabel);

    // ---------- Tableau ----------

    QGroupBox *resultats = new QGroupBox("Employés");
    QVBoxLayout *resultatsLayout = new QVBoxLayout(resultats);

    tableau = new QTableWidget(0, 5);
    tableau->setHorizontalHeaderLabels({
        "Matricule", "Nom", "Poste", "Salaire de base (Ar)", "Date d'embauche"
    });
    tableau->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableau->verticalHeader()->hide();
    tableau->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableau->setAlternatingRowColors(true);

    resultatsLayout->addWidget(tableau);

    layout->addWidget(titre);
    layout->addWidget(filtre);
    layout->addWidget(resultats, 1);

    chargerPostes();
    chargerEmployes();

    connect(posteCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) { chargerEmployes(); });
}

void EmployesParPosteDialog::chargerPostes()
{
    posteCombo->clear();
    posteCombo->addItem("Tous les postes", QString());

    QSqlDatabase connexion = connexionOuverte();
    if (!connexion.isOpen()) {
        return;
    }

    QSqlQuery query(connexion);

    if (!query.exec("SELECT Libelle FROM POSTE ORDER BY Libelle ASC")) {
        qDebug() << "Erreur SQL :" << query.lastError().text();
        return;
    }

    while (query.next()) {
        const QString libelle = query.value("Libelle").toString();
        posteCombo->addItem(libelle, libelle);
    }
}

void EmployesParPosteDialog::chargerEmployes()
{
    tableau->setRowCount(0);
    effectifLabel->setText("Effectif : 0");

    QSqlDatabase connexion = connexionOuverte();
    if (!connexion.isOpen()) {
        QMessageBox::warning(this, "Base de données",
                             "Impossible de se connecter à la base de données.");
        return;
    }

    const QString poste = posteCombo->currentData().toString();

    QString sql =
        "SELECT EMPLOYE.Matricule, EMPLOYE.Nom, POSTE.Libelle, "
        "POSTE.SalaireBase, EMPLOYE.dateEmbauche "
        "FROM EMPLOYE "
        "INNER JOIN POSTE ON EMPLOYE.codePoste = POSTE.codePoste ";

    if (!poste.isEmpty()) {
        sql += "WHERE POSTE.Libelle = :poste ";
    }

    sql += "ORDER BY POSTE.Libelle ASC, EMPLOYE.Nom ASC";

    QSqlQuery query(connexion);
    query.prepare(sql);

    if (!poste.isEmpty()) {
        query.bindValue(":poste", poste);
    }

    if (!query.exec()) {
        qDebug() << "Erreur SQL :" << query.lastError().text();
        return;
    }

    int ligne = 0;

    while (query.next()) {
        tableau->insertRow(ligne);

        tableau->setItem(ligne, 0, new QTableWidgetItem(query.value("Matricule").toString()));
        tableau->setItem(ligne, 1, new QTableWidgetItem(query.value("Nom").toString()));
        tableau->setItem(ligne, 2, new QTableWidgetItem(query.value("Libelle").toString()));
        tableau->setItem(ligne, 3, new QTableWidgetItem(montant(query.value("SalaireBase").toDouble())));
        tableau->setItem(ligne, 4, new QTableWidgetItem(query.value("dateEmbauche").toString()));

        ++ligne;
    }

    effectifLabel->setText(QString("Effectif : %1").arg(ligne));
}
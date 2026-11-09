#include "ecrans_paie.h"

#include <QComboBox>
#include <QDate>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLocale>
#include <QMessageBox>
#include <QPageSize>
#include <QPainter>
#include <QPdfWriter>
#include <QPushButton>
#include <QSqlQuery>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTextDocument>
#include <QVBoxLayout>

// Qt Charts (module "charts" à activer dans le .pro / CMakeLists)
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
// Si vous utilisez Qt 5, ajouter aussi : QT_CHARTS_USE_NAMESPACE

// ============================================================
//  Outils communs
// ============================================================
static QString fmt(double v)
{
    return QLocale(QLocale::French).toString(v, 'f', 0);
}

static void remplirMois(QComboBox *cb)
{
    for (int m = 1; m <= 12; ++m)
        cb->addItem(QString::number(m), m);
    cb->setCurrentIndex(QDate::currentDate().month() - 1);
}

static void remplirAnnees(QComboBox *cb)
{
    QSqlQuery q("SELECT DISTINCT Annee FROM POINTAGE ORDER BY Annee DESC");
    while (q.next())
        cb->addItem(q.value(0).toString(), q.value(0).toInt());
    if (cb->count() == 0) {
        const int a = QDate::currentDate().year();
        cb->addItem(QString::number(a), a);
    }
}

static QTableWidgetItem *cellule(const QString &texte,
                                 Qt::Alignment align = Qt::AlignLeft | Qt::AlignVCenter,
                                 bool gras = false)
{
    auto *item = new QTableWidgetItem(texte);
    item->setTextAlignment(align);
    if (gras) {
        QFont f = item->font();
        f.setBold(true);
        item->setFont(f);
    }
    return item;
}

// Rubriques du bulletin, dans l'ordre du format de l'énoncé
static QList<QPair<QString, double>> rubriques(const Bulletin &b)
{
    return {
        {"Salaire de base",         b.salaireBase},
        {"Prime d'ancienneté",      b.prime},
        {"Heures supplémentaires",  b.montantHeuresSup},
        {"BRUT",                    b.brut},
        {"Retenue absence",         b.retenueAbsence},
        {"Retenue sociale",         b.retenueSociale},
        {"NET A PAYER",             b.net},
    };
}
static bool estGras(int i) { return i == 3 || i == 6; }   // BRUT et NET

// ============================================================
//  Menu 4 : bulletin de paie
// ============================================================
BulletinDialog::BulletinDialog(QWidget *parent) : QWidget(parent)
{
    cbMatricule = new QComboBox;
    QSqlQuery q("SELECT Matricule, Nom FROM EMPLOYE ORDER BY Nom");
    while (q.next())
        cbMatricule->addItem(q.value(0).toString() + " - " + q.value(1).toString(),
                             q.value(0));

    cbMois = new QComboBox;
    remplirMois(cbMois);
    cbAnnee = new QComboBox;
    remplirAnnees(cbAnnee);

    auto *btnAfficher = new QPushButton("Afficher le bulletin");
    auto *btnPdf = new QPushButton("Exporter en PDF");

    lblInfo = new QLabel("Choisissez un employé et une période.");
    lblInfo->setWordWrap(true);

    table = new QTableWidget(0, 2);
    table->setHorizontalHeaderLabels({"RUBRIQUE", "MONTANT"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    lblNet = new QLabel;
    QFont f = lblNet->font();
    f.setBold(true);
    f.setPointSize(f.pointSize() + 2);
    lblNet->setFont(f);

    auto *form = new QFormLayout;
    form->addRow("Matricule :", cbMatricule);
    form->addRow("Mois :", cbMois);
    form->addRow("Année :", cbAnnee);

    auto *boutons = new QHBoxLayout;
    boutons->addWidget(btnAfficher);
    boutons->addWidget(btnPdf);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addLayout(boutons);
    layout->addWidget(lblInfo);
    layout->addWidget(table);
    layout->addWidget(lblNet);

    connect(btnAfficher, &QPushButton::clicked, this, &BulletinDialog::afficher);
    connect(btnPdf, &QPushButton::clicked, this, &BulletinDialog::exporterPdf);
}

void BulletinDialog::afficher()
{
    if (cbMatricule->count() == 0) {
        QMessageBox::warning(this, "Bulletin", "Aucun employé dans la base.");
        return;
    }

    const QString matricule = cbMatricule->currentData().toString();
    const int mois  = cbMois->currentData().toInt();
    const int annee = cbAnnee->currentData().toInt();

    Bulletin b = calculerBulletin(matricule, mois, annee);
    if (!b.ok) {
        QMessageBox::warning(this, "Bulletin", b.erreur);
        return;
    }
    dernier = b;
    dernierMois = mois;
    dernierAnnee = annee;

    lblInfo->setText(QString("MATRICULE : %1     NOM : %2\nPOSTE : %3     PERIODE : %4 / %5")
                         .arg(b.matricule, b.nom, b.poste)
                         .arg(mois).arg(annee));

    const auto lignes = rubriques(b);
    table->setRowCount(lignes.size());
    for (int i = 0; i < lignes.size(); ++i) {
        table->setItem(i, 0, cellule(lignes[i].first, Qt::AlignLeft | Qt::AlignVCenter, estGras(i)));
        table->setItem(i, 1, cellule(fmt(lignes[i].second), Qt::AlignRight | Qt::AlignVCenter, estGras(i)));
    }
    lblNet->setText("NET A PAYER : " + fmt(b.net) + " Ar");
}

QString BulletinDialog::htmlBulletin() const
{
    const Bulletin &b = dernier;
    QString h = QString("<h2 align='center'>BULLETIN DE PAIE</h2>"
                        "<p>MATRICULE : %1 &nbsp;&nbsp;&nbsp; NOM : %2<br>"
                        "POSTE : %3 &nbsp;&nbsp;&nbsp; PERIODE : %4 / %5</p>"
                        "<table border='1' cellspacing='0' cellpadding='6' width='100%'>"
                        "<tr><th align='left'>RUBRIQUE</th><th align='right'>MONTANT</th></tr>")
                    .arg(b.matricule.toHtmlEscaped(), b.nom.toHtmlEscaped(), b.poste.toHtmlEscaped())
                    .arg(dernierMois).arg(dernierAnnee);

    const auto lignes = rubriques(b);
    for (int i = 0; i < lignes.size(); ++i) {
        const QString nom = lignes[i].first.toHtmlEscaped();
        const QString val = fmt(lignes[i].second);
        h += estGras(i)
                 ? QString("<tr><td><b>%1</b></td><td align='right'><b>%2</b></td></tr>").arg(nom, val)
                 : QString("<tr><td>%1</td><td align='right'>%2</td></tr>").arg(nom, val);
    }
    h += "</table>";
    h += QString("<h3>NET A PAYER : %1 Ar</h3>").arg(fmt(b.net));
    return h;
}

void BulletinDialog::exporterPdf()
{
    if (!dernier.ok) {
        QMessageBox::information(this, "Export PDF", "Affichez d'abord un bulletin.");
        return;
    }
    const QString chemin = QFileDialog::getSaveFileName(
        this, "Exporter en PDF",
        QString("bulletin_%1_%2_%3.pdf").arg(dernier.matricule).arg(dernierMois).arg(dernierAnnee),
        "PDF (*.pdf)");
    if (chemin.isEmpty())
        return;

    QPdfWriter pdf(chemin);
    pdf.setPageSize(QPageSize(QPageSize::A4));
    QTextDocument doc;
    doc.setHtml(htmlBulletin());
    doc.print(&pdf);

    QMessageBox::information(this, "Export PDF", "Bulletin exporté :\n" + chemin);
}

// ============================================================
//  Menu 5 : masse salariale par poste
// ============================================================
MasseSalarialeDialog::MasseSalarialeDialog(QWidget *parent) : QWidget(parent)
{
    cbMois = new QComboBox;
    remplirMois(cbMois);
    cbAnnee = new QComboBox;
    remplirAnnees(cbAnnee);

    auto *btnCalculer = new QPushButton("Calculer");
    btnGraphique = new QPushButton("Histogramme");
    btnGraphique->setEnabled(false);

    table = new QTableWidget(0, 4);
    table->setHorizontalHeaderLabels({"POSTE", "EFFECTIF", "POINTES", "MASSE NETTE"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    lblTotal = new QLabel;
    QFont f = lblTotal->font();
    f.setBold(true);
    lblTotal->setFont(f);

    auto *haut = new QHBoxLayout;
    haut->addWidget(new QLabel("Mois :"));
    haut->addWidget(cbMois);
    haut->addWidget(new QLabel("Année :"));
    haut->addWidget(cbAnnee);
    haut->addWidget(btnCalculer);
    haut->addWidget(btnGraphique);

    // Page du tableau, placée dans une pile avec la page du graphique
    pageTableau = new QWidget;
    auto *layout = new QVBoxLayout(pageTableau);
    layout->addLayout(haut);
    layout->addWidget(table);
    layout->addWidget(lblTotal);

    pile = new QStackedWidget;
    pile->addWidget(pageTableau);

    auto *principal = new QVBoxLayout(this);
    principal->setContentsMargins(0, 0, 0, 0);
    principal->addWidget(pile);

    connect(btnCalculer, &QPushButton::clicked, this, &MasseSalarialeDialog::calculer);
    connect(btnGraphique, &QPushButton::clicked, this, &MasseSalarialeDialog::ouvrirGraphique);
}

void MasseSalarialeDialog::calculer()
{
    moisCalcule   = cbMois->currentData().toInt();
    anneeCalculee = cbAnnee->currentData().toInt();

    double total = 0;
    lignes = masseSalarialeParPoste(moisCalcule, anneeCalculee, total);

    table->setRowCount(lignes.size());
    for (int i = 0; i < lignes.size(); ++i) {
        const LignePoste &l = lignes[i];
        table->setItem(i, 0, cellule(l.libelle));
        table->setItem(i, 1, cellule(QString::number(l.effectif), Qt::AlignCenter));
        table->setItem(i, 2, cellule(QString::number(l.pointes), Qt::AlignCenter));
        table->setItem(i, 3, cellule(fmt(l.masse), Qt::AlignRight | Qt::AlignVCenter));
    }
    lblTotal->setText("TOTAL GENERAL DE LA MASSE SALARIALE : " + fmt(total) + " Ar");
    btnGraphique->setEnabled(!lignes.isEmpty());
}

void MasseSalarialeDialog::ouvrirGraphique()
{
    if (pageGraphique) {
        pile->removeWidget(pageGraphique);
        pageGraphique->deleteLater();
    }

    auto *g = new GraphiqueMasseDialog(lignes, moisCalcule, anneeCalculee);
    pageGraphique = g;
    connect(g, &GraphiqueMasseDialog::retour, this, [this]() {
        pile->setCurrentWidget(pageTableau);
    });

    pile->addWidget(g);
    pile->setCurrentWidget(g);
}

// ============================================================
//  Menu 6 : employés par poste
// ============================================================
EmployesParPosteDialog::EmployesParPosteDialog(QWidget *parent) : QWidget(parent)
{
    cbPoste = new QComboBox;
    int totalEmployes = 0;
    QSqlQuery q("SELECT p.codePoste, p.Libelle, COUNT(e.Matricule) "
                "FROM POSTE p LEFT JOIN EMPLOYE e ON e.codePoste = p.codePoste "
                "GROUP BY p.codePoste, p.Libelle ORDER BY p.Libelle");
    while (q.next()) {
        cbPoste->addItem(QString("%1 (%2)").arg(q.value(1).toString()).arg(q.value(2).toInt()),
                         q.value(0));
        totalEmployes += q.value(2).toInt();
    }
    cbPoste->insertItem(0, QString("Tous les postes (%1)").arg(totalEmployes));
    cbPoste->setCurrentIndex(0);

    table = new QTableWidget(0, 5);
    table->setHorizontalHeaderLabels({"MATRICULE", "NOM", "ADRESSE", "DATE EMBAUCHE", "POSTE"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    lblEffectif = new QLabel;
    QFont f = lblEffectif->font();
    f.setBold(true);
    lblEffectif->setFont(f);

    auto *haut = new QHBoxLayout;
    haut->addWidget(new QLabel("Poste :"));
    haut->addWidget(cbPoste, 1);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(haut);
    layout->addWidget(table);
    layout->addWidget(lblEffectif);

    connect(cbPoste, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) { charger(); });
    charger();
}

void EmployesParPosteDialog::charger()
{
    const bool tous = cbPoste->currentIndex() <= 0;

    QString sql = "SELECT e.Matricule, e.Nom, e.Adresse, e.dateEmbauche, p.Libelle "
                  "FROM EMPLOYE e JOIN POSTE p ON e.codePoste = p.codePoste ";
    if (!tous)
        sql += "WHERE p.codePoste = ? ";
    sql += "ORDER BY p.Libelle, e.Nom";

    QSqlQuery q;
    q.prepare(sql);
    if (!tous)
        q.addBindValue(cbPoste->currentData());
    q.exec();

    table->setRowCount(0);
    int r = 0;
    while (q.next()) {
        table->insertRow(r);
        for (int c = 0; c < 5; ++c) {
            const QString v = (c == 3) ? q.value(c).toDate().toString("dd/MM/yyyy")
                                       : q.value(c).toString();
            table->setItem(r, c, cellule(v));
        }
        ++r;
    }
    lblEffectif->setText(QString("EFFECTIF : %1").arg(r));
}

// ============================================================
//  Bonus : histogramme
// ============================================================
GraphiqueMasseDialog::GraphiqueMasseDialog(const QList<LignePoste> &lignes, int mois, int annee,
                                           QWidget *parent) : QWidget(parent)
{
    auto *set = new QBarSet("Masse salariale nette");
    QStringList categories;
    double max = 0;
    for (const LignePoste &l : lignes) {
        *set << l.masse;
        categories << l.libelle;
        max = qMax(max, l.masse);
    }

    auto *series = new QBarSeries;
    series->append(set);
    series->setLabelsVisible(true);

    auto *chart = new QChart;
    chart->addSeries(series);
    chart->setTitle(QString("Masse salariale par poste - %1/%2").arg(mois).arg(annee));

    auto *axeX = new QBarCategoryAxis;
    axeX->append(categories);
    chart->addAxis(axeX, Qt::AlignBottom);
    series->attachAxis(axeX);

    auto *axeY = new QValueAxis;
    axeY->setRange(0, max > 0 ? max * 1.15 : 1);
    axeY->setLabelFormat("%.0f");
    chart->addAxis(axeY, Qt::AlignLeft);
    series->attachAxis(axeY);

    auto *vue = new QChartView(chart);
    vue->setRenderHint(QPainter::Antialiasing);

    auto *btnRetour = new QPushButton("← Retour");
    connect(btnRetour, &QPushButton::clicked, this, &GraphiqueMasseDialog::retour);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(btnRetour, 0, Qt::AlignLeft);
    layout->addWidget(vue);
}
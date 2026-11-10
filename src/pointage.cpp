/*
 * ======================================================================
 *  pointage.cpp : implementation du module POINTAGE
 *  Projet 4 - Gestion des employes et de la paie (Qt / C++ / MySQL)
 * ======================================================================
 */

#include "pointage.h"

#include <QComboBox>
#include <QDate>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTableView>
#include <QVBoxLayout>

// ----------------------------------------------------------------------
//  Noms des tables : modifiables a UN SEUL endroit.
//  (sous Linux la casse compte : adapte si tes tables sont en MAJUSCULES)
// ----------------------------------------------------------------------
static const QString T_EMPLOYE  = "EMPLOYE";
static const QString T_POINTAGE = "POINTAGE";

// ======================================================================
//  CONSTRUCTEUR
// ======================================================================
PointageWidget::PointageWidget(QWidget *parent)
    : QWidget(parent)
{
    construireInterface();   // cree les widgets et les layouts
    remplirMoisAnnee();      // listes deroulantes fixes (mois, annee)
    chargerEmployes();       // liste deroulante dependant de la base
    actualiserTable();       // premier affichage du tableau
    majBoutons();            // Modifier/Supprimer inactifs au depart
}

// ======================================================================
//  CONSTRUCTION DE L'INTERFACE
// ======================================================================
void PointageWidget::construireInterface()
{
    // ---- Titre de la page ----
    QLabel *titre = new QLabel("Gestion du pointage mensuel");
    titre->setStyleSheet("font-size: 18px; font-weight: bold;");

    // ---- Creation des champs du formulaire ----
    cbMatricule    = new QComboBox;
    cbMois         = new QComboBox;
    cbAnnee        = new QComboBox;

    spHeuresSup    = new QDoubleSpinBox;
    spHeuresSup->setRange(0, 200);       // 0 a 200 heures sup par mois
    spHeuresSup->setDecimals(2);
    spHeuresSup->setSuffix(" h");

    // La colonne JoursAbsence est DECIMAL(5,2) : on accepte les demi-journees
    spJoursAbsence = new QDoubleSpinBox;
    spJoursAbsence->setRange(0, 31);     // au plus 31 jours
    spJoursAbsence->setDecimals(1);
    spJoursAbsence->setSingleStep(0.5);  // pas d'une demi-journee
    spJoursAbsence->setSuffix(" j");

    // ---- Disposition du formulaire (libelle + champ) ----
    QFormLayout *form = new QFormLayout;
    form->addRow("Employé :",            cbMatricule);
    form->addRow("Mois :",               cbMois);
    form->addRow("Année :",              cbAnnee);
    form->addRow("Heures sup. :",        spHeuresSup);
    form->addRow("Jours d'absence :",    spJoursAbsence);

    // ---- Boutons (memes couleurs que le reste de l'application) ----
    btnAjouter   = new QPushButton("Ajouter");
    btnModifier  = new QPushButton("Modifier");
    btnSupprimer = new QPushButton("Supprimer");

    // Style commun ; l'etat :disabled donne un bouton grise
    auto styliser = [](QPushButton *b, const QString &couleur) {
        b->setStyleSheet(
            "QPushButton { background-color: " + couleur + "; color: white; "
                                                           "font-weight: bold; padding: 8px 16px; border-radius: 6px; }"
                                                           "QPushButton:disabled { background-color: #b0b0b0; color: #eeeeee; }");
    };
    styliser(btnAjouter,   "blue");
    styliser(btnModifier,  "green");
    styliser(btnSupprimer, "red");

    QHBoxLayout *boutons = new QHBoxLayout;
    boutons->addWidget(btnAjouter);
    boutons->addWidget(btnModifier);
    boutons->addWidget(btnSupprimer);
    boutons->addStretch();

    // ---- Zone de recherche ----
    edRecherche = new QLineEdit;
    edRecherche->setPlaceholderText("Rechercher par matricule ou par nom...");

    // ---- Tableau : modele en lecture seule + vue ----
    modele = new QSqlQueryModel(this);    // 'this' : detruit avec le widget
    table  = new QTableView;
    table->setModel(modele);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);  // ligne entiere
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);   // pas d'edition directe
    table->setAlternatingRowColors(false);       // pas de lignes noires/alternees
    table->verticalHeader()->setVisible(false);  // cache la colonne des numeros de ligne
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Style du tableau : fond blanc partout, texte noir, en-tete bleu fonce.
    // On impose les couleurs pour ne pas dependre du theme (clair/sombre) du systeme.
    table->setStyleSheet(
        "QTableView {"
        "  background-color: white;"
        "  color: black;"
        "  gridline-color: #d0d0d0;"
        "  selection-background-color: #cfe2ff;"   // ligne selectionnee : bleu clair
        "  selection-color: black;"
        "}"
        "QHeaderView::section {"
        "  background-color: #1b2a41;"
        "  color: white;"
        "  font-weight: bold;"
        "  padding: 6px;"
        "  border: none;"
        "}"
        "QTableCornerButton::section { background-color: #1b2a41; }");

    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // ---- Assemblage final de la page ----
    QVBoxLayout *principal = new QVBoxLayout(this);
    principal->addWidget(titre);
    principal->addLayout(form);
    principal->addLayout(boutons);
    principal->addWidget(edRecherche);
    principal->addWidget(table);

    // ---- Connexion des signaux aux slots ----
    connect(btnAjouter,   &QPushButton::clicked, this, &PointageWidget::onAjouter);
    connect(btnModifier,  &QPushButton::clicked, this, &PointageWidget::onModifier);
    connect(btnSupprimer, &QPushButton::clicked, this, &PointageWidget::onSupprimer);
    connect(edRecherche,  &QLineEdit::textChanged, this, &PointageWidget::onRecherche);
    connect(table,        &QTableView::clicked,    this, &PointageWidget::onLigneChoisie);
}

// ======================================================================
//  LISTES DEROULANTES
// ======================================================================

// Mois (1 a 12) et annee (de l'annee courante -5 a +1), avec valeurs par defaut
void PointageWidget::remplirMoisAnnee()
{
    cbMois->clear();
    for (int m = 1; m <= 12; ++m)
        cbMois->addItem(QString::number(m), m);   // texte affiche, valeur associee

    const int anneeCourante = QDate::currentDate().year();
    cbAnnee->clear();
    for (int a = anneeCourante - 5; a <= anneeCourante + 1; ++a)
        cbAnnee->addItem(QString::number(a), a);

    // Positionne sur le mois et l'annee d'aujourd'hui
    cbMois->setCurrentIndex(cbMois->findData(QDate::currentDate().month()));
    cbAnnee->setCurrentIndex(cbAnnee->findData(anneeCourante));
}

// Liste des employes lue dans la base : on affiche "Matricule - Nom"
// mais on garde le matricule seul comme valeur associee (userData)
void PointageWidget::chargerEmployes()
{
    const QString ancien = matriculeCourant();   // pour retrouver la selection apres rechargement

    cbMatricule->clear();

    QSqlQuery q;
    if (!q.exec("SELECT Matricule, Nom FROM " + T_EMPLOYE + " ORDER BY Nom")) {
        afficherErreurSql(q);
        return;
    }
    while (q.next()) {
        const QString mat = q.value(0).toString();
        const QString nom = q.value(1).toString();
        cbMatricule->addItem(mat + " - " + nom, mat);
    }

    // Restaure la selection precedente si elle existe encore
    const int idx = cbMatricule->findData(ancien);
    if (idx >= 0)
        cbMatricule->setCurrentIndex(idx);
}

// Retourne le matricule selectionne (chaine vide si la liste est vide)
QString PointageWidget::matriculeCourant() const
{
    return cbMatricule->currentData().toString();
}

// ======================================================================
//  LISTAGE : remplit le tableau (jointure POINTAGE - EMPLOYE)
// ======================================================================
void PointageWidget::actualiserTable()
{
    QSqlQuery q;
    q.prepare(
        "SELECT p.Matricule, e.Nom, p.Mois, p.Annee, p.HeuresSup, p.JoursAbsence "
        "FROM " + T_POINTAGE + " p "
                       "JOIN " + T_EMPLOYE + " e ON e.Matricule = p.Matricule "
                      "WHERE p.Matricule LIKE :filtre OR e.Nom LIKE :filtre "
                      "ORDER BY p.Annee DESC, p.Mois DESC, e.Nom");

    // Le filtre contient le texte de recherche entoure de % (recherche partielle)
    q.bindValue(":filtre", "%" + edRecherche->text().trimmed() + "%");

    if (!q.exec()) {
        afficherErreurSql(q);
        return;
    }
    modele->setQuery(std::move(q));   // le modele prend la requete executee

    // Titres des colonnes
    modele->setHeaderData(0, Qt::Horizontal, "Matricule");
    modele->setHeaderData(1, Qt::Horizontal, "Nom");
    modele->setHeaderData(2, Qt::Horizontal, "Mois");
    modele->setHeaderData(3, Qt::Horizontal, "Année");
    modele->setHeaderData(4, Qt::Horizontal, "Heures sup.");
    modele->setHeaderData(5, Qt::Horizontal, "Jours d'absence");
}

void PointageWidget::actualiser()
{
    chargerEmployes();
    actualiserTable();
}

void PointageWidget::onRecherche(const QString &)
{
    actualiserTable();   // le texte est relu dans edRecherche
}

// ======================================================================
//  CONTROLES DE SAISIE
//  Retourne true si les donnees du formulaire sont coherentes
// ======================================================================
bool PointageWidget::validerSaisie()
{
    // 1) Un employe doit etre choisi (liste vide si aucun employe en base)
    if (matriculeCourant().isEmpty()) {
        QMessageBox::warning(this, "Saisie incomplète",
                             "Aucun employé sélectionné.\n"
                             "Ajoutez d'abord un employé dans le module Employé.");
        return false;
    }

    const int mois  = cbMois->currentData().toInt();
    const int annee = cbAnnee->currentData().toInt();

    // 2) Les jours d'absence ne peuvent pas depasser le nombre de jours du mois
    const QDate premierDuMois(annee, mois, 1);
    if (spJoursAbsence->value() > premierDuMois.daysInMonth()) {
        QMessageBox::warning(this, "Saisie invalide",
                             QString("Le mois %1/%2 ne compte que %3 jours : "
                                     "les jours d'absence sont trop élevés.")
                                 .arg(mois).arg(annee).arg(premierDuMois.daysInMonth()));
        return false;
    }

    // 3) La periode ne doit pas preceder la date d'embauche de l'employe
    QSqlQuery q;
    q.prepare("SELECT dateEmbauche FROM " + T_EMPLOYE + " WHERE Matricule = :mat");
    q.bindValue(":mat", matriculeCourant());
    if (!q.exec()) {
        afficherErreurSql(q);
        return false;
    }
    if (q.next()) {
        const QDate embauche = q.value(0).toDate();
        const QDate finDuMois(annee, mois, premierDuMois.daysInMonth());
        if (embauche.isValid() && finDuMois < embauche) {
            QMessageBox::warning(this, "Date incohérente",
                                 "Cette période est antérieure à la date d'embauche "
                                 "de l'employé (" + embauche.toString("dd/MM/yyyy") + ").");
            return false;
        }
    }
    return true;   // tout est correct
}

// ======================================================================
//  RECHERCHE D'UN POINTAGE (SELECT) : existe-t-il deja ?
// ======================================================================
bool PointageWidget::pointageExiste(const QString &matricule, int mois, int annee)
{
    QSqlQuery q;
    q.prepare("SELECT COUNT(*) FROM " + T_POINTAGE +
              " WHERE Matricule = :mat AND Mois = :mois AND Annee = :annee");
    q.bindValue(":mat",   matricule);
    q.bindValue(":mois",  mois);
    q.bindValue(":annee", annee);

    if (!q.exec() || !q.next()) {
        afficherErreurSql(q);
        return false;
    }
    return q.value(0).toInt() > 0;
}

// ======================================================================
//  AJOUT (SELECT puis INSERT)
// ======================================================================
void PointageWidget::onAjouter()
{
    if (!validerSaisie())
        return;                                   // on s'arrete si la saisie est invalide

    const QString mat = matriculeCourant();
    const int mois    = cbMois->currentData().toInt();
    const int annee   = cbAnnee->currentData().toInt();

    // Etape 1 : recherche dans la table POINTAGE
    if (pointageExiste(mat, mois, annee)) {
        QMessageBox::information(this, "Pointage",
                                 "Pointage déjà saisi pour cet employé et cette période.");
        return;
    }

    // Etape 2 : il n'existe pas encore, on l'insere
    QSqlQuery q;
    q.prepare("INSERT INTO " + T_POINTAGE +
              " (Matricule, Mois, Annee, HeuresSup, JoursAbsence) "
              "VALUES (:mat, :mois, :annee, :hsup, :jabs)");
    q.bindValue(":mat",   mat);
    q.bindValue(":mois",  mois);
    q.bindValue(":annee", annee);
    q.bindValue(":hsup",  spHeuresSup->value());
    q.bindValue(":jabs",  spJoursAbsence->value());

    if (!q.exec()) {
        afficherErreurSql(q);
        return;
    }

    QMessageBox::information(this, "Pointage", "Pointage ajouté avec succès.");
    viderFormulaire();
    actualiserTable();
    emit pointageModifie();    // previent les autres modules
}

// ======================================================================
//  MODIFICATION (SELECT puis UPDATE)
//  On modifie la ligne CHOISIE dans le tableau. Son ancienne cle
//  (cleMatricule, cleMois, cleAnnee) sert a la retrouver : on peut donc
//  aussi changer l'employe, le mois ou l'annee de ce pointage.
// ======================================================================
void PointageWidget::onModifier()
{
    // 1) Une ligne doit avoir ete choisie dans le tableau
    if (!ligneSelectionnee()) {
        QMessageBox::warning(this, "Pointage",
                             "Cliquez d'abord sur la ligne à modifier dans le tableau.");
        return;
    }

    // 2) Controles de saisie
    if (!validerSaisie())
        return;

    const QString mat = matriculeCourant();
    const int mois    = cbMois->currentData().toInt();
    const int annee   = cbAnnee->currentData().toInt();

    // 3) L'ancienne ligne doit toujours exister
    if (!pointageExiste(cleMatricule, cleMois, cleAnnee)) {
        QMessageBox::warning(this, "Pointage", "Pointage non trouvé.");
        viderFormulaire();
        actualiserTable();
        return;
    }

    // 4) Si la cle change, la nouvelle cle ne doit pas deja exister
    const bool cleChangee = (mat != cleMatricule || mois != cleMois || annee != cleAnnee);
    if (cleChangee && pointageExiste(mat, mois, annee)) {
        QMessageBox::information(this, "Pointage",
                                 "Pointage déjà saisi pour cet employé et cette période.");
        return;
    }

    // 5) Mise a jour de la ligne (anciennes valeurs de cle dans le WHERE)
    QSqlQuery q;
    q.prepare("UPDATE " + T_POINTAGE +
              " SET Matricule = :nmat, Mois = :nmois, Annee = :nannee, "
              "HeuresSup = :hsup, JoursAbsence = :jabs "
              "WHERE Matricule = :omat AND Mois = :omois AND Annee = :oannee");
    q.bindValue(":nmat",   mat);
    q.bindValue(":nmois",  mois);
    q.bindValue(":nannee", annee);
    q.bindValue(":hsup",   spHeuresSup->value());
    q.bindValue(":jabs",   spJoursAbsence->value());
    q.bindValue(":omat",   cleMatricule);
    q.bindValue(":omois",  cleMois);
    q.bindValue(":oannee", cleAnnee);

    if (!q.exec()) {
        afficherErreurSql(q);
        return;
    }

    QMessageBox::information(this, "Pointage", "Pointage modifié avec succès.");
    viderFormulaire();        // formulaire propre, plus de ligne choisie
    actualiserTable();
    emit pointageModifie();
}

// ======================================================================
//  SUPPRESSION (SELECT puis DELETE, avec confirmation)
//  On supprime la ligne CHOISIE dans le tableau.
// ======================================================================
void PointageWidget::onSupprimer()
{
    // 1) Une ligne doit avoir ete choisie dans le tableau
    if (!ligneSelectionnee()) {
        QMessageBox::warning(this, "Pointage",
                             "Cliquez d'abord sur la ligne à supprimer dans le tableau.");
        return;
    }

    // 2) La ligne doit toujours exister
    if (!pointageExiste(cleMatricule, cleMois, cleAnnee)) {
        QMessageBox::warning(this, "Pointage", "Pointage non trouvé.");
        viderFormulaire();
        actualiserTable();
        return;
    }

    // 3) Confirmation avant suppression definitive
    const auto reponse = QMessageBox::question(
        this, "Confirmation",
        QString("Supprimer le pointage de %1 pour %2/%3 ?")
            .arg(cleMatricule).arg(cleMois).arg(cleAnnee));
    if (reponse != QMessageBox::Yes)
        return;

    // 4) Suppression
    QSqlQuery q;
    q.prepare("DELETE FROM " + T_POINTAGE +
              " WHERE Matricule = :mat AND Mois = :mois AND Annee = :annee");
    q.bindValue(":mat",   cleMatricule);
    q.bindValue(":mois",  cleMois);
    q.bindValue(":annee", cleAnnee);

    if (!q.exec()) {
        afficherErreurSql(q);
        return;
    }

    QMessageBox::information(this, "Pointage", "Pointage supprimé.");
    viderFormulaire();
    actualiserTable();
    emit pointageModifie();
}

// ======================================================================
//  INTERACTION AVEC LE TABLEAU ET LE FORMULAIRE
// ======================================================================

// Clic sur une ligne : recopie ses valeurs dans le formulaire
void PointageWidget::onLigneChoisie(const QModelIndex &index)
{
    const int ligne = index.row();

    // Colonnes du SELECT : 0 Matricule, 1 Nom, 2 Mois, 3 Annee, 4 HeuresSup, 5 JoursAbsence
    const QString mat   = modele->data(modele->index(ligne, 0)).toString();
    const int mois      = modele->data(modele->index(ligne, 2)).toInt();
    const int annee     = modele->data(modele->index(ligne, 3)).toInt();
    const double hsup   = modele->data(modele->index(ligne, 4)).toDouble();
    const double jabs   = modele->data(modele->index(ligne, 5)).toDouble();

    cbMatricule->setCurrentIndex(cbMatricule->findData(mat));
    cbMois->setCurrentIndex(cbMois->findData(mois));

    // Si l'annee n'est pas dans la liste (vieilles donnees), on l'ajoute
    int idxAnnee = cbAnnee->findData(annee);
    if (idxAnnee < 0) {
        cbAnnee->addItem(QString::number(annee), annee);
        idxAnnee = cbAnnee->findData(annee);
    }
    cbAnnee->setCurrentIndex(idxAnnee);

    spHeuresSup->setValue(hsup);
    spJoursAbsence->setValue(jabs);

    // On memorise la cle de la ligne choisie (utile pour Modifier / Supprimer)
    cleMatricule = mat;
    cleMois      = mois;
    cleAnnee     = annee;
    majBoutons();
}

// Remet TOUT le formulaire a zero : employe, mois/annee du jour, valeurs a 0,
// recherche videe, aucune ligne choisie.
void PointageWidget::viderFormulaire()
{
    cbMatricule->setCurrentIndex(0);                       // premier employe de la liste
    cbMois->setCurrentIndex(cbMois->findData(QDate::currentDate().month()));
    cbAnnee->setCurrentIndex(cbAnnee->findData(QDate::currentDate().year()));
    spHeuresSup->setValue(0);
    spJoursAbsence->setValue(0);

    cleMatricule.clear();                                  // plus de ligne choisie
    cleMois  = 0;
    cleAnnee = 0;
    table->clearSelection();
    edRecherche->clear();                                  // reaffiche toutes les lignes
    majBoutons();
}

// Vrai si l'utilisateur a clique sur une ligne du tableau
bool PointageWidget::ligneSelectionnee() const
{
    return !cleMatricule.isEmpty();
}

// Modifier et Supprimer ne servent que si une ligne est choisie ;
// Ajouter reste toujours actif
void PointageWidget::majBoutons()
{
    const bool choisie = ligneSelectionnee();
    btnModifier->setEnabled(choisie);
    btnSupprimer->setEnabled(choisie);
}

// ======================================================================
//  AFFICHAGE D'UNE ERREUR SQL (requete ratee, connexion perdue, etc.)
// ======================================================================
void PointageWidget::afficherErreurSql(const QSqlQuery &q)
{
    QMessageBox::critical(this, "Erreur base de données",
                          "La requête a échoué :\n" + q.lastError().text());
}

#ifndef POINTAGE_H
#define POINTAGE_H



#include <QWidget>

// Declarations anticipees : evitent d'inclure tous les en-tetes Qt ici
class QComboBox;
class QDoubleSpinBox;
class QSpinBox;
class QLineEdit;
class QPushButton;
class QTableView;
class QSqlQueryModel;
class QSqlQuery;
class QModelIndex;

class PointageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PointageWidget(QWidget *parent = nullptr);

signals:
    // Emis apres un ajout / modification / suppression.
    // Utile pour que les modules Paie et Masse salariale se rafraichissent.
    void pointageModifie();

public slots:
    // Recharge la liste des employes et le tableau
    // (a appeler quand on affiche la page, car des employes ont pu changer)
    void actualiser();

private slots:
    void onAjouter();                          // bouton "Ajouter"
    void onModifier();                         // bouton "Modifier"
    void onSupprimer();                        // bouton "Supprimer"
    void onRecherche(const QString &texte);    // champ de recherche
    void onLigneChoisie(const QModelIndex &i); // clic sur une ligne du tableau

private:
    // ---- Construction de l'interface ----
    void construireInterface();
    void chargerEmployes();     // alimente la liste des matricules depuis la base
    void remplirMoisAnnee();    // alimente les listes mois et annee
    void actualiserTable();     // execute le SELECT et remplit le tableau
    void viderFormulaire();           // remet TOUT le formulaire a zero
    bool ligneSelectionnee() const;   // true si une ligne du tableau est choisie
    void majBoutons();                // active/desactive Modifier et Supprimer

    // ---- Regles de gestion / outils ----
    bool validerSaisie();                                // controles de saisie
    bool pointageExiste(const QString &matricule, int mois, int annee);
    QString matriculeCourant() const;                    // matricule selectionne
    void afficherErreurSql(const QSqlQuery &q);          // message d'erreur base

    // ---- Widgets du formulaire ----
    QComboBox      *cbMatricule;     // employe (alimente depuis la base)
    QComboBox      *cbMois;          // 1 a 12
    QComboBox      *cbAnnee;         // annee
    QDoubleSpinBox *spHeuresSup;     // heures supplementaires
    QDoubleSpinBox *spJoursAbsence;  // jours d'absence (demi-journees permises)
    QLineEdit      *edRecherche;     // filtre matricule / nom

    // ---- Boutons (membres : on change leur etat actif/inactif) ----
    QPushButton    *btnAjouter;
    QPushButton    *btnModifier;
    QPushButton    *btnSupprimer;

    // ---- Cle de la ligne choisie dans le tableau (vide = aucune) ----
    QString         cleMatricule;
    int             cleMois  = 0;
    int             cleAnnee = 0;

    // ---- Affichage des donnees ----
    QTableView     *table;
    QSqlQueryModel *modele;          // modele en lecture seule
};

#endif // POINTAGE_H


CREATE TABLE IF NOT EXISTS POSTE (
    codePoste INT AUTO_INCREMENT PRIMARY KEY,
    Libelle VARCHAR(100) NOT NULL UNIQUE,
    SalaireBase DECIMAL(12,2) NOT NULL,
    TauxHeureSup DECIMAL(10,2) NOT NULL,
    CHECK (SalaireBase >= 0),
    CHECK (TauxHeureSup >= 0)
) ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS EMPLOYE (
    Matricule VARCHAR(20) PRIMARY KEY,
    Nom VARCHAR(100) NOT NULL,
    Adresse VARCHAR(255),
    dateEmbauche DATE NOT NULL,
    codePoste INT NOT NULL,
    CONSTRAINT fk_employe_poste
        FOREIGN KEY (codePoste)
        REFERENCES POSTE(codePoste)
        ON UPDATE CASCADE
        ON DELETE RESTRICT
) ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS POINTAGE (
    Matricule VARCHAR(20) NOT NULL,
    Mois TINYINT NOT NULL,
    Annee SMALLINT NOT NULL,
    HeuresSup DECIMAL(8,2) NOT NULL DEFAULT 0,
    JoursAbsence DECIMAL(5,2) NOT NULL DEFAULT 0,
    PRIMARY KEY (Matricule, Mois, Annee),
    CONSTRAINT fk_pointage_employe
        FOREIGN KEY (Matricule)
        REFERENCES EMPLOYE(Matricule)
        ON UPDATE CASCADE
        ON DELETE RESTRICT,
    CHECK (Mois BETWEEN 1 AND 12),
    CHECK (HeuresSup >= 0),
    CHECK (JoursAbsence >= 0)
) ENGINE=InnoDB;

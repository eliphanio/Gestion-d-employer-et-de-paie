-- Jeu d'essai pour la base gestion_paie
-- A exécuter APRES la création des tables (l'appli les crée au démarrage) :
--   mysql -u root -p gestion_paie < database/jeu_essai.sql
-- Rejouable sans erreur : INSERT IGNORE ne duplique rien.

-- 10 postes (3 sans employé : Commercial, Magasinier, Stagiaire)
INSERT IGNORE INTO POSTE (Libelle, SalaireBase, TauxHeureSup) VALUES
('Directeur',        1500000, 12000),
('Responsable RH',    700000,  5500),
('Comptable',         600000,  5000),
('Secrétaire',        450000,  3500),
('Informaticien',     800000,  6000),
('Commercial',        500000,  4000),
('Chauffeur',         350000,  3000),
('Gardien',           300000,  2500),
('Magasinier',        380000,  3000),
('Stagiaire',         200000,  2000);

-- 10 employés
-- E001 : 7 ans d'ancienneté (prime)      E002 : exactement 5 ans (prime)
-- E003 : 4 ans (pas de prime)            E010 : n'a pas de pointage en novembre
INSERT IGNORE INTO EMPLOYE (Matricule, Nom, Adresse, dateEmbauche, codePoste) VALUES
('E001', 'Rakoto Jean',     'Antsirabe',      '2019-03-01', (SELECT codePoste FROM POSTE WHERE Libelle = 'Comptable')),
('E002', 'Rabe Marie',      'Antsirabe',      '2021-01-15', (SELECT codePoste FROM POSTE WHERE Libelle = 'Comptable')),
('E003', 'Randria Paul',    'Antananarivo',   '2022-06-10', (SELECT codePoste FROM POSTE WHERE Libelle = 'Secrétaire')),
('E004', 'Rasoa Hanta',     'Fianarantsoa',   '2015-09-01', (SELECT codePoste FROM POSTE WHERE Libelle = 'Directeur')),
('E005', 'Andry Faly',      'Antsirabe',      '2024-02-20', (SELECT codePoste FROM POSTE WHERE Libelle = 'Informaticien')),
('E006', 'Rivo Tiana',      'Antsirabe',      '2020-11-05', (SELECT codePoste FROM POSTE WHERE Libelle = 'Informaticien')),
('E007', 'Mamy Solo',       'Antsirabe',      '2018-04-12', (SELECT codePoste FROM POSTE WHERE Libelle = 'Chauffeur')),
('E008', 'Njaka Toky',      'Antsirabe',      '2023-07-01', (SELECT codePoste FROM POSTE WHERE Libelle = 'Chauffeur')),
('E009', 'Voahangy Lala',   'Antsirabe',      '2017-01-09', (SELECT codePoste FROM POSTE WHERE Libelle = 'Gardien')),
('E010', 'Fenitra Dina',    'Antsirabe',      '2025-05-15', (SELECT codePoste FROM POSTE WHERE Libelle = 'Responsable RH'));

-- Pointages : octobre 2026 (10 employés) et novembre 2026 (9 employés, sans E010)
-- (Matricule, Mois, Annee, HeuresSup, JoursAbsence)
INSERT IGNORE INTO POINTAGE (Matricule, Mois, Annee, HeuresSup, JoursAbsence) VALUES
('E001', 10, 2026,  4, 2),   -- exemple du sujet : NET = 603500
('E002', 10, 2026,  0, 0),   -- 0 heure sup, 0 absence
('E003', 10, 2026,  6, 1),
('E004', 10, 2026,  2, 0),
('E005', 10, 2026, 10, 3),
('E006', 10, 2026,  5, 0),
('E007', 10, 2026,  8, 1),
('E008', 10, 2026,  3, 0),
('E009', 10, 2026,  0, 2),
('E010', 10, 2026,  4, 0),
('E001', 11, 2026,  2, 0),
('E002', 11, 2026,  5, 1),
('E003', 11, 2026,  0, 0),
('E004', 11, 2026,  0, 1),
('E005', 11, 2026,  6, 0),
('E006', 11, 2026,  4, 2),
('E007', 11, 2026,  0, 0),
('E008', 11, 2026,  7, 0),
('E009', 11, 2026,  1, 0);

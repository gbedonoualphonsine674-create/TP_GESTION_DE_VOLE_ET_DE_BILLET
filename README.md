# Billetterie d'avions – Projet C++ (POO)

## 1. Description
Application console de gestion d'une billetterie : vols, passagers, réservations, paiements.

## 2. Livrables
| Livrable | Fichier |
|---|---|
| Diagramme de classes UML | `diagramme_classes.puml` (source PlantUML) et `diagramme_classes.png` |
| Code source C++ complet | `billetterie.cpp` |
| Fichier aide-mémoire | `README.md` (ce fichier) |
| Captures d'écran de l'exécution | `captures/capture1.png`, `capture2.png`, `capture3.png` |

## 3. Compilation et exécution
    g++ -std=c++17 -Wall -o billetterie billetterie.cpp
    ./billetterie        (Windows : billetterie.exe)

## 4. Menu
1 Enregistrer un passager · 2 Ajouter un vol · 3 Afficher les vols disponibles · 4 Rechercher un vol ·
5 Effectuer une réservation · 6 Enregistrer un paiement · 7 Annuler une réservation ·
8 Consulter les réservations · 9 Afficher les informations d'une réservation · 10 Quitter

## 5. Règles de gestion
- R1 : un passager doit être enregistré avant de réserver.
- R2 : un vol ne peut pas recevoir plus de réservations que sa capacité.
- R3 : un passager ne peut pas réserver deux fois le même vol.
- R4 : une réservation est liée à un seul vol et un seul passager.
- R5 (hypothèse) : une réservation est « en attente » à sa création, « payée » après paiement du montant exact, « annulée » sinon.
- R6 (hypothèse) : une annulation libère la place sur le vol ; on ne paie pas une réservation annulée ou déjà payée.

## 6. Classes
- `Passager` (id, nom, prénom, téléphone)
- `Vol` (numéro, départ, destination, date, capacité, placesRéservées, prix)
- `Reservation` (id, statut, → Passager, → Vol)
- `Paiement` (id, montant, mode, → Reservation)
- `Agence` : contrôleur détenant les `vector` et les opérations du menu
- `Statut` : énumération EnAttente / Payee / Annulee

## 7. Principes POO appliqués
Encapsulation (attributs privés + accesseurs), constructeurs, méthodes dédiées,
associations par `shared_ptr`, collections `std::vector`, contrôle des saisies.

## 8. Captures
1 : ajout de vols, affichage et recherche · 2 : règles R1 et R3 (erreurs) · 3 : paiement, consultation, annulation.

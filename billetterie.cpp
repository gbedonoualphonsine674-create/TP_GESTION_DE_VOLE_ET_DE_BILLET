// Billetterie d'avions - application console (C++17, POO)
// Compilation : g++ -std=c++17 -o billetterie billetterie.cpp
#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>
using namespace std;

enum class Statut { EnAttente, Payee, Annulee };

string statutTexte(Statut s) {
    switch (s) {
        case Statut::EnAttente: return "En attente de paiement";
        case Statut::Payee:     return "Payee (validee)";
        default:                return "Annulee";
    }
}

// ---------- Saisie sécurisée ----------
int lireEntier(const string& msg, int mini = numeric_limits<int>::min(),
               int maxi = numeric_limits<int>::max()) {
    int v;
    while (true) {
        cout << msg;
        if (cin >> v && v >= mini && v <= maxi) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return v;
        }
        if (cin.eof()) exit(0);
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  Saisie invalide, reessayez.\n";
    }
}

double lireReel(const string& msg, double mini = 0.01) {
    double v;
    while (true) {
        cout << msg;
        if (cin >> v && v >= mini) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return v;
        }
        if (cin.eof()) exit(0);
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  Saisie invalide, reessayez.\n";
    }
}

string lireTexte(const string& msg) {
    string s;
    while (true) {
        cout << msg;
        if (!getline(cin, s)) exit(0);
        if (!s.empty()) return s;
        cout << "  Le champ ne peut pas etre vide.\n";
    }
}

// ---------- Classes métier ----------
class Passager {
    int id_;
    string nom_, prenom_, telephone_;
public:
    Passager(int id, string nom, string prenom, string tel)
        : id_(id), nom_(move(nom)), prenom_(move(prenom)), telephone_(move(tel)) {}
    int getId() const { return id_; }
    string nomComplet() const { return prenom_ + " " + nom_; }
    void afficher() const {
        cout << "  [" << id_ << "] " << nomComplet() << " - Tel: " << telephone_ << "\n";
    }
};

class Vol {
    int numero_;
    string depart_, destination_, date_;
    int capacite_, placesReservees_;
    double prix_;
public:
    Vol(int numero, string dep, string dest, string date, int cap, double prix)
        : numero_(numero), depart_(move(dep)), destination_(move(dest)),
          date_(move(date)), capacite_(cap), placesReservees_(0), prix_(prix) {}
    int getNumero() const { return numero_; }
    double getPrix() const { return prix_; }
    const string& getDepart() const { return depart_; }
    const string& getDestination() const { return destination_; }
    int placesRestantes() const { return capacite_ - placesReservees_; }
    bool estComplet() const { return placesReservees_ >= capacite_; }
    bool reserverPlace() {
        if (estComplet()) return false;
        ++placesReservees_;
        return true;
    }
    void libererPlace() { if (placesReservees_ > 0) --placesReservees_; }
    void afficher() const {
        cout << "  Vol " << numero_ << " : " << depart_ << " -> " << destination_
             << " | " << date_ << " | " << prix_ << " $ | places libres: "
             << placesRestantes() << "/" << capacite_ << "\n";
    }
};

class Reservation {
    int id_;
    shared_ptr<Passager> passager_;   // association : 1 passager
    shared_ptr<Vol> vol_;             // association : 1 vol
    Statut statut_;
public:
    Reservation(int id, shared_ptr<Passager> p, shared_ptr<Vol> v)
        : id_(id), passager_(move(p)), vol_(move(v)), statut_(Statut::EnAttente) {}
    int getId() const { return id_; }
    Statut getStatut() const { return statut_; }
    void setStatut(Statut s) { statut_ = s; }
    shared_ptr<Passager> getPassager() const { return passager_; }
    shared_ptr<Vol> getVol() const { return vol_; }
    void afficher() const {
        cout << "  Reservation #" << id_ << " | " << passager_->nomComplet()
             << " | Vol " << vol_->getNumero() << " (" << vol_->getDepart() << " -> "
             << vol_->getDestination() << ") | " << statutTexte(statut_) << "\n";
    }
};

class Paiement {
    int id_;
    shared_ptr<Reservation> reservation_;
    double montant_;
    string mode_;
public:
    Paiement(int id, shared_ptr<Reservation> r, double m, string mode)
        : id_(id), reservation_(move(r)), montant_(m), mode_(move(mode)) {}
    void afficher() const {
        cout << "  Paiement #" << id_ << " : " << montant_ << " $ par " << mode_
             << " (reservation #" << reservation_->getId() << ")\n";
    }
};

// ---------- Contrôleur : gère les collections ----------
class Agence {
    vector<shared_ptr<Passager>> passagers_;
    vector<shared_ptr<Vol>> vols_;
    vector<shared_ptr<Reservation>> reservations_;
    vector<shared_ptr<Paiement>> paiements_;
    int idP_ = 1, idR_ = 1, idPay_ = 1;

    shared_ptr<Passager> trouverPassager(int id) const {
        for (auto& p : passagers_) if (p->getId() == id) return p;
        return nullptr;
    }
    shared_ptr<Vol> trouverVol(int n) const {
        for (auto& v : vols_) if (v->getNumero() == n) return v;
        return nullptr;
    }
    shared_ptr<Reservation> trouverReservation(int id) const {
        for (auto& r : reservations_) if (r->getId() == id) return r;
        return nullptr;
    }
public:
    void enregistrerPassager() {
        string nom = lireTexte("Nom : ");
        string prenom = lireTexte("Prenom : ");
        string tel = lireTexte("Telephone : ");
        passagers_.push_back(make_shared<Passager>(idP_, nom, prenom, tel));
        cout << "  -> Passager enregistre avec l'identifiant " << idP_++ << "\n";
    }

    void ajouterVol() {
        int num = lireEntier("Numero du vol : ", 1);
        if (trouverVol(num)) { cout << "  Erreur : ce numero de vol existe deja.\n"; return; }
        string dep = lireTexte("Ville de depart : ");
        string dest = lireTexte("Destination : ");
        string date = lireTexte("Date (jj/mm/aaaa) : ");
        int cap = lireEntier("Capacite : ", 1, 1000);
        double prix = lireReel("Prix du billet : ");
        vols_.push_back(make_shared<Vol>(num, dep, dest, date, cap, prix));
        cout << "  -> Vol ajoute.\n";
    }

    void afficherVolsDisponibles() const {
        bool trouve = false;
        for (auto& v : vols_)
            if (!v->estComplet()) { v->afficher(); trouve = true; }
        if (!trouve) cout << "  Aucun vol disponible.\n";
    }

    void rechercherVol() const {
        string cle = lireTexte("Destination recherchee : ");
        bool trouve = false;
        for (auto& v : vols_)
            if (v->getDestination() == cle) { v->afficher(); trouve = true; }
        if (!trouve) cout << "  Aucun vol vers " << cle << ".\n";
    }

    void effectuerReservation() {
        int idp = lireEntier("Identifiant du passager : ", 1);
        auto p = trouverPassager(idp);
        if (!p) { cout << "  Erreur : passager non enregistre.\n"; return; }          // R1
        int nv = lireEntier("Numero du vol : ", 1);
        auto v = trouverVol(nv);
        if (!v) { cout << "  Erreur : vol introuvable.\n"; return; }
        for (auto& r : reservations_)                                                  // R3
            if (r->getPassager() == p && r->getVol() == v && r->getStatut() != Statut::Annulee) {
                cout << "  Erreur : ce passager a deja reserve ce vol.\n";
                return;
            }
        if (!v->reserverPlace()) { cout << "  Erreur : vol complet.\n"; return; }    // R2
        reservations_.push_back(make_shared<Reservation>(idR_, p, v));
        cout << "  -> Reservation #" << idR_++ << " creee (en attente de paiement).\n";
    }

    void enregistrerPaiement() {
        int id = lireEntier("Numero de reservation : ", 1);
        auto r = trouverReservation(id);
        if (!r) { cout << "  Erreur : reservation introuvable.\n"; return; }
        if (r->getStatut() == Statut::Annulee) { cout << "  Erreur : reservation annulee.\n"; return; }
        if (r->getStatut() == Statut::Payee) { cout << "  Reservation deja payee.\n"; return; }
        double prix = r->getVol()->getPrix();
        cout << "  Montant a payer : " << prix << " $\n";
        double m = lireReel("Montant verse : ");
        if (m != prix) { cout << "  Erreur : le montant doit etre exactement " << prix << " $.\n"; return; }
        string mode = lireTexte("Mode de paiement (carte/cash/virement) : ");
        paiements_.push_back(make_shared<Paiement>(idPay_++, r, m, mode));
        r->setStatut(Statut::Payee);
        cout << "  -> Paiement enregistre, reservation validee.\n";
    }

    void annulerReservation() {
        int id = lireEntier("Numero de reservation a annuler : ", 1);
        auto r = trouverReservation(id);
        if (!r) { cout << "  Erreur : reservation introuvable.\n"; return; }
        if (r->getStatut() == Statut::Annulee) { cout << "  Deja annulee.\n"; return; }
        r->getVol()->libererPlace();
        r->setStatut(Statut::Annulee);
        cout << "  -> Reservation annulee, place liberee.\n";
    }

    void consulterReservations() const {
        if (reservations_.empty()) { cout << "  Aucune reservation.\n"; return; }
        for (auto& r : reservations_) r->afficher();
    }

    void afficherInfosReservation() const {
        int id = lireEntier("Numero de reservation : ", 1);
        auto r = trouverReservation(id);
        if (!r) { cout << "  Erreur : reservation introuvable.\n"; return; }
        r->afficher();
        r->getPassager()->afficher();
        r->getVol()->afficher();
    }
};

// ---------- Programme principal ----------
void afficherMenu() {
    cout << "\n===== BILLETTERIE D'AVIONS =====\n"
         << " 1- Enregistrer un passager\n"
         << " 2- Ajouter un vol\n"
         << " 3- Afficher les vols disponibles\n"
         << " 4- Rechercher un vol\n"
         << " 5- Effectuer une reservation\n"
         << " 6- Enregistrer un paiement\n"
         << " 7- Annuler une reservation\n"
         << " 8- Consulter les reservations\n"
         << " 9- Afficher les informations d'une reservation\n"
         << "10- Quitter\n";
}

int main() {
    Agence agence;
    int choix;
    do {
        afficherMenu();
        choix = lireEntier("Votre choix : ", 1, 10);
        switch (choix) {
            case 1: agence.enregistrerPassager(); break;
            case 2: agence.ajouterVol(); break;
            case 3: agence.afficherVolsDisponibles(); break;
            case 4: agence.rechercherVol(); break;
            case 5: agence.effectuerReservation(); break;
            case 6: agence.enregistrerPaiement(); break;
            case 7: agence.annulerReservation(); break;
            case 8: agence.consulterReservations(); break;
            case 9: agence.afficherInfosReservation(); break;
            case 10: cout << "Au revoir !\n"; break;
        }
    } while (choix != 10);
    return 0;
}

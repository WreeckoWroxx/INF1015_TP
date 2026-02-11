#include "Jeu.hpp"
#include <iostream>
#include <fstream>
#include <span>
#include "gsl/span" 
#include "cppitertools/range.hpp"
#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp"
#include "debogage_memoire.hpp" // Ajout des numéros de ligne des "new" dans le rapport de fuites.
// Doit être après les include du système, qui peuvent utiliser des
// "placement new" (non supporté par notre ajout de numéros de lignes).

using namespace std;
using namespace gsl;
using namespace iter;
using gsl::span;

#pragma region "Fonctions de base pour vous aider"
typedef uint8_t UInt8;
typedef uint16_t UInt16;
UInt8 lireUint8(istream& fichier)
{
	UInt8 valeur = 0;
	fichier.read((char*)&valeur, sizeof(valeur));
	return valeur;
}
UInt16 lireUint16(istream& fichier)
{
	UInt16 valeur = 0;
	fichier.read((char*)&valeur, sizeof(valeur));
	return valeur;
}
string lireString(istream& fichier)
{
	string texte;
	texte.resize(lireUint16(fichier));
	fichier.read((char*)&texte[0], streamsize(sizeof(texte[0])) * texte.length());
	return texte;
}
#pragma endregion


Designer* chercherDesigner(ListeJeux& listeJeux, Designer& designer) {
	for (Jeu* jeu : gsl::span(listeJeux.elements, listeJeux.nElements)) {
		for (Designer* autreDesigner : gsl::span(jeu->designers.elements,
			jeu->designers.nElements)) {
			if (designer.nom == autreDesigner->nom) {
				return autreDesigner;
			}
		}
	}
	return nullptr;
}

Designer* lireDesigner(istream& fichier, ListeJeux& listeJeux)
{
	Designer designer = {};
	designer.nom = lireString(fichier);
	designer.anneeNaissance = lireUint16(fichier);
	designer.pays = lireString(fichier);

	Designer* d = chercherDesigner(listeJeux, designer);
	if (d == nullptr) {
		d = new Designer(designer);
		d->listeJeuxParticipes = { 0, 1, new Jeu * [1] };
	}
	return d;
}

void changerTailleTableauJeux(ListeJeux& tableauJeux) {
	int capaciteDoublee = 2 * tableauJeux.capacite;
	Jeu** nouveauTableauJeux = new Jeu * [capaciteDoublee];
	tableauJeux.capacite = capaciteDoublee;
	for (auto i : range(tableauJeux.nElements)) {
		nouveauTableauJeux[i] = tableauJeux.elements[i];
	}
	delete[] tableauJeux.elements;
	tableauJeux.elements = nouveauTableauJeux;
}

void ajouterJeuDansListe(ListeJeux& listeJeux, Jeu* jeu) {
	// Ligne non exécutée, mais nécessaire. Il faut s'assurer qu'une liste 
	// ait au moins une capacité de 1 si nous voulons doubler sa capacité.
	if (listeJeux.capacite < 1) {
		listeJeux.capacite = 1;
	}

	if (listeJeux.nElements == listeJeux.capacite) {
		changerTailleTableauJeux(listeJeux);
	}
	listeJeux.elements[listeJeux.nElements] = jeu;
	listeJeux.nElements += 1;
}

bool enleverJeuDeListe(Designer* d, int indexJeuAEnlever) {
	d->listeJeuxParticipes.nElements--;

	if (d->listeJeuxParticipes.nElements == 0) {
		delete[] d->listeJeuxParticipes.elements;
		delete d;
		return true;
	}
	else {
		for (auto i : range(d->listeJeuxParticipes.nElements
			- indexJeuAEnlever)) {

			d->listeJeuxParticipes.elements[i + indexJeuAEnlever] =
				d->listeJeuxParticipes.elements[i + indexJeuAEnlever + 1];
		}
	}
	return false;
}

Jeu* lireJeu(istream& fichier, ListeJeux& listeJeux)
{
	Jeu jeu = {};
	jeu.titre = lireString(fichier);
	jeu.anneeSortie = lireUint16(fichier);
	jeu.developpeur = lireString(fichier);
	jeu.designers.nElements = lireUint8(fichier);

	Jeu* j = new Jeu(jeu);
	j->designers.elements = new Designer * [j->designers.nElements];
	j->designers.capacite = j->designers.nElements;

	for ([[maybe_unused]] int i : iter::range(jeu.designers.nElements)) {
		Designer* nouveauDesigner = lireDesigner(fichier, listeJeux);
		j->designers.elements[i] = nouveauDesigner;
		ajouterJeuDansListe(nouveauDesigner->listeJeuxParticipes, j);
	}
	return j;
}

ListeJeux creerListeJeux(const string& nomFichier)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);
	int nElements = lireUint16(fichier);

	ListeJeux listeJeux = { 0, 1, new Jeu * [1] };

	for ([[maybe_unused]] int n : iter::range(nElements))
	{
		Jeu* nouveauJeu = lireJeu(fichier, listeJeux);
		ajouterJeuDansListe(listeJeux, nouveauJeu);
	}

	return listeJeux;
}

void detruireJeu(Jeu* jeu) {
	for (Designer* d : gsl::span(jeu->designers.elements,
		jeu->designers.nElements)) {
		if (d != nullptr) {
			for (auto i : range(d->listeJeuxParticipes.nElements)) {
				if (d->listeJeuxParticipes.elements[i]->titre == jeu->titre) {
					bool designerDetruit = enleverJeuDeListe(d, i);
					if (designerDetruit) {
						break;
					}
				}
			}
		}
	}
	delete[] jeu->designers.elements;
	delete jeu;
}

void detruireListeJeux(ListeJeux& listeJeux) {
	if (listeJeux.elements != nullptr) {	 // pour eviter que le code crash
		for (auto i : range(listeJeux.nElements)) {
			if (listeJeux.elements[i] != nullptr) {
				detruireJeu(listeJeux.elements[i]);
			}
		}
	}
	delete[] listeJeux.elements;
	listeJeux.nElements = 0;
	listeJeux.capacite = 0;

	cout << "Liste de jeux detruite.\n";
}

void afficherInfosDesigner(const Designer& d) {
	cout << "Nom\t\t\t: " << d.nom << endl;
	cout << "Annee de naissance\t: " << d.anneeNaissance << endl;
	cout << "Pays\t\t\t: " << d.pays << endl;
	cout << "Jeux auxquels " << d.nom << " a participe ("
		<< d.listeJeuxParticipes.nElements << ") :\n";

	for (auto i : range(d.listeJeuxParticipes.nElements)) {
		cout << "\t\t\t  " << d.listeJeuxParticipes.elements[i]->titre << endl;
	}
}

void afficherInfosJeux(const Jeu& j) {
	cout << "Titre\t\t: " << j.titre << endl;
	cout << "Annee de sortie : " << j.anneeSortie << endl;
	cout << "Developpeur\t: " << j.developpeur << endl;
	cout << "Designers:" << endl;

	for (auto i : range(j.designers.nElements)) {
		cout << "\t\t  " << j.designers.elements[i]->nom << endl;
	}
}

void afficherListeJeux(ListeJeux& listeJeux) {
	static const string nouvelLigneSeparation =
		"\033[35m----------------------------------\033[0m\n";
	cout << "Liste de jeux complete : " << endl << endl;

	for (auto i : range(listeJeux.nElements)) {
		cout << "\t\t " << listeJeux.elements[i]->titre << endl;
		cout << "\t\t " << nouvelLigneSeparation;
	}
	cout << "\n\n";
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
#pragma region "Bibliothèque du cours"
	// Permet sous Windows les "ANSI escape code" pour changer de couleur
	// https://en.wikipedia.org/wiki/ANSI_escape_code ; les consoles Linux/Mac
	// les supportent normalement par défaut.
	bibliotheque_cours::activerCouleursAnsi();
#pragma endregion

	//int* fuite = new int;  // Pour vérifier que la détection de fuites fonctionne; un message devrait dire qu'il y a une fuite à cette ligne.

	ListeJeux listeJeux = creerListeJeux("jeux.bin");

	static const string ligneSeparation =
		"\n\033[35m════════════════════════════════════════\033[0m\n";
	cout << ligneSeparation << endl;
	cout << "Premier jeu de la liste :" << endl;
	afficherInfosJeux(*(listeJeux.elements[0]));
	cout << ligneSeparation << endl;

	afficherListeJeux(listeJeux);

	afficherInfosDesigner(*(listeJeux.elements[0]->designers.elements[0]));

	detruireListeJeux(listeJeux);
}
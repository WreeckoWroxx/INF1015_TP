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

//TODO: Fonction qui cherche un designer par son nom dans une ListeJeux.  Devrait utiliser span.

Designer* chercherDesigner(ListeJeux& listeJeux, Designer& designer) {
	for (Jeu* jeu : span(listeJeux.elements, listeJeux.nElements)) {
		for (Designer* autreDesigner : span(jeu->designers.elements, jeu->designers.nElements)) {
			if (designer.nom == autreDesigner->nom) {
				cout << "Même designer : " << autreDesigner->nom << endl;
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
		d->listeJeuxParticipes = ListeJeux{ 0, 1, new Jeu * [1] };
		cout << "Nouveau designer : " << d->nom << endl;
	}
	return d;
}

//TODO: Fonction qui change la taille du tableau de jeux de ListeJeux (listeJeuxParticipes).
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

//TODO: Fonction pour ajouter un Jeu à ListeJeux (listeJeuxParticipes).
void ajouterJeuDansListe(ListeJeux& listeJeux, Jeu* jeu) {
	if (listeJeux.capacite == 0) {
		listeJeux.capacite = 1;
	}
	if (listeJeux.nElements == listeJeux.capacite) {
		changerTailleTableauJeux(listeJeux);
	}
	listeJeux.elements[listeJeux.nElements] = jeu;
	listeJeux.nElements += 1;
}

//TODO: Fonction qui enlève un jeu de ListeJeux (listeJeuxParticipes).
//Fait  
void enleverJeuDeListe(Designer* d, const Jeu* jeuAEnlever) {
	if (d->listeJeuxParticipes.nElements == 0 || d->listeJeuxParticipes.elements == nullptr) { //|| jeuAEnlever == nullptr) {
		return;
	}
	for (int i = 0; i < d->listeJeuxParticipes.nElements; i++) {
		if (d->listeJeuxParticipes.elements[i]->titre == jeuAEnlever->titre) {
			//d->listeJeuxParticipes.elements[i] = nullptr;
			//delete d->listeJeuxParticipes.elements[i];
			d->listeJeuxParticipes.nElements--;

			if (d->listeJeuxParticipes.nElements == 0) {
				delete[] d->listeJeuxParticipes.elements;
				delete d;
			}
			else {
				for (int j = i; j < d->listeJeuxParticipes.nElements; j++) {
					d->listeJeuxParticipes.elements[j] = d->listeJeuxParticipes.elements[j + 1];
				}
			}
		}
	}
	/*
	unsigned indexID = 0;
	while (indexID < listeJeux.nElements && listeJeux.elements[indexID] != jeuAEnlever) {
		++indexID;
		if (indexID == listeJeux.nElements) {
			return false;
		}
		const unsigned indexDernierElement = listeJeux.nElements - 1;
		if (indexID != indexDernierElement) {
			listeJeux.elements[indexID] = listeJeux.elements[indexDernierElement];
		}
		listeJeux.elements[indexDernierElement] = nullptr;
		--listeJeux.nElements;

		return true;
	}
	*/
}

Jeu* lireJeu(istream& fichier, ListeJeux& listeJeux)
{
	Jeu jeu = {};
	jeu.titre = lireString(fichier);
	jeu.anneeSortie = lireUint16(fichier);
	jeu.developpeur = lireString(fichier);
	jeu.designers.nElements = lireUint8(fichier);

	//TODO: Ajouter en mémoire le jeu lu. Il faut renvoyer le pointeur créé.
	//Fait
	Jeu* j = new Jeu(jeu);
	j->designers.elements = new Designer * [j->designers.nElements];
	j->designers.capacite = j->designers.nElements;
	cout << j->titre << endl;
	// A faire a la fin	// TIP: Afficher un message lorsque l'allocation du jeu est réussie pour aider au débogage.
						// Vous pouvez enlever l'affichage une fois que le tout fonctionne.


	for ([[maybe_unused]] int i : iter::range(jeu.designers.nElements)) {
		Designer* nouveauDesigner = lireDesigner(fichier, listeJeux);  //TODO: Mettre le designer dans la liste des designer du jeu.
		j->designers.elements[i] = nouveauDesigner;
		//TODO: Ajouter le jeu à la liste des jeux auquel a participé le designer.
		ajouterJeuDansListe(nouveauDesigner->listeJeuxParticipes, j);// jeu.designers.elements[i].listeJeuxParticipes;
	}
	return j; //TODO: Retourner le pointeur vers le nouveau jeu.
}

ListeJeux creerListeJeux(const string& nomFichier)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);
	int nElements = lireUint16(fichier);

	ListeJeux listeJeux = { 0, 1, new Jeu * [1] };

	for ([[maybe_unused]] int n : iter::range(nElements))
	{
		Jeu* nouveauJeu = lireJeu(fichier, listeJeux); //TODO: Ajouter le jeu à la ListeJeux.
		ajouterJeuDansListe(listeJeux, nouveauJeu);
		//listeJeux.elements[n] = nouveauJeu;
	}

	return listeJeux; //TODO: Renvoyer la ListeJeux.
}

//TODO: Fonction pour détruire un jeu (libération de mémoire allouée).
//Fait
void detruireJeu(ListeJeux& listeJeux, Jeu* jeu) {

	if (jeu == nullptr) {
		return;
	}
	else if (jeu->designers.elements != nullptr &&
		jeu->designers.nElements > 0) {
		for (Designer* d : span(jeu->designers.elements, jeu->designers.nElements)) {//(unsigned i = 0; i < jeu->designers.nElements; ++i) {//(Designer* d : jeu->designers.elements) {
			if (d != nullptr) {
				//Designer* d = jeu->designers.elements[i];
				for (Jeu* j : span(d->listeJeuxParticipes.elements, d->listeJeuxParticipes.nElements)) {
					if (jeu->titre == j->titre) {
						enleverJeuDeListe(d, j);
					}
				}
			}
		}
	}
	//for (int i = 0; i < listeJeux.nElements; i++) {
	//	if (listeJeux.elements[i]->titre == jeu->titre) {
	//		listeJeux.nElements--;
	//		for (int j = i; j < listeJeux.nElements; j++) {
	//			listeJeux.elements[j] = listeJeux.elements[j + 1];
	//		}
	//		break;
	//	}
	//}
	//jeu->designers.elements = nullptr;
	//jeu->designers.nElements = 0;
	//jeu->designers.capacite = 0;
	cout << "Destruction du jeu : " << jeu->titre << endl; // Vous pouvez enlever l'affichage une fois que le tout fonctionne.
	delete[] jeu->designers.elements;
	delete jeu;
	//cout << "Destruction du jeu : " << jeu->titre << endl;
}

//TODO: Fonction pour détruire une ListeJeux et tous ses jeux.
//Fait
void detruireListeJeux(ListeJeux& listeJeux) {
	if (listeJeux.elements != nullptr) {	 // pour eviter que le code crash
		for (unsigned i = 0; i < listeJeux.nElements; ++i) {
			if (listeJeux.elements[i] != nullptr) {
				detruireJeu(listeJeux, listeJeux.elements[i]);
				//listeJeux.elements[i] = nullptr;
			}
		}
	}
	delete[] listeJeux.elements;
	listeJeux.elements = nullptr;
	listeJeux.nElements = 0;
	listeJeux.capacite = 0;

	cout << "Liste de jeux detruite.\n";
}

//TODO: Fonction pour afficher les infos d'un designer.
//Fait
void afficherInfosDesigner(const Designer& d) {
	cout << "Nom\t\t\t: " << d.nom << endl;
	cout << "Annee de naissance\t: " << d.anneeNaissance << endl;
	cout << "Pays\t\t\t: " << d.pays << endl;
	cout << "Jeux auxquels " << d.nom << " a participe ("
		<< d.listeJeuxParticipes.nElements << ") :\n";

	if (d.listeJeuxParticipes.elements == nullptr ||
		d.listeJeuxParticipes.nElements == 0) {
		cout << " (Il n'existe aucun)\n";
		return;
	}

	for (int i = 0; i < d.listeJeuxParticipes.nElements; i++) {
		cout << "\t\t\t  " << d.listeJeuxParticipes.elements[i]->titre << endl;
	}
}

//TODO: Fonction pour afficher les infos d'un jeu ainsi que ses designers.

void afficherInfosJeux(const Jeu& j) {
	cout << "Titre\t\t: " << j.titre << endl;
	cout << "Annee de sortie : " << j.anneeSortie << endl;
	cout << "Developpeur\t: " << j.developpeur << endl;
	cout << "Designers:" << endl;
	for (int i = 0; i < j.designers.nElements; i++) {
		cout << "\t\t  " << j.designers.elements[i]->nom << endl;
	}
}


//TODO: Fonction pour afficher tous les jeux de ListeJeux, séparés par un ligne.
// Votre ligne de séparation doit être différent de celle utilisée dans le main.
void afficherListeJeux(ListeJeux& listeJeux) {
	static const string nouvelLigneSeparation = "\033[35m----------------------------------\033[0m\n";
	cout << "Liste de jeux complete : " << endl << endl;
	for (unsigned i = 0; i < listeJeux.nElements; ++i) {
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

	int* fuite = new int;  // Pour vérifier que la détection de fuites fonctionne; un message devrait dire qu'il y a une fuite à cette ligne.

	ListeJeux listeJeux = creerListeJeux("jeux.bin"); //TODO: Appeler correctement votre fonction de création de la liste de jeux.

	static const string ligneSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";
	cout << ligneSeparation << endl;
	cout << "Premier jeu de la liste :" << endl;
	//TODO: Afficher le premier jeu de la liste (en utilisant la fonction).  Devrait être Chrono Trigger.
	afficherInfosJeux(*(listeJeux.elements[0]));
	cout << ligneSeparation << endl;

	//TODO: Appel à votre fonction d'affichage de votre liste de jeux.
	afficherListeJeux(listeJeux);

	//TODO: Faire les appels à toutes vos fonctions/méthodes pour voir qu'elles fonctionnent et avoir 0% de lignes non exécutées dans le programme (aucune ligne rouge dans la couverture de code; c'est normal que les lignes de "new" et "delete" soient jaunes).  Vous avez aussi le droit d'effacer les lignes du programmes qui ne sont pas exécutée, si finalement vous pensez qu'elle ne sont pas utiles.
	//afficherInfosDesigner(*(listeJeux.elements[0]->designers.elements[0]));

	//TODO: Détruire tout avant de terminer le programme.  Devrait afficher "Aucune fuite detectee." a la sortie du programme; il affichera "Fuite detectee:" avec la liste des blocs, s'il manque des delete.
	//detruireJeu(listeJeux, listeJeux.elements[1]);
	//afficherInfosJeux(*(listeJeux.elements[1]));
	detruireListeJeux(listeJeux);
}
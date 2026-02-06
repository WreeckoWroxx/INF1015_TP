#include "Jeu.hpp"
#include <iostream>
#include <fstream>
#include <span>
#include "gsl/span" 
#include "cppitertools/range.hpp"
#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp"
#include "debogage_memoire.hpp"  // Ajout des numéros de ligne des "new" dans le rapport de fuites.  Doit être après les include du système, qui peuvent utiliser des "placement new" (non supporté par notre ajout de numéros de lignes).

using namespace std;
using namespace gsl;
using namespace iter;

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
	// code ajouté
	Designer* ptrDesigner = chercherDesigner(listeJeux, designer);
	if (ptrDesigner == nullptr) {
		ptrDesigner = new Designer(designer);
		ptrDesigner->listeJeuxParticipes = ListeJeux{ 0, 1, new Jeu * [1] };
		cout << ptrDesigner->nom << endl;
	}


	//TODO: Ajouter en mémoire le designer lu. Il faut revoyer le pointeur créé.

	// TIP: Afficher un message lorsque l'allocation du designer est réussie pour aider au débogage.
	// Vous pouvez enlever l'affichage une fois que le tout fonctionne.


	//cout << designer.nom << endl;  //TODO: Enlever cet affichage temporaire servant à voir que le code fourni lit bien les jeux.
	return ptrDesigner; //TODO: Retourner le pointeur vers le designer crée.
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

Jeu* lireJeu(istream& fichier, ListeJeux& listeJeux)
{
	Jeu jeu = {};
	jeu.titre = lireString(fichier);
	jeu.anneeSortie = lireUint16(fichier);
	jeu.developpeur = lireString(fichier);
	jeu.designers.nElements = lireUint8(fichier);

	//TODO: Ajouter en mémoire le jeu lu. Il faut renvoyer le pointeur créé.
	Jeu* ptrJeu = new Jeu(jeu);
	ptrJeu->designers.elements = new Designer * [ptrJeu->designers.nElements];
	ptrJeu->designers.capacite = ptrJeu->designers.nElements;
	cout << ptrJeu->titre << endl;
	// TIP: Afficher un message lorsque l'allocation du jeu est réussie pour aider au débogage.
	// Vous pouvez enlever l'affichage une fois que le tout fonctionne.


	for ([[maybe_unused]] int i : iter::range(jeu.designers.nElements)) {
		Designer* nouveauDesigner = lireDesigner(fichier, listeJeux);  //TODO: Mettre le designer dans la liste des designer du jeu.
		ptrJeu->designers.elements[i] = nouveauDesigner;
		//TODO: Ajouter le jeu à la liste des jeux auquel a participé le designer.
		ajouterJeuDansListe(nouveauDesigner->listeJeuxParticipes, ptrJeu);// jeu.designers.elements[i].listeJeuxParticipes;
	}
	return ptrJeu; //TODO: Retourner le pointeur vers le nouveau jeu.
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
		listeJeux.elements[n] = nouveauJeu;
	}

	return listeJeux; //TODO: Renvoyer la ListeJeux.
}

//TODO: Fonction pour détruire un jeu (libération de mémoire allouée).
// TIP: Afficher un message lorsque le jeu est détruit pour aider au débogage.
// Vous pouvez enlever l'affichage une fois que le tout fonctionne.


//TODO: Fonction pour détruire une ListeJeux et tous ses jeux.

//TODO: Fonction pour afficher les infos d'un designer.

//TODO: Fonction pour afficher les infos d'un jeu ainsi que ses designers.


//TODO: Fonction pour afficher tous les jeux de ListeJeux, séparés par un ligne.
// Votre ligne de séparation doit être différent de celle utilisée dans le main.


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

	cout << ligneSeparation << endl;

	//TODO: Appel à votre fonction d'affichage de votre liste de jeux.

	//TODO: Faire les appels à toutes vos fonctions/méthodes pour voir qu'elles fonctionnent et avoir 0% de lignes non exécutées dans le programme (aucune ligne rouge dans la couverture de code; c'est normal que les lignes de "new" et "delete" soient jaunes).  Vous avez aussi le droit d'effacer les lignes du programmes qui ne sont pas exécutée, si finalement vous pensez qu'elle ne sont pas utiles.

	//ajouterJeuDansListe(listeJeux, );
	//TODO: Détruire tout avant de terminer le programme.  Devrait afficher "Aucune fuite detectee." a la sortie du programme; il affichera "Fuite detectee:" avec la liste des blocs, s'il manque des delete.
}
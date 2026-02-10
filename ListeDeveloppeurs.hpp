#pragma once
#include "Developpeur.hpp"

class ListeDeveloppeurs
{
	//TODO: En faire une classe qui suit les principes OO.
	//TODO: On veut pouvoir ajouter et enlever un Developpeur* de la liste, avec réallocation dynamique tel que faite pour ListeJeux.
	//NOTE: Le code sera principalement copié de certaines fonctions écrites pour la partie 1, mais mises dans une classe.
public:
	ListeDeveloppeurs(unsigned nElements = 0, unsigned capacite = 1, Developpeur** elements = new Developpeur*[0]); //crée une liste valide contenant aucun élément
	unsigned lireNElements() const;
	unsigned lireCapacite() const;
	void doublerCapacite();
	void ecrireNElements(unsigned nElements);
	void afficher(); // appeler afficher de Développeur
	void ajouterDeveloppeur(Developpeur* developpeur); // si capacité atteinte, doubler avec même code que changerTailleTableau de main.cpp
	void retirerDeveloppeur(Developpeur* developpeur); // passer dev déjà existant par adresse (comme pour ajouter)
	~ListeDeveloppeurs(); //  libère le tableau dynamique
private:
	
	unsigned nElements_;
	unsigned capacite_;
	Developpeur** elements_;
};

#pragma once
#include "Developpeur.hpp"

class ListeDeveloppeurs
{
public:
	ListeDeveloppeurs(unsigned nElements = 0, unsigned capacite = 1,
		Developpeur** elements = new Developpeur * [1]);
	unsigned lireNElements() const;
	unsigned lireCapacite() const;
	Developpeur** lireElements() const;
	void doublerCapacite();
	void ecrireNElements(unsigned nElements);
	void afficher() const;
	void ajouterDeveloppeur(Developpeur* developpeur);
	void retirerDeveloppeur(Developpeur* developpeur);
	~ListeDeveloppeurs();
private:
	unsigned nElements_;
	unsigned capacite_;
	Developpeur** elements_;
};
#include "ListeDeveloppeurs.hpp"
#include <iostream>
#include <span>
#include "gsl/span"
#include "cppitertools/range.hpp"

using namespace std;
using namespace gsl;
using namespace iter;
using gsl::span;

ListeDeveloppeurs::ListeDeveloppeurs(unsigned nElements, unsigned capacite,
	Developpeur** elements)
{
	nElements_ = nElements;
	capacite_ = capacite;
	elements_ = elements;
}

unsigned ListeDeveloppeurs::lireNElements() const
{
	return nElements_;
}

unsigned ListeDeveloppeurs::lireCapacite() const
{
	return capacite_;
}

Developpeur** ListeDeveloppeurs::lireElements() const
{
	return elements_;
}

void ListeDeveloppeurs::doublerCapacite()
{
	unsigned capaciteDoublee = lireCapacite() * 2;
	Developpeur** nouveauTableau = new Developpeur * [capaciteDoublee];
	Developpeur** vieuTableau = lireElements();
	for (auto i : range(lireNElements() + 1)) {
		nouveauTableau[i] = lireElements()[i];
	}
	capacite_ = capaciteDoublee;
	elements_ = nouveauTableau;
	delete[] vieuTableau;
}

void ListeDeveloppeurs::ecrireNElements(unsigned nElements)
{
	if (nElements >= lireCapacite()) {
		doublerCapacite();
	}

	nElements_ = nElements;
}

void ListeDeveloppeurs::afficher() const
{
	for (Developpeur* d : span(lireElements(), lireNElements())) {
		(*d).afficher();
	}
}

void ListeDeveloppeurs::ajouterDeveloppeur(Developpeur* developpeur)
{
	for (Developpeur* d : span(lireElements(), lireNElements())) {
		if ((*d).lireNom() == (*developpeur).lireNom()) {
			return;
		}
	}
	lireElements()[lireNElements()] = developpeur;
	ecrireNElements(lireNElements() + 1);
}

void ListeDeveloppeurs::retirerDeveloppeur(Developpeur* developpeur)
{
	for (auto i : range(lireNElements())) {
		if ((*lireElements()[i]).lireNom() == (*developpeur).lireNom()) {
			for (auto j : range(lireNElements() - i)) {
				lireElements()[j + i] = lireElements()[j + i + 1];
			}
			ecrireNElements(lireNElements() - 1);
			break;
		}
	}
}

ListeDeveloppeurs::~ListeDeveloppeurs()
{
	delete[] elements_;
}
#include "ListeDeveloppeurs.hpp"
#include <iostream>
#include <span>
#include "gsl/span"
#include "cppitertools/range.hpp"

using namespace std;
using namespace gsl;
using namespace iter;
using gsl::span;

ListeDeveloppeurs::ListeDeveloppeurs(unsigned nElements, unsigned capacite, Developpeur** elements) {
	nElements_ = nElements;
	capacite_ = capacite;
	elements_ = elements;
}

unsigned ListeDeveloppeurs::lireNElements() const {
	return nElements_;
}

unsigned ListeDeveloppeurs::lireCapacite() const {
	return capacite_;
}

void ListeDeveloppeurs::doublerCapacite() {
	unsigned capaciteDoublee = lireCapacite() * 2;
	Developpeur** nouveauTableau = new Developpeur*[capaciteDoublee];
	for (auto i : range(lireNElements())) {
		nouveauTableau[i] = elements_[i];
	}
	delete[] elements_;
	elements_ = nouveauTableau;
}

void ListeDeveloppeurs::ecrireNElements(unsigned nElements) {
	while (nElements >= lireCapacite()) {
		doublerCapacite();
	}

	nElements_ = nElements;
}

void ListeDeveloppeurs::afficher() {
	for (Developpeur* d : span(elements_, lireNElements())) {
		cout << (*d).afficher(); // TODO : afficher un développeur
	}
}

void ListeDeveloppeurs::ajouterDeveloppeur(Developpeur* developpeur) {
	for (Developpeur* d : span(elements_, lireNElements())) {
		if ((*d).lireNom() == (*developpeur).lireNom()) { // TODO : accéder au nom d'un développeur
			cout << "Le développeur " << (*developpeur).lireNom() << " est déjà dans la liste.";
			return;
		}
	}
	elements_[lireNElements()] = developpeur;
	ecrireNElements(lireNElements() + 1);
}

void ListeDeveloppeurs::retirerDeveloppeur(Developpeur* developpeur) {
	for (auto i : range(lireNElements())) {
		if ((*elements_[i]).lireNom() == (*developpeur).lireNom()) { // TODO : accéder au nom d'un développeur
			for (auto j : range(lireNElements() - i)) {
				elements_[j+i] = elements_[j+i+1];
			}
			// retirer de liste sans delete; on delete les développeurs ainsi que la listedéveloppeur à la fin du main
		}
	}
}

ListeDeveloppeurs::~ListeDeveloppeurs() {
	delete[] elements_;
}
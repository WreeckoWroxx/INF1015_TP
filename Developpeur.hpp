#pragma once
#include <string>
#include <utility>
#include "ListeJeux.hpp"
#include "gsl/span"
using namespace std;

class Developpeur
{
	//TODO: En faire une classe qui suit les principes OO (oriente objet).
public:

	Developpeur(const string& nom);
	~Developpeur();

	const string& lireNom() const;
	int compterJeux(gsl::span<Jeu*> tousLesJeux) const;
	void mettreAJourJeux(gsl::span<Jeu*> tousLesJeux);
	void afficher() const;

private: 
	pair <string, ListeJeux> paireNomJeux_;
};

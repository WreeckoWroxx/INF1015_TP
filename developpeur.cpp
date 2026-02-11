#include <iostream>
#include "gsl/span"  

#include "Developpeur.hpp"
#include "Jeu.hpp"

using namespace std;
using gsl::span;

Developpeur::Developpeur(const string& nom)
    : paireNomJeux_{ nom, {0, 0, nullptr} } 
{ }

Developpeur::~Developpeur()
{
   delete[] paireNomJeux_.second.elements;
}
const string& Developpeur::lireNom() const
{
    return paireNomJeux_.first; 
}

int Developpeur::compterJeux(gsl::span<Jeu*> tousLesJeux) const
{
    int compteur = 0;
    for (Jeu* jeu : tousLesJeux) 
    {
        if (jeu->developpeur == paireNomJeux_.first)
        {
            compteur++;
        }
    }
    return compteur;
}

void Developpeur::mettreAJourJeux(gsl::span<Jeu*> tousLesJeux)
{
    delete[] paireNomJeux_.second.elements;
    paireNomJeux_.second.elements = nullptr;

    unsigned nMatch = static_cast<unsigned>(compterJeux(tousLesJeux));
    paireNomJeux_.second.nElements = nMatch;
    paireNomJeux_.second.capacite = nMatch;

    if (nMatch != 0) {
        paireNomJeux_.second.elements = new Jeu * [nMatch];

        unsigned index = 0;
        for (Jeu* jeu : tousLesJeux)
        {
            if (jeu->developpeur == paireNomJeux_.first && index < nMatch)
            {
                paireNomJeux_.second.elements[index++] = jeu;
            }
        }
    }
}


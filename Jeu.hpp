#pragma once
#include <string>
#include "ListeDesigners.hpp"

struct Jeu
{
	std::string titre;
	unsigned int anneeSortie;
	std::string developpeur;
	ListeDesigners designers;
};

#pragma once
// La déclaration suivante plutôt qu'un #include "Jeu.hpp" est nécessaire sinon il y aurait une boucle d'inclusion (ListeJeux.hpp > Jeu.hpp > ListeDesigners.hpp > Designer.hpp > ListeJeux.hpp), la boucle serait arrêtée par le "pragma once" mais ListeJeux ne serait alors pas déclaré lors de la compilation de Designer.
struct Jeu;  // Déclaration avancée ("forward declaration") pour pouvoir utilise le nom du type avant qu'il soit complètement défini.

struct ListeJeux
{
	unsigned nElements;
	unsigned capacite;
	Jeu** elements;
};

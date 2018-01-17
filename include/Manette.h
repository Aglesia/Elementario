#ifndef MANETTE_H
#define MANETTE_H

#include <Touche.h>
#include <defs.h>

class Manette // TODO : Surcharger le copy-constructeur
{
	public:
		Manette();
		virtual ~Manette();

		void ajoutTouche(Touche* touche);
		Touche* getTouche(int noBouton, int typeBouton);
		std::vector<Touche*> getTouches();

		int id; // Identifiant de la manette
		SDL_Joystick* joy; // Adresse du périphérique
		int nbJoystick = 0;
		int nbBouton = 0;
		int nbDirection = 0;
		int nbMolette = 0;
		std::string name; // Nom de la manette

	private:
		std::vector<Touche*> touches; // Ensemble des inputs de la manette
};

#endif // MANETTE_H
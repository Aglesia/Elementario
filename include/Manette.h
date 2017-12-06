#ifndef MANETTE_H
#define MANETTE_H

#include <defs.h>

class Manette // TODO : Surcharger le copy-constructeur
{
	public:
		Manette();
		virtual ~Manette();

		int id; // Identifiant de la manette
		SDL_Joystick* joy; // Adresse du périphérique
		int nbJoystick = 0;
		int nbBouton = 0;
		int nbDirection = 0;
		int nbMolette = 0;
		std::string name;
};

#endif // MANETTE_H

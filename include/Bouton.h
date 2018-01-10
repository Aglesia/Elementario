#ifndef BOUTON_H
#define BOUTON_H

#include <defs.h>

class Bouton // TODO : Surcharger le copy-constructeur et jouer un son à la sélection
{
	public:
		Bouton(SDL_Surface* boutonSurface, SDL_Renderer* renderer, int taille);
		virtual ~Bouton();

		/**
		 * Initialise la texture via la surface donnée
		 * Doit être lancé dans le thread principale
		 */
		void init();

		/**
		 * Affiche le bouton sur la texture, en plaçant le centre à l'emplacement indiqué
		 * @param pos           Position du centre du bouton
		 * @param nbTicks       Nombre de ticks actuel
		 * @param etatSelection Etat actuel du bouton (sélectionné ou non)
		 */
		void afficher(SDL_Rect* p, unsigned int nbTicks, bool etatSelection = false, int opacite = 255);

		/**
		 * Définit le texte à afficher sous le bouton
		 * @param texte texte
		 */
		void setTitre(std::string texte);
		/**
		 * Modifie la taille de référence du bouton (zoom)
		 * @param taille nouvelle taille
		 */
		void setTaille(int taille);

	protected:

	private:
		unsigned int nbTicksAnimation = 0; // Nombre de ticks lors du changement d'état
		int taille = 0; // Taille (en pix²) du bouton normal
		int etatAnimation = 0; // Animation actuelle (en cours de sélection, en cours de déselection, sélectionné ou relâché)
		SDL_Surface* boutonSurface = nullptr; // Bouton (surface)
		SDL_Texture* boutonTexture = nullptr; // Bouton (texture)
		SDL_Texture* texteTexture = nullptr; // Texte du bouton
		SDL_Renderer* renderer = nullptr; // Renderer de la fenêtre
};

#endif // MANETTE_H
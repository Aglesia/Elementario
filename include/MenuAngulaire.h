#ifndef MENU_ANGULAIRE_H
#define MENU_ANGULAIRE_H

#include <Menu.h>

class MenuAngulaire : public Menu
{
	public:
		MenuAngulaire(ToucheJeu** touches, SDL_Renderer* renderer, TTF_Font* policeMenu);
		~MenuAngulaire();

		/**
		 * Ajoute une catégorie de boutons au menu
		 * @param boutons     Ensemble des boutons contenus dans la catégorie, le premier est le bouton représentant la catégorie, qui sera mis dans la barre des catégories, en haut
		 */
		void ajoutCategorie(std::vector<Bouton*> boutons);

		/**
		 * Passe le menu sous la catégorie indiquée
		 * @param numCategorie Numéro de la catégorie à sélectionner
		 */
		void selectCategorie(int numCategorie);

		/**
		 * Met à jour les actions boutons, puis l'affichage selon les déplacements/sélections effectués
		 * Les déplacements horizontaux du menu servent aux catégories, les déplacement verticaux servent aux boutons de sous-catégorie
		 * @ticks  Nombre de ticks actuel (si 0, initialise le compteur pour début, si -1, initalise pour fin)
		 * @return 0 s'il n'y a rien eu de spécifique, -1 si annulé, n° du bouton (en partant de 1) si un bouton a été validé, -([n°bouton]-1) en cas d'option avancée
		 */
		int update(int ticks);

		int makeTextes();

	private:
		SDL_Renderer* renderer; // Renderer sur lequel dessiner
		int positionX = 0; // position absolue du menu, en pixel
		int positionY = 0; // position absolue du menu, en pixel
		int tailleX = 0; // Taille absolue du menu en pixel
		int tailleY = 0; // Taille absolue du menu en pixel
		int tailleEcranX = 0; // Taille absolue de l'écran en pixel
		int tailleEcranY = 0; // Taille absolue de l'écran en pixel
		int tailleBouton = 0; // Taille d'un bouton (taille*taille), en pixels
		int selectionX = 0; // Position du pointeur de sélection (zone : écran complet)
		int selectionY = 0; // Position du pointeur de sélection (zone : écran complet)
		int deplacementBoutonsX = 0; // Position du pointeur de déplacement, pour les catégories (zone : écran complet)
		int deplacementBoutonsY = 0; // Position du pointeur de déplacement, pour les boutons (zone : écran complet)
		int boutonSelect = 0; // Bouton actuellement sélectionné
		int categorieSelect = 0; // Catégorie actuellement sélectionnée
		int nbBtParLigne = 0; // Nombre de boutons par lignes
		std::vector<int> nbBoutons; // Tableau indiquant pour CHAQUE catégorie, combien de boutons il y a
		std::vector<Bouton*> categories; // Ensemble des boutons de catégorie
		std::vector<Bouton*> boutons; // Ensemble des boutons de sous-catégorie (toutes catégories confondues, mais triés)
		ToucheJeu** touches; // Ensemble des toucheJeu de l'application
		int ticksAnimation = 0; // nbTicks depuis le début de l'animation
		int typeAnimation = 0; // 0 = aucune animation, 1 = ouverture, 2 = fermeture (opacité +/-)
		int result = 0; // Dernier résultat s'il y'en a un
		TTF_Font* policeMenu; // Police d'écriture sur le menu
		std::vector<SDL_Texture*> texteT; // Ensemble des lignes de texte pour le titre
		std::vector<SDL_Texture*> texteD; // Ensemble des lignes de texte pour la description
};

#endif
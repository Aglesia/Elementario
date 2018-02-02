#ifndef MENU_ANGULAIRE_H
#define MENU_ANGULAIRE_H

#include <Menu.h>

class MenuAngulaire : public Menu
{
	public:
		MenuAngulaire(ToucheJeu** touches, SDL_Renderer* renderer, TTF_Font* policeMenu);
		/**
		 * Ajoute une catégorie de boutons au menu
		 * @param boutons     Ensemble des boutons contenus dans la catégorie, le premier est le bouton représentant la catégorie, qui sera mis dans la barre des catégories, en haut
		 */
		void ajoutBoutons(std::vector<Bouton*> boutons);
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
		void reset();

		int makeTextes();

	private:
		int nbBoutons; // Tableau indiquant pour CHAQUE catégorie, combien de boutons il y a
		Bouton* categorie; // Bouton représentant la catégorie, placé au centre
};

#endif
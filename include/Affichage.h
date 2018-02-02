#ifndef AFFICHAGE_H
#define AFFICHAGE_H

#include <Bouton.h>
#include <Manette.h>
#include <Touche.h>
#include <ToucheJeu.h>
#include <Menu.h>
#include <MenuAngulaire.h>
#include <defs.h>
#include <Config.h>

class Affichage
{
	public:
		Affichage(std::string name, std::string path, ToucheJeu** touchesJeu, Config* param);
		virtual ~Affichage();
		void modePleinEcran(bool pleinEcran = true, int* x = nullptr, int* y = nullptr);
		void update();
		// Initialise la fenêtre : charge toutes les surfaces nécessaires
		int init();
		// à appeler dans le main thread, une fois l'init fait : crée les textures depuis les surfaces
		int init_main();
		void getScreenSize(int* x, int* y);
		// Retourne le nombre de ticks depuis le lancement du jeu
		// Si un nombre de ticks est indiqué, retourne le nombre de ticks écoulés depuis la référence donnée
		unsigned int nbTicks(unsigned int t = 0);
		/**
		 * Retourne le bouton pointé par son numéro (son emplacement dans "boutons")
		 * @param  noBouton numéro du bouton
		 * @return          Bouton s'il y en a un, nullptr sinon
		 */
		Bouton* getBouton(int noBouton);
		/**
		 * Change l'écran à afficher dans le jeu
		 * @param ecran nouvel écran à afficher
		 */
		void setEcran(int ecran);
		/**
		 * Certains écrans créent un menu. Il est récupérable ici
		 */
		Menu* getMenu(int menu);
		/**
		 * Affiche un écran de chargement avec une barre de progression
		 * Lorsqu'un pourcentage est spécifié, actualise les valeurs
		 */
		void afficherEcranChargement(int pourcentage = -1, std::string message = "");
		/**
		 * Affiche toutes les touchesJeu possibles, en les catégorisant.
		 * Les catégories sont en haut en ligne, les touches sont en colonne sous chaque catégorie
		 * @param categorieSelectionnee Colonne actuellement sélectionnée, les autres sont transparents à x%
		 * @param noToucheJeu           Touche actuellement sélectionné, plus gros que les autres boutons
		 * @param xReference            Position X du tableau, de -255 à 255. -255 veut dire que le bouton le plus à gauche se trouve au millieu, 255 veut dire que le bouton le plus à droite est au millieu
		 * @param yReference            Position Y du tableau
		 * @param opacite               Opacité du tableau
		 */
		void afficherMenuConfigTouchesSelectionTouche(int categorieSelectionnee = -1, int noToucheJeu = -1, int xReference = -1, int yReference = -1, int opacite = 255);
		void afficherInfoConfigTouchesCategories(std::string nom = "", std::string string = "");
		static std::string intToString(int i);

	protected:

	private:
		Config* param; // Ensemble des paramètres du logiciel
		std::string path; // Chemin d'accès aux fichiers
		SDL_Window* pWindow = nullptr; // Fenêtre principale
		SDL_Renderer* renderer = nullptr; // Rendu de la fenêtre principale
		ToucheJeu** touchesJeu; // Ensemble des touches du jeu (pour les menuss)
		std::vector<Bouton*> boutons; // Ensemble des boutons

		// Ecran chargement
		SDL_Surface* fond_surface = nullptr; // Surface du fond de l'écran
		SDL_Surface* barreChargement_surface = nullptr; // Barre de chargement (surface)
		SDL_Surface* icone = nullptr; // Icône de l'appli
		SDL_Surface* pointeur_surface = nullptr; // Pointeur (surface)
		SDL_Texture* fond = nullptr; // Texture du fond de l'écran
		SDL_Texture* gifChargement = nullptr; // Texture de l'icône de l'application, qui tourne sur l'écran ds chargement
		SDL_Texture* barreChargement = nullptr; // Barre de chargement
		SDL_Texture* pointeur = nullptr; // Pointeur
		unsigned int pourcentage = 0; // Pourcentage actuel de chargement
		double rotation = 0; // Rotation actuelle de l'image de chargement
		std::string texteChargement = ""; // Texte actuel à afficher sur l'écran de chargement

		// Polices
		TTF_Font* policeChargement = nullptr; // Police d'écriture sur l'écran de chargement
		TTF_Font* policeTitreMenu = nullptr; // Police d'écriture des titres sur les écrans de menu
		TTF_Font* policeMenu = nullptr; // Police d'écriture sur les écrans de menu

		// Ecran config touches 1
		int configTouches1Categorie = 0; // Ligne (catégorie) actuelle
		int configTouches1CategorieD = 0; // Ligne (catégorie) à atteindre
		int configTouchesNbTicksAnimation = 0; // Nombre de ticks actuels pour l'animation
		std::string configTouchesNom = ""; // Nom du bouton actuellement sélectionné
		std::string configTouchesDescription = ""; // Description du bouton actuellement sélectionné

		// Autre
		unsigned int nbTicksDebutTransition = 0; // Référence de début de la transition
		unsigned int dernierTickChargement = 0; // Dernière fois que nous avons actualisé les ticks sur l'écran de chargement
		Uint32 dernierTempsTick = 0; // Référence de temps pour calculer le nombre de ticks (temps du jeu)
		unsigned int nbT = 0; // Nombre de ticks depuis le lancement du programme
		std::mutex lock; // Mutex de gestion d'accès à l'objet
		std::mutex lockAff; // Mutex de gestion d'accès à l'objet "modeAffichage"
		std::mutex lockTick; // Mutex de gestion d'accès à l'objet "nbT"
		Menu** menus = nullptr; // Ensemble des menus possibles, ils seront créés à l'init
		int ecran = 0; // Ecran actuel

		bool pleinEcran = false; // Indique si la fenêtre est en plein écran (fenêtré sans bordure)
		int tailleRef = 1; // Indique la taille de référence pour l'affichage
		int ancienneTailleX = 0; // Ancienne taille enregistrée pour la fenêtre
		int ancienneTailleY = 0; // Ancienne taille enregistrée pour la fenêtre

		std::string titres_icones_menu_principale[NB_ICONES_MENU_PRINCIPALE] = {"MAPPER LES BOUTONS","JOUER","CONFIGURATION","QUITTER"};
		std::string description_icones_menu_principale[NB_ICONES_MENU_PRINCIPALE] = {"Configurer les différents boutons, les entrées utilisateur","Lancer une partie en ligne","Configurer les différents paramètres du jeu","Quitter le jeu"};
};

#endif // AFFICHAGE_H
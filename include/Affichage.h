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
		// Initialise la fen�tre : charge toutes les surfaces n�cessaires
		int init();
		// � appeler dans le main thread, une fois l'init fait : cr�e les textures depuis les surfaces
		int init_main();
		void getScreenSize(int* x, int* y);
		// Retourne le nombre de ticks depuis le lancement du jeu
		// Si un nombre de ticks est indiqu�, retourne le nombre de ticks �coul�s depuis la r�f�rence donn�e
		unsigned int nbTicks(unsigned int t = 0);
		/**
		 * Retourne le bouton point� par son num�ro (son emplacement dans "boutons")
		 * @param  noBouton num�ro du bouton
		 * @return          Bouton s'il y en a un, nullptr sinon
		 */
		Bouton* getBouton(int noBouton);
		/**
		 * Change l'�cran � afficher dans le jeu
		 * @param ecran nouvel �cran � afficher
		 */
		void setEcran(int ecran);
		/**
		 * Certains �crans cr�ent un menu. Il est r�cup�rable ici
		 */
		Menu* getMenu(int menu);
		/**
		 * Affiche un �cran de chargement avec une barre de progression
		 * Lorsqu'un pourcentage est sp�cifi�, actualise les valeurs
		 */
		void afficherEcranChargement(int pourcentage = -1, std::string message = "");
		/**
		 * Affiche toutes les touchesJeu possibles, en les cat�gorisant.
		 * Les cat�gories sont en haut en ligne, les touches sont en colonne sous chaque cat�gorie
		 * @param categorieSelectionnee Colonne actuellement s�lectionn�e, les autres sont transparents � x%
		 * @param noToucheJeu           Touche actuellement s�lectionn�, plus gros que les autres boutons
		 * @param xReference            Position X du tableau, de -255 � 255. -255 veut dire que le bouton le plus � gauche se trouve au millieu, 255 veut dire que le bouton le plus � droite est au millieu
		 * @param yReference            Position Y du tableau
		 * @param opacite               Opacit� du tableau
		 */
		void afficherMenuConfigTouchesSelectionTouche(int categorieSelectionnee = -1, int noToucheJeu = -1, int xReference = -1, int yReference = -1, int opacite = 255);
		void afficherInfoConfigTouchesCategories(std::string nom = "", std::string string = "");
		static std::string intToString(int i);

	protected:

	private:
		Config* param; // Ensemble des param�tres du logiciel
		std::string path; // Chemin d'acc�s aux fichiers
		SDL_Window* pWindow = nullptr; // Fen�tre principale
		SDL_Renderer* renderer = nullptr; // Rendu de la fen�tre principale
		ToucheJeu** touchesJeu; // Ensemble des touches du jeu (pour les menuss)
		std::vector<Bouton*> boutons; // Ensemble des boutons

		// Ecran chargement
		SDL_Surface* fond_surface = nullptr; // Surface du fond de l'�cran
		SDL_Surface* barreChargement_surface = nullptr; // Barre de chargement (surface)
		SDL_Surface* icone = nullptr; // Ic�ne de l'appli
		SDL_Surface* pointeur_surface = nullptr; // Pointeur (surface)
		SDL_Texture* fond = nullptr; // Texture du fond de l'�cran
		SDL_Texture* gifChargement = nullptr; // Texture de l'ic�ne de l'application, qui tourne sur l'�cran ds chargement
		SDL_Texture* barreChargement = nullptr; // Barre de chargement
		SDL_Texture* pointeur = nullptr; // Pointeur
		unsigned int pourcentage = 0; // Pourcentage actuel de chargement
		double rotation = 0; // Rotation actuelle de l'image de chargement
		std::string texteChargement = ""; // Texte actuel � afficher sur l'�cran de chargement

		// Polices
		TTF_Font* policeChargement = nullptr; // Police d'�criture sur l'�cran de chargement
		TTF_Font* policeTitreMenu = nullptr; // Police d'�criture des titres sur les �crans de menu
		TTF_Font* policeMenu = nullptr; // Police d'�criture sur les �crans de menu

		// Ecran config touches 1
		int configTouches1Categorie = 0; // Ligne (cat�gorie) actuelle
		int configTouches1CategorieD = 0; // Ligne (cat�gorie) � atteindre
		int configTouchesNbTicksAnimation = 0; // Nombre de ticks actuels pour l'animation
		std::string configTouchesNom = ""; // Nom du bouton actuellement s�lectionn�
		std::string configTouchesDescription = ""; // Description du bouton actuellement s�lectionn�

		// Autre
		unsigned int nbTicksDebutTransition = 0; // R�f�rence de d�but de la transition
		unsigned int dernierTickChargement = 0; // Derni�re fois que nous avons actualis� les ticks sur l'�cran de chargement
		Uint32 dernierTempsTick = 0; // R�f�rence de temps pour calculer le nombre de ticks (temps du jeu)
		unsigned int nbT = 0; // Nombre de ticks depuis le lancement du programme
		std::mutex lock; // Mutex de gestion d'acc�s � l'objet
		std::mutex lockAff; // Mutex de gestion d'acc�s � l'objet "modeAffichage"
		std::mutex lockTick; // Mutex de gestion d'acc�s � l'objet "nbT"
		Menu** menus = nullptr; // Ensemble des menus possibles, ils seront cr��s � l'init
		int ecran = 0; // Ecran actuel

		bool pleinEcran = false; // Indique si la fen�tre est en plein �cran (fen�tr� sans bordure)
		int tailleRef = 1; // Indique la taille de r�f�rence pour l'affichage
		int ancienneTailleX = 0; // Ancienne taille enregistr�e pour la fen�tre
		int ancienneTailleY = 0; // Ancienne taille enregistr�e pour la fen�tre

		std::string titres_icones_menu_principale[NB_ICONES_MENU_PRINCIPALE] = {"MAPPER LES BOUTONS","JOUER","CONFIGURATION","QUITTER"};
		std::string description_icones_menu_principale[NB_ICONES_MENU_PRINCIPALE] = {"Configurer les diff�rents boutons, les entr�es utilisateur","Lancer une partie en ligne","Configurer les diff�rents param�tres du jeu","Quitter le jeu"};
};

#endif // AFFICHAGE_H
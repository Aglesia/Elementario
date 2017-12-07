#ifndef AFFICHAGE_H
#define AFFICHAGE_H

#include <Bouton.h>
#include <Manette.h>
#include <Touche.h>
#include <defs.h>

class Affichage
{
	public:
		Affichage(std::string name, std::string path);
		virtual ~Affichage();
		void modePleinEcran(bool pleinEcran = true, int* x = nullptr, int* y = nullptr);
		void update();
		// Initialise la fenêtre : charge toutes les textures nécessaires
		int init();
		// Retourne le nombre de ticks depuis le lancement du jeu
		// Si un nombre de ticks est indiqué, retourne le nombre de ticks écoulés depuis la référence donnée
		unsigned int nbTicks(unsigned int t = 0);
		void setEcran(int ecran);
		/**
		 * Affiche un écran de chargement avec une barre de progression
		 * Lorsqu'un pourcentage est spécifié, actualise les valeurs
		 */
		void afficherEcranChargement(int pourcentage = -1, std::string message = "");
		void afficherMenuConfigTouches1(int categorie = -1, int xReference = -1, int opacite = 255);
		void afficherMenuConfigTouches2(int noToucheJeu = -1, int xReference = -1, int opacite = 255);
		void afficherMenuConfigTouches3(int nbController = -1, int noController = -3, int xReference = -1, int opacite = 255);
		void afficherMenuConfigTouches4(std::vector<Touche*>* t = nullptr, int noTouche = -1, int xReference = -1, int opacite = 255);
		void afficherInfoConfigurationTouches1(std::string nom = "", std::string string = "");
		void afficherInfoConfigurationTouches2(Touche* t = nullptr);
		static std::string intToString(int i);

	protected:

	private:
		std::string path; // Chemin d'accès aux fichiers
		SDL_Window* pWindow = nullptr; // Fenêtre principale
		SDL_Renderer* renderer = nullptr; // Rendu de la fenêtre principale

		// Ecran chargement
		SDL_Surface* icone = nullptr; // Icône de l'appli
		SDL_Texture* fond = nullptr; // Texture du fond de l'écran
		SDL_Texture* gifChargement = nullptr; // Texture de l'icône de l'application, qui tourne sur l'écran ds chargement
		SDL_Texture* barreChargement = nullptr; // Barre de chargement
		unsigned int pourcentage = 0; // Pourcentage actuel de chargement
		double rotation = 0; // Rotation actuelle de l'image de chargement
		std::string texteChargement = ""; // Texte actuel à afficher sur l'écran de chargement

		// Polices
		TTF_Font* policeChargement = nullptr; // Police d'écriture sur l'écran de chargement
		TTF_Font* policeTitreMenu = nullptr; // Police d'écriture des titres sur les écrans de menu
		TTF_Font* policeMenu = nullptr; // Police d'écriture sur les écrans de menu

		// Ecran config touches 1
		std::vector<Bouton*> configTouches1Boutons; // Ensemble des boutons de catégorie
		int configTouches1Categorie = 0; // Ligne (catégorie) actuelle
		int configTouches1CategorieD = 0; // Ligne (catégorie) à atteindre
		int configTouchesNbTicksAnimation = 0; // Nombre de ticks actuels pour l'animation
		std::string configTouchesNom = ""; // Nom du bouton actuellement sélectionné
		std::string configTouchesDescription = ""; // Description du bouton actuellement sélectionné

		// Ecran config touches 2
		std::vector<Bouton*> configTouches2Boutons; // Ensemble des boutons jeu
		int configTouches2noToucheJeu = 0; // Ligne (noToucheJeu) actuelle
		int configTouches2noToucheJeuD = 0; // Ligne (noToucheJeu) à atteindre

		// Ecran config touches 3
		std::vector<Bouton*> configTouches3Boutons; // Ensemble des boutons manette
		int configTouches3nbController = 0; // Nombre de manettes
		int configTouches3noController = 0; // Ligne (noToucheJeu) actuelle
		int configTouches3noControllerD = 0; // Ligne (noToucheJeu) à atteindre

		// Ecran config touches 4
		std::vector<Bouton*> configTouches4Boutons; // Ensemble des boutons jeu
		SDL_Texture* configTouches4TextureBouton = nullptr; // Texture du bouton
		SDL_Texture* configTouches4TextureAxe = nullptr; // Texture de l'axe
		SDL_Texture* configTouches4TextureCroix = nullptr; // Texture de la croix directionnelle
		SDL_Texture* configTouches4TextureMolette = nullptr; // Texture de la molette
		SDL_Texture* configTouches4TextureNew = nullptr; // Texture de la molette
		int configTouches4noTouche = 0; // Ligne (noToucheJeu) actuelle
		int configTouches4noToucheD = 0; // Ligne (noToucheJeu) à atteindre
		std::vector<Touche*>* configTouches4noController = nullptr; // Ancien contrôleur sélectionné
		Touche* configTouchesManette = nullptr; // Description du bouton actuellement sélectionné

		// Autre
		unsigned int nbTicksDebutTransition = 0; // Référence de début de la transition
		unsigned int dernierTickChargement = 0; // Dernière fois que nous avons actualisé les ticks sur l'écran de chargement
		Uint32 dernierTempsTick = 0; // Référence de temps pour calculer le nombre de ticks (temps du jeu)
		unsigned int nbT = 0; // Nombre de ticks depuis le lancement du programme
		std::mutex lock; // Mutex de gestion d'accès à l'objet
		std::mutex lockAff; // Mutex de gestion d'accès à l'objet "modeAffichage"
		std::mutex lockTick; // Mutex de gestion d'accès à l'objet "nbT"

		int modeAffichage = 0; // Indique quel type d'écran on affiche (menu, chargement...)
		bool pleinEcran = false; // Indique si la fenêtre est en plein écran (fenêtré sans bordure)
};

#endif // AFFICHAGE_H
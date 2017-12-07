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
		// Initialise la fen�tre : charge toutes les textures n�cessaires
		int init();
		// Retourne le nombre de ticks depuis le lancement du jeu
		// Si un nombre de ticks est indiqu�, retourne le nombre de ticks �coul�s depuis la r�f�rence donn�e
		unsigned int nbTicks(unsigned int t = 0);
		void setEcran(int ecran);
		/**
		 * Affiche un �cran de chargement avec une barre de progression
		 * Lorsqu'un pourcentage est sp�cifi�, actualise les valeurs
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
		std::string path; // Chemin d'acc�s aux fichiers
		SDL_Window* pWindow = nullptr; // Fen�tre principale
		SDL_Renderer* renderer = nullptr; // Rendu de la fen�tre principale

		// Ecran chargement
		SDL_Surface* icone = nullptr; // Ic�ne de l'appli
		SDL_Texture* fond = nullptr; // Texture du fond de l'�cran
		SDL_Texture* gifChargement = nullptr; // Texture de l'ic�ne de l'application, qui tourne sur l'�cran ds chargement
		SDL_Texture* barreChargement = nullptr; // Barre de chargement
		unsigned int pourcentage = 0; // Pourcentage actuel de chargement
		double rotation = 0; // Rotation actuelle de l'image de chargement
		std::string texteChargement = ""; // Texte actuel � afficher sur l'�cran de chargement

		// Polices
		TTF_Font* policeChargement = nullptr; // Police d'�criture sur l'�cran de chargement
		TTF_Font* policeTitreMenu = nullptr; // Police d'�criture des titres sur les �crans de menu
		TTF_Font* policeMenu = nullptr; // Police d'�criture sur les �crans de menu

		// Ecran config touches 1
		std::vector<Bouton*> configTouches1Boutons; // Ensemble des boutons de cat�gorie
		int configTouches1Categorie = 0; // Ligne (cat�gorie) actuelle
		int configTouches1CategorieD = 0; // Ligne (cat�gorie) � atteindre
		int configTouchesNbTicksAnimation = 0; // Nombre de ticks actuels pour l'animation
		std::string configTouchesNom = ""; // Nom du bouton actuellement s�lectionn�
		std::string configTouchesDescription = ""; // Description du bouton actuellement s�lectionn�

		// Ecran config touches 2
		std::vector<Bouton*> configTouches2Boutons; // Ensemble des boutons jeu
		int configTouches2noToucheJeu = 0; // Ligne (noToucheJeu) actuelle
		int configTouches2noToucheJeuD = 0; // Ligne (noToucheJeu) � atteindre

		// Ecran config touches 3
		std::vector<Bouton*> configTouches3Boutons; // Ensemble des boutons manette
		int configTouches3nbController = 0; // Nombre de manettes
		int configTouches3noController = 0; // Ligne (noToucheJeu) actuelle
		int configTouches3noControllerD = 0; // Ligne (noToucheJeu) � atteindre

		// Ecran config touches 4
		std::vector<Bouton*> configTouches4Boutons; // Ensemble des boutons jeu
		SDL_Texture* configTouches4TextureBouton = nullptr; // Texture du bouton
		SDL_Texture* configTouches4TextureAxe = nullptr; // Texture de l'axe
		SDL_Texture* configTouches4TextureCroix = nullptr; // Texture de la croix directionnelle
		SDL_Texture* configTouches4TextureMolette = nullptr; // Texture de la molette
		SDL_Texture* configTouches4TextureNew = nullptr; // Texture de la molette
		int configTouches4noTouche = 0; // Ligne (noToucheJeu) actuelle
		int configTouches4noToucheD = 0; // Ligne (noToucheJeu) � atteindre
		std::vector<Touche*>* configTouches4noController = nullptr; // Ancien contr�leur s�lectionn�
		Touche* configTouchesManette = nullptr; // Description du bouton actuellement s�lectionn�

		// Autre
		unsigned int nbTicksDebutTransition = 0; // R�f�rence de d�but de la transition
		unsigned int dernierTickChargement = 0; // Derni�re fois que nous avons actualis� les ticks sur l'�cran de chargement
		Uint32 dernierTempsTick = 0; // R�f�rence de temps pour calculer le nombre de ticks (temps du jeu)
		unsigned int nbT = 0; // Nombre de ticks depuis le lancement du programme
		std::mutex lock; // Mutex de gestion d'acc�s � l'objet
		std::mutex lockAff; // Mutex de gestion d'acc�s � l'objet "modeAffichage"
		std::mutex lockTick; // Mutex de gestion d'acc�s � l'objet "nbT"

		int modeAffichage = 0; // Indique quel type d'�cran on affiche (menu, chargement...)
		bool pleinEcran = false; // Indique si la fen�tre est en plein �cran (fen�tr� sans bordure)
};

#endif // AFFICHAGE_H
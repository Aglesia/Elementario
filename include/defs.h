#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <sstream>

// SDL
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	#define rmask 0xff000000
	#define gmask 0x00ff0000
	#define bmask 0x0000ff00
	#define amask 0x000000ff
#else
	#define rmask 0x000000ff
	#define gmask 0x0000ff00
	#define bmask 0x00ff0000
	#define amask 0xff000000
#endif

// int_to_string
#define SSTR( x ) (std::string)"OK"//static_cast<std::ostringstream &>((std::ostringstream() << std::dec << x )).str()

// Fenêtres
#define TAILLE_FENETRE_X 800
#define TAILLE_FENETRE_Y 600
#define IMG_INIT_FORMAT IMG_INIT_JPG
#define TRANSPARENCE_HEXAGONE_CHARGEMENT 10

// Etat main
#define ETAT_ERREUR 0
#define ETAT_QUITTER 1
#define ETAT_CHARGEMENT_JEU 2
#define ETAT_CONFIG_TOUCHES_1 3
#define ETAT_CONFIG_TOUCHES_2 4
#define ETAT_CONFIG_TOUCHES_3 5
#define ETAT_CONFIG_TOUCHES_4 6
#define ETAT_CONFIG_TOUCHES_5 7

// Icônes
#define TAILLE_ICONE_CHARGEMENT 256
#define TAILLE_ICONE_CONFIG_TOUCHE 128
#define TAILLE_TEXTE_BOUTON 12 // Taille du titre des boutons
#define NIVEAU_ZOOM_BOUTON_ACTIF 0.6 // multiplication de la taille du bouton sélectionné (%)
#define NB_TICKS_ANIMATION_BOUTON 5 // Nombre de ticks entre le début et la fin de l'animation de (dé)zoom du bouton
#define NB_TICKS_DEPLACEMENT_BOUTON 7 // Nombre de ticks entre le début et la fin de l'animation de déplacement
#define NB_TICKS_DEPLACEMENT_ECRAN 15 // Nombre de ticks entre le début et la fin de la transition entre 2 écrans

// Fichiers
#define ICONE_FILENAME "data/icon.png"
#define IMAGE_FOND_FILENAME "data/image/fond.png"
#define POLICE_CHARGEMENT_FILENAME "data/font/chargement.ttf"
#define POLICE_MENU_FILENAME "data/font/chargement.ttf"
#define BOUTON_SOURIS_FILENAME "data/image/button/config/mouse.png"
#define BOUTON_CLAVIER_FILENAME "data/image/button/config/keyboard.png"
#define BOUTON_MANETTE_FILENAME "data/image/button/config/controller.png"
#define BOUTON_CONFIG_DIRNAME "data/image/button/config/"
#define POLICE_CHARGEMENT_TAILLE 24
#define POLICE_MENU_TAILLE 18
#define POLICE_MENU_TITRE_TAILLE 24

// Couleurs
#define COULEUR_FOND_CHARGEMENT_R 65
#define COULEUR_FOND_CHARGEMENT_G 60
#define COULEUR_FOND_CHARGEMENT_B 60
#define COULEUR_BARRE_CHARGEMENT_R 0
#define COULEUR_BARRE_CHARGEMENT_G 150
#define COULEUR_BARRE_CHARGEMENT_B 255
#define COULEUR_HEXAGONE_CHARGEMENT_R 200
#define COULEUR_HEXAGONE_CHARGEMENT_G 90
#define COULEUR_HEXAGONE_CHARGEMENT_B 0
#define COULEUR_TEXTE_CHARGEMENT_R 0
#define COULEUR_TEXTE_CHARGEMENT_G 0
#define COULEUR_TEXTE_CHARGEMENT_B 0
#define COULEUR_TEXTE_MENU_R 255
#define COULEUR_TEXTE_MENU_G 255
#define COULEUR_TEXTE_MENU_B 255
#define EPAISSEUR_HEXAGONE_CHARGEMENT 8
#define OPACITE_HEXAGONE_CHARGEMENT 64

// Type de touche
#define TYPE_TOUCHE_INCONNU 0
#define TYPE_TOUCHE_MOLETTE 1
#define TYPE_TOUCHE_BOUTON 2
#define TYPE_TOUCHE_JOYSTIC 3
#define TYPE_TOUCHE_DIRECTION 4

#define NB_TOUCHES_SOURIS 5 // Nombre de touches allouées pour la souris
#define PRECISION_VALEUR_AXE_TOUCHE 1024

// Ecran de configuration des touches
#define CONFIG_TOUCHES_NB_CATEGORIES 4
#define CONFIG_TOUCHES_NB_TOUCHE_JEU 10
#define CONFIG_OPACITE_FILTRE_NOIR 150
#define POSITION_TITRE_Y 10

// Touches
#define NB_TOUCHES_JEU 10
#define TOUCHE_HAUT_BAS 0
#define TOUCHE_GAUCHE_DROITE 1
#define AXE_HAUT_BAS_SELECTION 2
#define AXE_GAUCHE_DROITE_SELECTION 3
#define AXE_HAUT_BAS_DEPLACEMENT 4
#define AXE_GAUCHE_DROITE_DEPLACEMENT 5
#define TOUCHE_ENTRER 6
#define TOUCHE_ANNULER 7
#define TOUCHE_QUITTER 8
#define TOUCHE_PLEIN_ECRAN 9

// Jeu
#define TEMPS_TICKS 16 // Nombre de ms entre chaque tick, ici Approx. 60t/sec


// Affichage en cours
#define MODE_AFFICHAGE_CHARGEMENT 0
#define MODE_AFFICHAGE_CONFIG_TOUCHES1 1
#define MODE_AFFICHAGE_CONFIG_TOUCHES2 2
#define MODE_AFFICHAGE_CONFIG_TOUCHES3 3
#define MODE_AFFICHAGE_CONFIG_TOUCHES4 4
#define MODE_AFFICHAGE_CONFIG_TOUCHES5 5
#define MODE_TRANSITION_CONFIG_TOUCHES12 6
#define MODE_TRANSITION_CONFIG_TOUCHES21 7
#define MODE_TRANSITION_CONFIG_TOUCHES23 8
#define MODE_TRANSITION_CONFIG_TOUCHES32 9
#define MODE_TRANSITION_CONFIG_TOUCHES34 10
#define MODE_TRANSITION_CONFIG_TOUCHES43 11


#endif // DEFINITIONS_H

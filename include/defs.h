#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <thread>
#include <mutex>
#include <vector>
#include <ctime>

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

#define LOG_LEVEL SDL_LOG_PRIORITY_DEBUG

// Fenêtres
#define TAILLE_FENETRE_X 1280
#define TAILLE_FENETRE_Y 720
#define IMG_INIT_FORMAT IMG_INIT_PNG
#define IMAGE_FORMAT "png"
#define TRANSPARENCE_HEXAGONE_CHARGEMENT 10

// Etat main
#define ETAT_ERREUR 0
#define ETAT_QUITTER 1
#define ETAT_CHARGEMENT_JEU 2
#define ETAT_CONFIG_TOUCHES_CATEGORIES 3

// Icônes
#define TAILLE_ICONE_CHARGEMENT 256
#define TAILLE_ICONE_CONFIG_TOUCHE 128 // TODO : rendre relatif à la taille de référence (variable dans Affichage)
// #define TAILLE_TEXTE_BOUTON 12 // Taille du titre des boutons
#define NIVEAU_ZOOM_BOUTON_ACTIF 0.6 // multiplication de la taille du bouton sélectionné (%)
#define NB_TICKS_ANIMATION_BOUTON 5 // Nombre de ticks entre le début et la fin de l'animation de (dé)zoom du bouton
#define NB_TICKS_DEPLACEMENT_BOUTON 7 // Nombre de ticks entre le début et la fin de l'animation de déplacement
#define NB_TICKS_DEPLACEMENT_ECRAN 15 // Nombre de ticks entre le début et la fin de la transition entre 2 écrans

// Fichiers
#define ICONE_FILENAME "data/image/icon.png"
#define IMAGE_FOND_FILENAME "data/image/fond.png"
#define POLICE_CHARGEMENT_FILENAME "data/font/chargement.ttf"
#define POLICE_MENU_FILENAME "data/font/chargement.ttf"
#define BOUTON_SOURIS_FILENAME "data/image/button/config/mouse.png"
#define BOUTON_CLAVIER_FILENAME "data/image/button/config/keyboard.png"
#define BOUTON_MANETTE_FILENAME "data/image/button/config/controller.png"
#define BOUTON_CONFIG_DIRNAME "data/image/button/config/"
#define POLICE_CHARGEMENT_TAILLE 24 // TODO : rendre relatif à la taille de référence (variable dans Affichage)
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

// Bundles
#define NB_BUNDLE 4
#define BUNDLE_PERSISTANT 0
#define BUNDLE_NAVIGATION 1
#define BUNDLE_ANGULAIRE 2
#define BUNDLE_PLATEAU 3

// Touches
#define NB_TOUCHES_JEU 24 // DOIVENT RESTER TRIES
#define TOUCHE_PLEIN_ECRAN 0 // DOIT ETRE LE PREMIER DE LA CATEGORIE !!!
#define TOUCHE_AFFICHAGE_FPS 1
#define TOUCHE_NAVIGATION_DEPLACER_AXE_HAUT_BAS 2 // DOIT ETRE LE PREMIER DE LA CATEGORIE !!!
#define TOUCHE_NAVIGATION_DEPLACER_AXE_GAUCHE_DROITE 3
#define TOUCHE_NAVIGATION_SELECTION_BOUTON_HAUT_BAS 4
#define TOUCHE_NAVIGATION_SELECTION_BOUTON_GAUCHE_DROITE 5
#define TOUCHE_NAVIGATION_SELECTION_AXE_HAUT_BAS 6
#define TOUCHE_NAVIGATION_SELECTION_AXE_GAUCHE_DROITE 7
#define TOUCHE_NAVIGATION_VALIDER 8
#define TOUCHE_NAVIGATION_RETOUR 9
#define TOUCHE_NAVIGATION_AVANCE 10
#define TOUCHE_ANGULAIRE_BOUTON_ANGLE 11 // DOIT ETRE LE PREMIER DE LA CATEGORIE !!!
#define TOUCHE_ANGULAIRE_AXE_ANGLE 12
#define TOUCHE_ANGULAIRE_VALIDER 12
#define TOUCHE_ANGULAIRE_RETOUR 13
#define TOUCHE_ANGULAIRE_AVANCE 14
#define TOUCHE_PLATEAU_MENU 15 // DOIT ETRE LE PREMIER DE LA CATEGORIE !!!
#define TOUCHE_PLATEAU_DEPLACER_X 16
#define TOUCHE_PLATEAU_DEPLACER_Y 17
#define TOUCHE_PLATEAU_SELECTION_BOUTON_HAUT_BAS 18
#define TOUCHE_PLATEAU_SELECTION_BOUTON_GAUCHE_DROITE 19
#define TOUCHE_PLATEAU_SELECTION_AXE_HAUT_BAS 20
#define TOUCHE_PLATEAU_SELECTION_AXE_GAUCHE_DROITE 21
#define TOUCHE_PLATEAU_VALIDER_CASE 22
#define TOUCHE_PLATEAU_ZOOM 23

// Modes de liaisons des touches
#define MODE_INCONNU 0
#define MODE_APPUIE_UNIQUE 1 // Enregistre les appuies, mais pas les relâchements
#define MODE_AXE_RELATIF 2 // Ajoute ou soustrait à la valeur actuelle (ex : la vitesse de déplacement correspond à la valeur du joystic)
#define MODE_AXE_ABSOLUE 3 // Remplace tout simplement la valeur actuelle (ex : la position du plateau correspond à la valeur du joystic)

// Jeu
#define TEMPS_TICKS 16 // Nombre de ms entre chaque tick, ici Approx. 60t/sec

// Gestion des menus
#define NB_MENUS 1
#define MENU_CONFIG_TOUCHES_CHOIX_TOUCHE 0

#define MENU_COULEUR_BORDURE_R 92
#define MENU_COULEUR_BORDURE_G 92
#define MENU_COULEUR_BORDURE_B 92
#define MENU_COULEUR_FOND_R 0
#define MENU_COULEUR_FOND_G 0
#define MENU_COULEUR_FOND_B 0
#define MENU_OPACITE_FOND 128
#define MENU_TAILLE_BANDE_TEXTE 1.2 // taille*tailleBouton
#define MENU_OPACITE_BANDE_TEXTE 240
#define MENU_COULEUR_TEXTE_R 255
#define MENU_COULEUR_TEXTE_G 255
#define MENU_COULEUR_TEXTE_B 255

#define MENU_CATEGORIE_DEPLACEMENT_MINIMUM 32 // Nombre de pixels minimum de déplacement des icônes de catégorie

// Affichage en cours
#define MODE_AFFICHAGE_CHARGEMENT 0
#define MODE_AFFICHAGE_CONFIG_TOUCHES_CATEGORIES 1
#define MODE_AFFICHAGE_CONFIG_TOUCHES_TOUCHE_JEU 2
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
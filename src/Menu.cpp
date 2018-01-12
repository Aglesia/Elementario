#include <Menu.h>

Menu::Menu(ToucheJeu** touches)
{
	this->nbBoutons.clear();
	this->categories.clear();
	this->boutons.clear();
	this->touches = touches;
	this->typeAnimation = 1;
}

Menu::~Menu()
{
	this->nbBoutons.clear();
	this->categories.clear();
	this->boutons.clear();
}

/**
 * Définit la taille d'affichage du menu
 * @param affichageX Taille (en pixels) de la zone d'affichage intérieur
 * @param affichageY Taille (en pixels) de la zone d'affichage intérieur
 */
void Menu::setTailleAffichage(int affichageX, int affichageY, int tailleBoutons)
{
	this->tailleX = affichageX;
	this->tailleY = affichageY;
	this->tailleBouton = tailleBoutons;
	this->nbBtParLigne = (affichageX/tailleBoutons)-1;
	if(this->nbBtParLigne<1)
		this->nbBtParLigne = 1;
}

/**
 * Position absolue du menu, en pixels. 0.0 = haut.gauche
 * @param affichageX Position absolue du point haut gauche de la zone
 * @param affichageY Position absolue du point haut gauche de la zone
 */
void Menu::setPositionAffichage(int affichageX, int affichageY)
{
	this->positionX = affichageX;
	this->positionY = affichageY;
}

/**
 * Ajoute une catégorie de boutons au menu
 * @param boutons     Ensemble des boutons contenus dans la catégorie, le premier est le bouton représentant la catégorie, qui sera mis dans la barre des catégories, en haut
 */
void Menu::ajoutCategorie(std::vector<Bouton*> boutons)
{
	if(boutons.size()>1)
	{
		this->nbBoutons.push_back(boutons.size());
		this->categories.push_back(boutons[0]);
		for(int i=1; i<boutons.size(); i++)
			this->boutons.push_back(boutons[i]);
	}
}

/**
 * Met à jour les actions boutons, puis l'affichage selon les déplacements/sélections effectués
 * Les déplacements horizontaux du menu servent aux catégories, les déplacement verticaux servent aux boutons de sous-catégorie
 * @return 0 s'il n'y a rien eu de spécifique, -1 si annulé, n° du bouton (en partant de 1) si un bouton a été validé, -([n°bouton]-1) en cas d'option avancée
 */
int Menu::update(int ticks) // TODO
{
	// Si on est en animation, on la gère
	// On regarde les input et on les met à jour en conséquence
	// On calcul la nouvelle sélection, et les nouveaux emplacements
	// On affiche le nom et la description de la sélection



	int selectionX = 0; // Position du pointeur de sélection (zone : écran complet)
	int selectionY = 0; // Position du pointeur de sélection (zone : écran complet)
	int boutonSelect = 0; // Bouton actuellement sélectionné
	int categorieSelect = 0; // Catégorie actuellement sélectionnée
	int result = 0; // Dernier résultat s'il y'en a un
	int ticksAnimation = 0; // nbTicks depuis le début de l'animation
	int typeAnimation = 0; // 0 = aucune animation, 1 = ouverture, 2 = fermeture (opacité +/-)
}

/**
 * Retourne le dernier résultat de l'update
 * @return 0 s'il n'y a rien eu de spécifique, -1 si annulé, n° du bouton (en partant de 1) si un bouton a été validé, -([n°bouton]-1) en cas d'option avancée
 */
int Menu::getResult()
{
	return this->result;
}






//int Menu::update()
//{
	/*ToucheJeu* touche = t->var->touchesJeu[TOUCHE_NAVIGATION_SELECTION_BOUTON_HAUT_BAS];
	if(touche != nullptr)
	{
		if(touche->isEvent())
		{
			if(touche->getVal()>0)
			{
				if(t->var->configTouchesCategorie<CONFIG_TOUCHES_NB_CATEGORIES-1)
					t->var->configTouchesCategorie++;

			}
			else if(touche->getVal()<0)
			{
				if(t->var->configTouchesCategorie>0)
					t->var->configTouchesCategorie--;
			}
			modif = true;
		}

		if(modif)
		{
			modif = false;
			std::string titre;
			std::string description;
			t->aff->afficherMenuConfigTouchesCategories(t->var->configTouchesCategorie);
			t->aff->afficherInfoConfigTouchesCategories(titre, description);
		}
	}

	// Si on a validé
	touche = t->var->touchesJeu[TOUCHE_NAVIGATION_VALIDER];
	if(touche != nullptr)
		if(touche->getVal()>0 && touche->isEvent())
		{
			t->ret = 1; // Passage à la colonne suivante
			stop = true;
			t->aff->afficherMenuConfigTouchesToucheJeu(t->var->configTouchesTouche);
		}

	// Si on a demandé à quitter
	touche = t->var->touchesJeu[TOUCHE_NAVIGATION_RETOUR];
	if(touche != nullptr)
		if(touche->getVal()>0 && touche->isEvent())
		{
			t->ret = 2;
			stop = true;
		}*/ // TODO : Sauvegarde du threadConfigTouches
//}



/*void Affichage::afficherInfoConfig(std::string nom, std::string description)
{
	if(description != "")
	{
		this->lock.lock();
		this->configTouchesNom = nom;
		this->configTouchesDescription = description;
		this->lock.unlock();
	}
	else
	{
		// On assombrit le fond
		int x = 0;
		int y = 0;
		SDL_Rect posEcran;
		posEcran.x = 0;
		posEcran.y = 0;
		SDL_GetWindowSize(this->pWindow, &(posEcran.w), &(posEcran.h));
		if(posEcran.w >= 3)
			posEcran.w /= 3;
		SDL_Surface* fondS = SDL_CreateRGBSurface(0, 1, 1, 32, rmask, gmask, bmask, amask);
		SDL_FillRect(fondS, NULL, SDL_MapRGBA(fondS->format, 0, 0, 0, CONFIG_OPACITE_FILTRE_NOIR));
		SDL_Texture* fondSombre = SDL_CreateTextureFromSurface(this->renderer, fondS);
		SDL_FreeSurface(fondS);
		SDL_RenderCopy(this->renderer, fondSombre, NULL, &posEcran);
		SDL_DestroyTexture(fondSombre);

		// On crée le titre
		SDL_Color couleurTexte = {COULEUR_TEXTE_MENU_R, COULEUR_TEXTE_MENU_G, COULEUR_TEXTE_MENU_B};
		SDL_Surface* titreSurface = TTF_RenderUTF8_Solid(this->policeTitreMenu, this->configTouchesNom.c_str(), couleurTexte);
		if(titreSurface == nullptr)
			return;
		SDL_Texture* titre = SDL_CreateTextureFromSurface(this->renderer, titreSurface);
		SDL_FreeSurface(titreSurface);
		SDL_Rect posTitre;
		SDL_QueryTexture(titre, NULL, NULL, &(posTitre.w), &(posTitre.h));
		posTitre.x = (posEcran.w/2) - (posTitre.w/2);
		posTitre.y = POSITION_TITRE_Y;
		SDL_RenderCopy(this->renderer, titre, NULL, &posTitre);
		SDL_DestroyTexture(titre);

		// On crée les textes en tenant compte de la place
		Uint16 text_Width = posEcran.w - 10;
		std::vector<std::string> linesFirstPass;
		std::vector<std::string> linesSecondPass;
		std::vector<SDL_Surface*> lines;

		// On éclate le texte
		std::istringstream split(this->configTouchesDescription);
		for (std::string each; std::getline(split, each, '\n');
			linesFirstPass.push_back(each));

		int hauteurTexte = 0;
		// Pour chaque ligne
		for(unsigned int i=0; i<linesFirstPass.size(); i++){
			// On éclate la ligne
			linesSecondPass.clear();
			std::istringstream split2(linesFirstPass[i]);
			for (std::string each; std::getline(split2, each, ' ');
				linesSecondPass.push_back(each));

			// Pour chaque mot
			std::string mot;
			for(unsigned int j=0; j<linesSecondPass.size(); j++){
				// On regarde si c'est le premier mot de la ligne
				if(!mot.compare(""))
					mot = linesSecondPass[j];
				else{
					SDL_Surface* test = TTF_RenderUTF8_Solid(this->policeMenu, (std::string(mot+" "+linesSecondPass[j])).c_str(), couleurTexte);
					// On regarde si on peut faire entrer le mot dans la case
					if(test->w < text_Width)
						mot += " "+linesSecondPass[j];
					else{
						lines.push_back(TTF_RenderUTF8_Solid(this->policeMenu, mot.c_str(), couleurTexte));
						hauteurTexte = test->h + 8;
						mot = linesSecondPass[j];
					}
					SDL_FreeSurface(test);
				}
			}

			// S'il reste un mot dans le buffer, on l'écrit
			if(mot.compare("")){
				SDL_Surface* txt = TTF_RenderUTF8_Solid(this->policeMenu, mot.c_str(), couleurTexte);
				lines.push_back(txt);
				hauteurTexte = txt->h + 8;
			}
		}

		// On écrit chaque ligne
		SDL_Rect posTxt;
		posTxt.x = 5;
		posTxt.y = posTitre.y+posTitre.h;
		for(Uint8 i = 0; i < lines.size(); i++){
			posTxt.y += hauteurTexte;
			SDL_Texture* texture = SDL_CreateTextureFromSurface(this->renderer, lines[i]);
			SDL_FreeSurface(lines[i]);
			SDL_QueryTexture(texture, NULL, NULL, &(posTxt.w), &(posTxt.h));
			SDL_RenderCopy(this->renderer, texture, NULL, &posTxt);
			SDL_DestroyTexture(texture);
		}
	}
}*/
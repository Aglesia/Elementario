#include <Menu.h>
#include <ToucheJeu.h>
#include <defs.h>

Menu::Menu(ToucheJeu** touches, SDL_Renderer* renderer, TTF_Font* policeMenu)
{
	this->policeMenu = policeMenu;
	this->renderer = renderer;
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

void Menu::selectCategorie(int numCategorie)
{
	if(numCategorie>=0 && numCategorie<this->categories.size())
	{
		this->boutonSelect = 0;
		this->categorieSelect = numCategorie;
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
	int opacite = 255;

	// On regarde les input et on les met à jour en conséquence
	// On calcul la taille si elle sort des ilmites
	if(this->tailleBouton>this->tailleY/3)
		this->tailleBouton = tailleY/3;
	if(this->tailleBouton>this->tailleX)
		this->tailleBouton = tailleX;

	// On calcul la nouvelle sélection, et les nouveaux emplacements
	// Catégorie précédente
	if(this->touches[TOUCHE_NAVIGATION_SELECTION_BOUTON_CATEGORIE_PRECEDENTE]->getVal()>0)
	{
		this->touches[TOUCHE_NAVIGATION_SELECTION_BOUTON_CATEGORIE_PRECEDENTE]->setVal(0);
		// On change la catégorie
		this->categorieSelect--;
		if(this->categorieSelect<0)
			this->categorieSelect = 0;
		// On place la sélection du bouton à 0
		this->boutonSelect = 0;
	}
	// Catégorie suivante
	if(this->touches[TOUCHE_NAVIGATION_SELECTION_BOUTON_CATEGORIE_SUIVANTE]->getVal()>0)
	{
		this->touches[TOUCHE_NAVIGATION_SELECTION_BOUTON_CATEGORIE_SUIVANTE]->setVal(0);
		// On change la catégorie
		this->categorieSelect++;
		if(this->categorieSelect>=this->categories.size())
			this->categorieSelect = this->categories.size()-1;
		// On place la sélection du bouton à 0
		this->boutonSelect = 0;
	}
	// Bouton précédent
	if(this->touches[TOUCHE_NAVIGATION_SELECTION_BOUTON_GAUCHE_DROITE]->getVal()<0)
	{
		this->touches[TOUCHE_NAVIGATION_SELECTION_BOUTON_GAUCHE_DROITE]->setVal(0);
		// On change le bouton
		this->boutonSelect--;
		if(this->boutonSelect < 0)
			this->boutonSelect = 0;
	}
	// Bouton suivant
	if(this->touches[TOUCHE_NAVIGATION_SELECTION_BOUTON_GAUCHE_DROITE]->getVal()>0)
	{
		this->touches[TOUCHE_NAVIGATION_SELECTION_BOUTON_GAUCHE_DROITE]->setVal(0);
		// On change le bouton
		this->boutonSelect++;
		if(this->boutonSelect>=this->nbBoutons[this->categorieSelect])
			this->boutonSelect = this->nbBoutons[this->categorieSelect]-1;
	}

	// On dessine le menu vide
	SDL_Rect fond;
	fond.x = this->positionX;
	fond.y = this->positionY;
	fond.w = this->tailleX;
	fond.h = this->tailleY;
	SDL_Surface* fondS = SDL_CreateRGBSurface(0, 1, 1, 32, rmask, gmask, bmask, amask);
	SDL_FillRect(fondS, NULL, SDL_MapRGBA(fondS->format, MENU_COULEUR_FOND_R, MENU_COULEUR_FOND_G, MENU_COULEUR_FOND_B, MENU_OPACITE_FOND));
	SDL_Texture* fondSombre = SDL_CreateTextureFromSurface(this->renderer, fondS);
	SDL_FreeSurface(fondS);
	SDL_RenderCopy(this->renderer, fondSombre, NULL, &fond);
	SDL_DestroyTexture(fondSombre);

	// bords du menu // TODO : images, motifs qui se répètent
	fondS = SDL_CreateRGBSurface(0, 1, 1, 32, rmask, gmask, bmask, amask);
	SDL_FillRect(fondS, NULL, SDL_MapRGBA(fondS->format, MENU_COULEUR_BORDURE_R, MENU_COULEUR_BORDURE_G, MENU_COULEUR_BORDURE_B, 255));
	fondSombre = SDL_CreateTextureFromSurface(this->renderer, fondS);
	SDL_FreeSurface(fondS);
	// haut
	fond.y = this->positionY; // TODO : prendre en compte l'apaisseur de la bordure
	fond.h = 1;
	SDL_RenderCopy(this->renderer, fondSombre, NULL, &fond);
	// bas
	fond.y = this->positionY+this->tailleY; // TODO : prendre en compte l'apaisseur de la bordure
	SDL_RenderCopy(this->renderer, fondSombre, NULL, &fond);
	// gauche
	fond.y = this->positionY; // TODO : prendre en compte l'apaisseur de la bordure
	fond.h = this->tailleY;
	fond.w = 1;
	SDL_RenderCopy(this->renderer, fondSombre, NULL, &fond);
	// droite
	fond.x = this->positionX+this->tailleX;
	SDL_RenderCopy(this->renderer, fondSombre, NULL, &fond);
	// Séparation
	fond.y = this->positionY+(this->tailleBouton*(NIVEAU_ZOOM_BOUTON_ACTIF+1)); // TODO : prendre en compte l'appaisseur de la bordure
	fond.x = this->positionX+4;
	fond.w = this->tailleX-8;
	fond.h = 2;
	SDL_RenderCopy(this->renderer, fondSombre, NULL, &fond);
	SDL_DestroyTexture(fondSombre);

	// On dessine les boutons de catégorie en tenant compte de la sélection
	int tailleXTotale = this->categories.size() * this->tailleBouton + this->tailleBouton*NIVEAU_ZOOM_BOUTON_ACTIF;
	int refX = this->positionX;

	// On calcul les déplacements maximum des icônes
	int refXMin = this->positionX-MENU_CATEGORIE_DEPLACEMENT_MINIMUM;
	int refXMax = this->positionX+MENU_CATEGORIE_DEPLACEMENT_MINIMUM;
	if(tailleXTotale > this->tailleX)
		refXMin = refX - (tailleXTotale - this->tailleX) - MENU_CATEGORIE_DEPLACEMENT_MINIMUM;

	// On décale X vers la gauche ou la droite selon la position du joystic
	ToucheJeu* axeX = this->touches[TOUCHE_NAVIGATION_DEPLACER_AXE_GAUCHE_DROITE];
	refX += ((axeX->getVal() - axeX->getValeurMin()) * (refXMax - refXMin)/(axeX->getValeurMax() - axeX->getValeurMin())) + refXMin;

	// Si on est sorti des limites, on replace
	if(refX > refXMax + 5 - MENU_CATEGORIE_DEPLACEMENT_MINIMUM)
		axeX->setVal(axeX->getVal()-PRECISION_VALEUR_AXE_TOUCHE/20);
	if(refX < refXMin + MENU_CATEGORIE_DEPLACEMENT_MINIMUM)
		axeX->setVal(axeX->getVal()+PRECISION_VALEUR_AXE_TOUCHE/20);

	// On affiche
	for(int i=0; i<this->categories.size(); i++)
	{
		// On calcul la taille
		int tailleB = this->tailleBouton;
		if(i == categorieSelect)
			tailleB = this->tailleBouton*(NIVEAU_ZOOM_BOUTON_ACTIF+1);

		// On calcul sa position
		SDL_Rect pos; // (X.Y)=position image sur l'écran, (W.H)=taille de l'image sur l'écran (sans prendre compte de la coupure)
		pos.y = this->positionY + (this->tailleBouton*(NIVEAU_ZOOM_BOUTON_ACTIF+1)/2);
		pos.x = refX+(tailleB/2);
		pos.w = tailleB;
		pos.h = tailleB;
		refX += tailleB;

		// On regarde si on doit l'afficher
		if(pos.x < this->tailleX+this->positionX+(tailleB/2) && pos.x > (this->positionX-(tailleB/2)))
		{
			// S'il est aux extrémités, on met à jour Pos2
			SDL_Rect pos2; // (X.Y)=position de la zone à prendre sur l'image, (W.H)=taille de la zone à prendre sur l'image
			pos2.x = 0;
			pos2.y = 0;
			pos2.w = tailleB;
			pos2.h = tailleB;
			// gauche
			if(pos.x < this->positionX + (tailleB/2))
			{
				pos2.x = tailleB-((pos.x+(tailleB/2))-this->positionX);
				pos2.y = 0;
				pos2.w -= pos2.x;
				pos2.h = tailleB;
			}
			// droite
			if(pos.x > this->tailleX+this->positionX - (tailleB/2))
			{
				pos2.x = 0;
				pos2.y = 0;
				pos2.w = this->tailleX+this->positionX - (pos.x-(tailleB/2));
				pos2.h = tailleB;
			}

			// Si sa position est dans les limites, on l'affiche
			this->categories[i]->setTaille(this->tailleBouton);
			this->categories[i]->afficher(&pos, (i == categorieSelect), 255, &pos2);
		}
	}
	
	// On dessine les boutons de la catégorie sélectionnée en tenant compte de la sélection
	int tailleBt = this->tailleBouton*(1+NIVEAU_ZOOM_BOUTON_ACTIF);
	int decalageX = (tailleBt*3)/4;
	int tailleX = 0;
	int tailleY = 0;
	int xRef = 0;
	int yRef = 0;

	// X
	xRef = tailleBt;
	this->nbBtParLigne = 0;
	for(int i=0; xRef<this->tailleX; i++)
	{
		this->nbBtParLigne++;
		xRef += decalageX;
	}
	tailleX = xRef - decalageX;
	xRef = this->positionX+((this->tailleX-tailleX)/2);
	// Y
	tailleY = tailleBt/2;
	for(int i=0; i<this->nbBoutons[categorieSelect]; i += this->nbBtParLigne)
		tailleY += tailleBt;
	yRef = this->positionY+tailleBt;
	int tailleZoneY = this->tailleY-tailleBt-(MENU_TAILLE_BANDE_TEXTE*this->tailleBouton);
	if(tailleZoneY > tailleY)
		yRef = this->positionY+tailleBt+(tailleZoneY/2)-(tailleY/2);

	int refYMin = yRef-MENU_CATEGORIE_DEPLACEMENT_MINIMUM;
	int refYMax = yRef+MENU_CATEGORIE_DEPLACEMENT_MINIMUM;
	if(tailleY-tailleZoneY>0)
		refYMin -= (tailleY-tailleZoneY);

	// On met à jour les valeurs selon les inputs
	
	// On calcul le numéro absolu du premier bouton du menu
	int btSelect = 0;
	for(int i=0; i<categorieSelect; i++)
		btSelect += nbBoutons[i];

	// On affiche l'ensemble des boutons
	for(int noLigne = 0, noBouton = 0; noBouton<this->nbBoutons[categorieSelect]; noLigne++)
	{
		bool decaler = false;
		for(int noBoutonLigne = 0; noBoutonLigne<this->nbBtParLigne && noBouton<this->nbBoutons[categorieSelect]; noBoutonLigne++)
		{
			// On calcul la position
			SDL_Rect p1;
			p1.x=xRef+(decalageX*noBoutonLigne)+(tailleBt/2);
			p1.y=yRef+(noLigne*tailleBt)+(tailleBt/2);
			if(decaler)
				p1.y+=tailleBt/2;
			p1.w=this->tailleBouton;
			p1.h=this->tailleBouton;
			decaler = !decaler;

			// On calcul la découpe si besoin
			SDL_Rect p2;
			p2.x=0;
			p2.y=0;
			p2.w=this->tailleBouton;
			p2.h=this->tailleBouton;
			
			// On affiche le bouton
			this->boutons[btSelect+noBouton]->afficher(&p1, (noBouton == this->boutonSelect), &p2);
			noBouton++;
		}
	}
	

	
	



	// On affiche le nom et la description de la sélection
	if(this->boutons.size()>this->boutonSelect)
	{
		// On ajoute le fond
		SDL_Rect fond;
		fond.x = this->positionX+1;
		fond.y = (this->positionY+this->tailleY)-(this->tailleBouton*MENU_TAILLE_BANDE_TEXTE);
		fond.w = this->tailleX-1;
		fond.h = this->tailleBouton*MENU_TAILLE_BANDE_TEXTE;
		SDL_Surface* fondS = SDL_CreateRGBSurface(0, 1, 1, 32, rmask, gmask, bmask, amask);
		SDL_FillRect(fondS, NULL, SDL_MapRGBA(fondS->format, MENU_COULEUR_FOND_R, MENU_COULEUR_FOND_G, MENU_COULEUR_FOND_B, MENU_OPACITE_BANDE_TEXTE));
		SDL_Texture* fondSombre = SDL_CreateTextureFromSurface(this->renderer, fondS);
		SDL_FreeSurface(fondS);
		SDL_RenderCopy(this->renderer, fondSombre, NULL, &fond);
		SDL_DestroyTexture(fondSombre);
		fondS = SDL_CreateRGBSurface(0, 1, 1, 32, rmask, gmask, bmask, amask);
		SDL_FillRect(fondS, NULL, SDL_MapRGBA(fondS->format, MENU_COULEUR_BORDURE_R, MENU_COULEUR_BORDURE_G, MENU_COULEUR_BORDURE_B, 255));
		fondSombre = SDL_CreateTextureFromSurface(this->renderer, fondS);
		SDL_FreeSurface(fondS);
		// haut
		fond.h = 1;
		SDL_RenderCopy(this->renderer, fondSombre, NULL, &fond);
		SDL_DestroyTexture(fondSombre);

		// On ajoute le titre
		btSelect += this->boutonSelect;
		SDL_Color couleurTexte = {MENU_COULEUR_TEXTE_R, MENU_COULEUR_TEXTE_G, MENU_COULEUR_TEXTE_B, 255};
		SDL_Surface* texte = TTF_RenderUTF8_Blended(this->policeMenu, this->boutons[btSelect]->getNom().c_str(), couleurTexte);
		if(texte != nullptr)
		{
			// On calcul sa position et on l'affiche
			SDL_Rect r;
			r.x = fond.x+(fond.w/2)-(texte->w/2);
			r.y = fond.y+(texte->h*2);
			r.h = texte->h; // TODO : rendre texte plus gros, puis diminuer selon la taille de référence
			r.w = texte->w;
			SDL_Texture* texteT = SDL_CreateTextureFromSurface(this->renderer, texte);
			SDL_FreeSurface(texte);
			SDL_RenderCopy(this->renderer, texteT, nullptr, &r);
			SDL_DestroyTexture(texteT);
		}
		else
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de créer le texte : %s", TTF_GetError());

		// On ajoute le texte
		texte = TTF_RenderUTF8_Blended(this->policeMenu, this->boutons[btSelect]->getDescription().c_str(), couleurTexte);
		if(texte != nullptr)
		{
			// On calcul sa position et on l'affiche
			SDL_Rect r;
			r.x = fond.x+(fond.w/2)-(texte->w/2);
			r.y = fond.y+(texte->h*4);
			r.h = texte->h; // TODO : rendre texte plus gros, puis diminuer selon la taille de référence
			r.w = texte->w;
			SDL_Texture* texteT = SDL_CreateTextureFromSurface(this->renderer, texte);
			SDL_FreeSurface(texte);
			SDL_RenderCopy(this->renderer, texteT, nullptr, &r);
			SDL_DestroyTexture(texteT);
		}
	}

	return 0;
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

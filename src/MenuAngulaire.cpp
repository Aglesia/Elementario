#include <Menu.h>
#include <ToucheJeu.h>
#include <defs.h>

MenuAngulaire::MenuAngulaire(ToucheJeu** touches, SDL_Renderer* renderer, TTF_Font* policeMenu)
{
	this->policeMenu = policeMenu;
	this->renderer = renderer;
	this->nbBoutons.clear();
	this->categories.clear();
	this->boutons.clear();
	this->touches = touches;
	this->typeAnimation = 1;
}

MenuAngulaire::~MenuAngulaire()
{
	this->nbBoutons.clear();
	this->categories.clear();
	this->boutons.clear();
}

/**
 * Ajoute un ou plusieurs boutons
 * @param boutons     Ensemble des boutons contenus dans la catégorie, le premier est le bouton représentant la catégorie, qui sera mis dans la barre des catégories, en haut
 */
void MenuAngulaire::ajoutBoutons(std::vector<Bouton*> boutons)
{
	if(boutons.size()>1)
	{
		this->nbBoutons.push_back(boutons.size()-1);
		this->categories.push_back(boutons[0]);
		for(int i=1; i<boutons.size(); i++)
			this->boutons.push_back(boutons[i]);
	}
}

void MenuAngulaire::ajoutCategorie(std::vector<Bouton*> boutons)
{
	// On remplace les boutons existants, et on extrait le titre de la catégorie
}

/**
 * Met à jour les actions boutons, puis l'affichage selon les déplacements/sélections effectués
 * Les déplacements horizontaux du menu servent aux catégories, les déplacement verticaux servent aux boutons de sous-catégorie
 * @return 0 s'il n'y a rien eu de spécifique, -1 si annulé, n° du bouton (en partant de 1) si un bouton a été validé, -([n°bouton]-1) en cas d'option avancée
 */
int MenuAngulaire::update(int ticks) // TODO
{
	// Si on est en animation, on la gère
	int opacite = 255;
	// TODO : Animation : fade

	// S'il n'y a pas de résultat, on regarde si on doit changer qque chose // TODO : optimisation, on ne recalcul le tout que si nécessaire

	// On regarde les input et on les met à jour en conséquence
	// On calcul la taille si elle sort des ilmites
	if(this->tailleBouton>this->tailleY/3)
		this->tailleBouton = tailleY/3;
	if(this->tailleBouton>this->tailleX)
		this->tailleBouton = tailleX;

	// On calcul la nouvelle sélection, et les nouveaux emplacements
	// Bouton précédent
	if(this->touches[TOUCHE_ANGULAIRE_BOUTON_ANGLE]->getVal()<0)
	{
		this->touches[TOUCHE_ANGULAIRE_BOUTON_ANGLE]->setVal(0);
		// On change le bouton
		this->boutonSelect--;
		if(this->boutonSelect < 0)
			this->boutonSelect = 0;
	}
	// Bouton suivant
	if(this->touches[TOUCHE_ANGULAIRE_BOUTON_ANGLE]->getVal()>0)
	{
		this->touches[TOUCHE_ANGULAIRE_BOUTON_ANGLE]->setVal(0);
		// On change le bouton
		this->boutonSelect++;
		if(this->boutonSelect>=this->nbBoutons)
			this->boutonSelect = this->nbBoutons-1;
	}
	// Bouton valider
	if(this->touches[TOUCHE_ANGULAIRE_VALIDER]->getVal()>0)
	{
		this->result = this->boutonSelect+1;
		return result;
	}
	// Bouton annuler
	if(this->touches[TOUCHE_ANGULAIRE_RETOUR]->getVal()>0)
	{
		this->result = -1;
		return result;
	}
	// Bouton avancé
	if(this->touches[TOUCHE_ANGULAIRE_AVANCE]->getVal()>0)
	{
		this->result = -1-this->boutonSelect;
		return result;
	}

	// On dessine le fond du menu
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

	// On crée les textes
	int hauteurTexte = this->makeTextes();

	// S'il y'a eu un mouvement, on l'indique
	bool eventSouris = (this->touches[TOUCHE_SOURIS_AXE_X]->isEvent() || this->touches[TOUCHE_SOURIS_AXE_Y]->isEvent());
	ToucheJeu* sourisX = this->touches[TOUCHE_SOURIS_AXE_X];
	ToucheJeu* sourisY = this->touches[TOUCHE_SOURIS_AXE_Y];
	int xR = (sourisX->getVal() - sourisX->getValeurMin()) * this->tailleEcranX/(sourisX->getValeurMax() - sourisX->getValeurMin());
	int yR = (sourisY->getVal() - sourisY->getValeurMin()) * this->tailleEcranY/(sourisY->getValeurMax() - sourisY->getValeurMin());

	// On dessine les boutons de la catégorie sélectionnée en tenant compte de la sélection
	int tailleBt = this->tailleBouton*(1+(NIVEAU_ZOOM_BOUTON_ACTIF/2));
	int tailleBtX = this->tailleBouton*(1+NIVEAU_ZOOM_BOUTON_ACTIF);
	int decalageX = (tailleBtX*2)/3;
	int tailleX = 0;
	int tailleY = 0;
	int xRef = 0;
	int yRef = 0;

	// X
	xRef = tailleBtX;
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
	yRef = this->positionY+tailleBtX;
	int tailleZoneY = (this->tailleY-tailleBtX)-hauteurTexte;
	if(tailleZoneY > tailleY)
		yRef = this->positionY+tailleBtX+(tailleZoneY/2)-(tailleY/2);

	int refYMin = -MENU_CATEGORIE_DEPLACEMENT_MINIMUM;
	int refYMax = +MENU_CATEGORIE_DEPLACEMENT_MINIMUM;
	if(tailleY-tailleZoneY>0)
		refYMin -= (tailleY-tailleZoneY);

	// On décale X vers la gauche ou la droite selon la position du joystic
	ToucheJeu* axeY = this->touches[TOUCHE_NAVIGATION_DEPLACER_AXE_HAUT_BAS];
	yRef += ((axeY->getVal() - axeY->getValeurMin()) * (refYMax - refYMin)/(axeY->getValeurMax() - axeY->getValeurMin())) + refYMin;

	// On calcul le numéro absolu du premier bouton du menu
	int btSelect = 0;
	for(int i=0; i<categorieSelect; i++)
		btSelect += nbBoutons[i];

	// On regarde si on doit prendre en compte la souris
	eventSouris = (eventSouris &&
		xR>this->positionX && xR<this->positionX+this->tailleX &&
		yR>this->positionY+tailleBtX && yR<this->positionY+tailleBtX+tailleZoneY);

	// On affiche l'ensemble des boutons
	for(int noLigne = 0, noBouton = 0; noBouton<this->nbBoutons[categorieSelect]; noLigne++)
	{
		bool decaler = false;
		for(int noBoutonLigne = 0; noBoutonLigne<this->nbBtParLigne && noBouton<this->nbBoutons[categorieSelect]; noBoutonLigne++)
		{
			// On calcul la position
			SDL_Rect p1;
			p1.x=xRef+(decalageX*noBoutonLigne)+(tailleBtX/2);
			p1.y=yRef+(noLigne*tailleBt)+(tailleBt/2);
			if(decaler)
				p1.y+=tailleBt/2;
			p1.w=this->tailleBouton;
			p1.h=this->tailleBouton;
			decaler = !decaler;

			// On regarde si la souris est dessus
			if(eventSouris)
			{
				// On regarde s'il est sur notre bouton
				if(this->boutons[btSelect+noBouton]->estPointe(p1.w, p1.h, xR-p1.x, yR-p1.y))
					this->boutonSelect = noBouton;
			}

			// On calcul la découpe si besoin
			SDL_Rect p2;
			p2.x=0;
			p2.y=0;
			p2.w=this->tailleBouton;
			p2.h=this->tailleBouton;

			if(noBouton == this->boutonSelect)
			{
				// Limite haute
				if(p1.y-(tailleBtX/2)<this->positionY+tailleBtX)
					p2.y = (this->positionY+tailleBtX)-(p1.y-(tailleBtX/2))+1;
				// Limite basse
				if(p1.y+(tailleBtX/2)>this->positionY+this->tailleY)
					p2.h -= ((p1.y+(tailleBtX/2))-(this->positionY+this->tailleY))/(1+NIVEAU_ZOOM_BOUTON_ACTIF);
			}
			else
			{
				// Limite haute
				if(p1.y-(this->tailleBouton/2)<this->positionY+tailleBtX)
					p2.y = (this->positionY+tailleBtX)-(p1.y-(this->tailleBouton/2))+1;
				// Limite basse
				if(p1.y+(this->tailleBouton/2)>this->positionY+this->tailleY)
					p2.h = (this->positionY+this->tailleY)-(p1.y-(this->tailleBouton/2));
			}
			p2.h -= p2.y;				

			// On affiche le bouton
			this->boutons[btSelect+noBouton]->afficher(&p1, (noBouton == this->boutonSelect), 255, &p2);
			noBouton++;
		}
	}

	// On affiche le nom et la description de la sélection
	if(this->boutons.size()>this->boutonSelect)
	{
		// On ajoute le fond
		SDL_Rect fond;
		fond.x = this->positionX+1;
		fond.y = (this->positionY+this->tailleY)-hauteurTexte;
		fond.w = this->tailleX-1;
		fond.h = hauteurTexte;
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
		SDL_Rect r;
		r.y = fond.y+10;
		for(int i=0; i<this->texteT.size(); i++)
		{
			SDL_QueryTexture(this->texteT[i], nullptr, nullptr, &(r.w), &(r.h));
			r.x = fond.x+(fond.w/2)-(r.w/2);
			SDL_RenderCopy(this->renderer, this->texteT[i], nullptr, &r);
			r.y += r.h;
		}

		r.y += 10;

		// On ajoute la description
		for(int i=0; i<this->texteD.size(); i++)
		{
			SDL_QueryTexture(this->texteD[i], nullptr, nullptr, &(r.w), &(r.h));
			r.x = fond.x+(fond.w/2)-(r.w/2);
			SDL_RenderCopy(this->renderer, this->texteD[i], nullptr, &r);
			r.y += r.h;
		}
	}

	// On dessine les bords du menu // TODO : images, motifs qui se répètent
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

	return 0;
}

int MenuAngulaire::makeTextes() // TODO : spliter le texte et faire des retours à la ligne si besoin
{
	int taille = 10;
	int btSelect = 0;
	for(int i=0; i<categorieSelect; i++)
		btSelect += nbBoutons[i];

	btSelect += this->boutonSelect;

	// On vide les anciennes textures
	for(int i=0; i<this->texteT.size(); i++)
		SDL_DestroyTexture(this->texteT[i]);
	this->texteT.clear();
	for(int i=0; i<this->texteD.size(); i++)
		SDL_DestroyTexture(this->texteD[i]);
	this->texteD.clear();

	SDL_Color couleurTexteT = {MENU_COULEUR_TEXTE_R, MENU_COULEUR_TEXTE_G, MENU_COULEUR_TEXTE_B, 255};
	SDL_Color couleurTexteD = {MENU_COULEUR_TEXTE_R, MENU_COULEUR_TEXTE_G, MENU_COULEUR_TEXTE_B, 255};

	// On crée le ou les textes
	std::string nom = this->boutons[btSelect]->getNom();
	std::string desc = this->boutons[btSelect]->getDescription();
	SDL_Surface* texte = TTF_RenderUTF8_Blended(this->policeMenu, nom.c_str(), couleurTexteT);
	if(texte != nullptr)
	{
		// On crée la texture et on l'ajoute à la liste
		SDL_Texture* t = SDL_CreateTextureFromSurface(this->renderer, texte);
		if(t!=nullptr)
		{
			this->texteT.push_back(t);
			int x, y;
			SDL_QueryTexture(t, nullptr, nullptr, &x, &y);
			taille += y;
		}
		else
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de créer la texture : %s", SDL_GetError());
		SDL_FreeSurface(texte);
	}
	else
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de créer le texte : %s", TTF_GetError());

	taille += 10;

	// Description
	texte = TTF_RenderUTF8_Blended(this->policeMenu, desc.c_str(), couleurTexteD);
	if(texte != nullptr)
	{
		// On crée la texture et on l'ajoute à la liste
		SDL_Texture* t = SDL_CreateTextureFromSurface(this->renderer, texte);
		if(t!=nullptr)
		{
			this->texteD.push_back(t);
			int x, y;
			SDL_QueryTexture(t, nullptr, nullptr, &x, &y);
			taille += y;
		}
		else
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de créer la texture : %s", SDL_GetError());
		SDL_FreeSurface(texte);
	}
	else
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de créer le texte : %s", TTF_GetError());

	return taille+5;
}

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
	if(tailleBoutons>0)
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
 * Position absolue du menu, en pixels. 0.0 = haut.gauche
 * @param affichageX Position absolue du point haut gauche de la zone
 * @param affichageY Position absolue du point haut gauche de la zone
 */
void Menu::setTailleEcran(int tailleX, int tailleY)
{
	this->tailleEcranX = tailleX;
	this->tailleEcranY = tailleY;
}

/**
 * Ajoute une catégorie de boutons au menu
 * @param boutons     Ensemble des boutons contenus dans la catégorie, le premier est le bouton représentant la catégorie, qui sera mis dans la barre des catégories, en haut
 */
void Menu::ajoutCategorie(std::vector<Bouton*> boutons)
{
	if(boutons.size()>1)
	{
		this->nbBoutons.push_back(boutons.size()-1);
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

void Menu::reset()
{
	this->result = 0;
}

/**
 * Met à jour les actions boutons, puis l'affichage selon les déplacements/sélections effectués
 * Les déplacements horizontaux du menu servent aux catégories, les déplacement verticaux servent aux boutons de sous-catégorie
 * @return 0 s'il n'y a rien eu de spécifique, -1 si annulé, n° du bouton (en partant de 1) si un bouton a été validé, -([n°bouton]-1) en cas d'option avancée
 */
int Menu::update(int ticks) // TODO : optimiser (ne recalcul que si besoin) // TODO BUG : Bouton sélectionné arrive à la limite haute : monte trop vite // BUG : Android double-clic -> sélection bouton = valider
{
	// Si on est en animation, on la gère
	int opacite = 255;
	// TODO : Animations :
	// - Fade pour afficher/enlever le menu
	// - Lorsque les catégories ne bougent pas, ils s'assombrissent et le titre/description de la catégorie apparaît en planc par dessus

	// S'il n'y a pas de résultat, on regarde si on doit changer qque chose // TODO : optimisation, on ne recalcul le tout que si nécessaire

	// On regarde les input et on les met à jour en conséquence
	// On calcul la taille si elle sort des ilmites
	if(this->tailleBouton>this->tailleY/3)
		this->tailleBouton = tailleY/3;
	if(this->tailleBouton>this->tailleX)
		this->tailleBouton = tailleX;

	// On calcul la nouvelle sélection, et les nouveaux emplacements
	// Catégorie précédente
	if(this->touches[TOUCHE_NAVIGATION_SELECTION_BOUTON_CATEGORIE_PRECEDENTE]->isEvent())
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
	if(this->touches[TOUCHE_NAVIGATION_SELECTION_BOUTON_CATEGORIE_SUIVANTE]->isEvent())
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
	// Bouton haut
	if(this->touches[TOUCHE_NAVIGATION_SELECTION_BOUTON_HAUT_BAS]->getVal()<0)
	{
		this->touches[TOUCHE_NAVIGATION_SELECTION_BOUTON_HAUT_BAS]->setVal(0);
		// On change le bouton
		if(this->boutonSelect-this->nbBtParLigne >= 0)
			this->boutonSelect -= this->nbBtParLigne;
	}
	// Bouton bas
	if(this->touches[TOUCHE_NAVIGATION_SELECTION_BOUTON_HAUT_BAS]->getVal()>0)
	{
		this->touches[TOUCHE_NAVIGATION_SELECTION_BOUTON_HAUT_BAS]->setVal(0);
		// On change le bouton
		if(this->boutonSelect+this->nbBtParLigne < this->nbBoutons[this->categorieSelect])
			this->boutonSelect += this->nbBtParLigne;
	}
	// Bouton annuler
	if(this->touches[TOUCHE_NAVIGATION_RETOUR]->isEvent())
	{
		this->result = -1;
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

	// S'il y'a eu un mouvement, on l'indique
	bool eventSouris = (this->touches[TOUCHE_SOURIS_AXE_X]->isEvent() || this->touches[TOUCHE_SOURIS_AXE_Y]->isEvent());
	ToucheJeu* sourisX = this->touches[TOUCHE_SOURIS_AXE_X];
	ToucheJeu* sourisY = this->touches[TOUCHE_SOURIS_AXE_Y];
	int xR = (sourisX->getVal() - sourisX->getValeurMin()) * this->tailleEcranX/(sourisX->getValeurMax() - sourisX->getValeurMin());
	int yR = (sourisY->getVal() - sourisY->getValeurMin()) * this->tailleEcranY/(sourisY->getValeurMax() - sourisY->getValeurMin());

	// Si on a cliqué, on l'indique
	bool clic = (this->touches[TOUCHE_SOURIS_CLIC]->isEvent() && this->touches[TOUCHE_SOURIS_CLIC]->getVal()>0);
	if(clic)
		eventSouris = true;

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

			// On regarde si la souris est dessus
			if(eventSouris)
			{
				// On regarde s'il est sur notre bouton
				if(this->categories[i]->estPointe(pos.w, pos.h, xR-pos.x, yR-pos.y))
				{
					this->categorieSelect = i;
					this->boutonSelect = 0;
				}
			}

			// Si sa position est dans les limites, on l'affiche
			this->categories[i]->setTaille(this->tailleBouton);
			this->categories[i]->afficher(&pos, false, 255, &pos2);
		}
	}

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
	if(this->nbBoutons.size()>categorieSelect)
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
	if(this->nbBoutons.size()>categorieSelect)
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
					{
						if(this->boutonSelect == noBouton && clic)
						{
							this->result = btSelect+noBouton+1;
							return this->result;
						}
						this->boutonSelect = noBouton;
					}
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

	// Bouton valider
	if(this->touches[TOUCHE_NAVIGATION_VALIDER]->isEvent())
	{
		this->result = btSelect+this->boutonSelect+1;
		return result;
	}
	// Bouton avancé
	if(this->touches[TOUCHE_NAVIGATION_AVANCE]->isEvent())
	{
		this->result = -2-(btSelect+this->boutonSelect);
		return result;
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

int Menu::makeTextes() // TODO : spliter le texte et faire des retours à la ligne si besoin
{
	if(this->categories.size()<=this->categorieSelect)
		return 0;

	int taille = 10;
	int btSelect = 0;
	for(int i=0; i<categorieSelect; i++)
		btSelect += nbBoutons[i];

	btSelect += this->boutonSelect;

	if(this->boutons.size()<=btSelect)
		return 0;

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

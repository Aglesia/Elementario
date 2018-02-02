#include <MenuAngulaire.h>
#include <defs.h>

MenuAngulaire::MenuAngulaire(ToucheJeu** touches, SDL_Renderer* renderer, TTF_Font* policeMenu)
	: Menu(touches, renderer, policeMenu)
{
	this->boutonSelect = -1;
}

/**
 * Ajoute un ou plusieurs boutons
 * @param boutons     Ensemble des boutons contenus dans la catégorie, le premier est le bouton représentant la catégorie, qui sera mis dans la barre des catégories, en haut
 */
void MenuAngulaire::ajoutBoutons(std::vector<Bouton*> boutons)
{
	if(boutons.size()>1)
	{
		this->nbBoutons = boutons.size()-1;
		this->categorie = boutons[0];
		this->nbBoutons = boutons.size()-1;
		for(int i=1; i<boutons.size(); i++)
			this->boutons.push_back(boutons[i]);
	}
}

void MenuAngulaire::ajoutCategorie(std::vector<Bouton*> boutons)
{
	// On remplace les boutons existants, et on extrait le titre de la catégorie
	this->ajoutBoutons(boutons);
}

void MenuAngulaire::reset()
{
	Menu::reset();
	this->boutonSelect = -1;
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

	// On calcul la nouvelle sélection, et les nouveaux emplacements
	// Bouton précédent
	if(this->touches[TOUCHE_ANGULAIRE_BOUTON_ANGLE]->isEvent())
	{
		this->touches[TOUCHE_ANGULAIRE_BOUTON_ANGLE]->setVal(0);
		// On change le bouton
		this->boutonSelect--;
		if(this->boutonSelect < 0)
			this->boutonSelect = 0;
	}
	// Bouton suivant
	if(this->touches[TOUCHE_ANGULAIRE_BOUTON_ANGLE]->isEvent())
	{
		this->touches[TOUCHE_ANGULAIRE_BOUTON_ANGLE]->setVal(0);
		// On change le bouton
		this->boutonSelect++;
		if(this->boutonSelect>=this->nbBoutons)
			this->boutonSelect = this->nbBoutons-1;
	}
	// Bouton annuler
	if(this->touches[TOUCHE_ANGULAIRE_RETOUR]->isEvent())
	{
		this->result = -1;
		this->boutonSelect = -1;
		return result;
	}

	// On dessine le fond du menu
	SDL_Rect fond;
	fond.x = 0;
	fond.y = 0;
	fond.w = this->tailleEcranX;
	fond.h = this->tailleEcranY;
	SDL_Surface* fondS = SDL_CreateRGBSurface(0, 1, 1, 32, rmask, gmask, bmask, amask);
	SDL_FillRect(fondS, NULL, SDL_MapRGBA(fondS->format, MENU_ANGULAIRE_COULEUR_FOND_R, MENU_ANGULAIRE_COULEUR_FOND_G, MENU_ANGULAIRE_COULEUR_FOND_B, MENU_ANGULAIRE_OPACITE_FOND));
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

	// On dessine les boutons en tenant compte de la sélection
	int tailleBt = this->tailleBouton*(1+(NIVEAU_ZOOM_BOUTON_ACTIF/2));
	int tailleBtX = this->tailleBouton*(1+NIVEAU_ZOOM_BOUTON_ACTIF);
	int xRef = this->tailleEcranX/2;
	int yRef = this->tailleEcranY/2;

	int angle = -1;

	// Si on a cliqué, on l'indique
	bool clic = (this->touches[TOUCHE_SOURIS_CLIC]->isEvent() && this->touches[TOUCHE_SOURIS_CLIC]->getVal()>0);
	if(clic)
		eventSouris = true;

	// Si la souris a bougée, on calcul son nouvel angle
	if(eventSouris)
	{
		int mouseX = xR - (this->tailleEcranX/2);
		int mouseY = yR - (this->tailleEcranY/2);
		if(std::abs(mouseX)>tailleBt/2 || std::abs(mouseY)>tailleBt/2)
		{
			if(mouseX == 0)
				mouseX = 1;
			angle = (180*std::atan(std::abs(mouseY)/std::abs(mouseX)))/PI;
			if(mouseX<0)
			{
				if(mouseY<0)
					angle += 180;
				else
					angle = 180-angle;
			}
			else if(mouseY<0)
				angle = 360-angle;
		}
		else
			angle = -2;
	}
	// Si un axe de l'angle a bougé, on calcul le nouvel angle
	if(this->touches[TOUCHE_ANGULAIRE_AXE_ANGLE_X]->isEvent() || this->touches[TOUCHE_ANGULAIRE_AXE_ANGLE_Y]->isEvent())
	{
		ToucheJeu* axeX = this->touches[TOUCHE_ANGULAIRE_AXE_ANGLE_X];
		ToucheJeu* axeY = this->touches[TOUCHE_ANGULAIRE_AXE_ANGLE_Y];
		int mouseX = axeX->getVal();
		int mouseY = axeY->getVal();
		if(std::abs(mouseX)>tailleBtX || std::abs(mouseY)>tailleBtX)
		{
			if(mouseX == 0)
				mouseX = 1;
			angle = (180*std::atan(std::abs(mouseY)/std::abs(mouseX)))/PI;
			if(mouseX<0)
			{
				if(mouseY<0)
					angle += 180;
				else
					angle = 180-angle;
			}
			else if(mouseY<0)
				angle = 360-angle;
		}
		else
			angle = -2;
	}
	// Si l'angle a changé, on regarde quel bouton est sélectionné
	if(angle == -2)
		this->boutonSelect = -1;
	if(angle >= 0)
	{
		int nbDegParBt = 0;
		if(this->nbBoutons>0)
			nbDegParBt = 360/this->nbBoutons;
		if(nbDegParBt==0)
			nbDegParBt = 360;
		// On change la référence par l'axe du haut
		angle += 90;
		if(angle>360)
			angle-=360;

		// On change la référence pour le début du premier bouton (la moitier du premier bouton étant à 0° actuellement)
		angle += nbDegParBt/2;
		if(angle>360)
			angle-=360;

		// On regarde sur quel bouton on tombe
		int noBouton = std::floor(angle/nbDegParBt);
		if(noBouton>=this->nbBoutons)
			noBouton = 0;
		this->boutonSelect = noBouton;
	}

	// On affiche le bouton retour
	{
		// On calcul la position
		SDL_Rect p1;
		p1.x=xRef; // TODO
		p1.y=yRef;
		p1.w=this->tailleBouton;
		p1.h=this->tailleBouton;

		// On regarde si la souris est dessus
		if(eventSouris)
		{
			// On regarde s'il est sur notre bouton
			if(this->categorie->estPointe(p1.w, p1.h, xR-p1.x, yR-p1.y))
			{
				// Si c'est déjà le bouton sélectionné, on retourne cette valeur
				if(this->boutonSelect == -1 && clic)
				{
					this->result = -1;
					return -1;
				}
				this->boutonSelect = -1;
			}
		}

		SDL_Rect p2;
		p2.x=0;
		p2.y=0;
		p2.w=this->tailleBouton;
		p2.h=this->tailleBouton;

		// On affiche le bouton
		this->categorie->afficher(&p1, false, (this->boutonSelect == -1)?180:100, &p2);
	}

	// On affiche l'ensemble des boutons
	for(int noBouton = 0; noBouton<this->nbBoutons; noBouton++)
	{
		// On calcul la position
		SDL_Rect p1;
		p1.x=xRef;
		p1.y=yRef;
		p1.w=this->tailleBouton;
		p1.h=this->tailleBouton;

		switch(noBouton)
		{
			case 0:
				p1.y -= tailleBtX;
				break;

			case 1:
				p1.x += tailleBtX;
				break;

			case 2:
				p1.y += tailleBtX;
				break;

			case 3:
				p1.x -= tailleBtX;
				break;

			default:
				break;
		}

		// On regarde si la souris est dessus
		if(eventSouris)
		{
			// On regarde s'il est sur notre bouton
			if(this->boutons[noBouton]->estPointe(p1.w, p1.h, xR-p1.x, yR-p1.y))
			{
				// Si c'est déjà le bouton sélectionné, on retourne cette valeur
				if(this->boutonSelect == noBouton && clic)
				{
					this->result = noBouton+1;
					return this->result;
				}
				this->boutonSelect = noBouton;
			}
		}

		SDL_Rect p2;
		p2.x=0;
		p2.y=0;
		p2.w=this->tailleBouton;
		p2.h=this->tailleBouton;

		// On affiche le bouton
		this->boutons[noBouton]->afficher(&p1, (noBouton == this->boutonSelect), 255, &p2);
	}

	// On affiche le nom et la description de la sélection
	if(this->boutons.size()>this->boutonSelect || this->boutonSelect == -1)
	{
		// On ajoute le fond
		SDL_Rect fond;
		fond.x = 0;
		fond.y = this->tailleEcranY-hauteurTexte;
		fond.w = this->tailleEcranX;
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

	// Bouton valider
	if(this->touches[TOUCHE_ANGULAIRE_VALIDER]->isEvent())
	{
		this->result = this->boutonSelect+1;
		if(this->result == 0)
			this->result = -1;
		return result;
	}
	// Bouton avancé
	if(this->touches[TOUCHE_ANGULAIRE_AVANCE]->isEvent() && this->boutonSelect>=0)
	{
		this->result = -2-this->boutonSelect;
		return result;
	}

	return 0;
}

int MenuAngulaire::makeTextes() // TODO : spliter le texte et faire des retours à la ligne si besoin
{
	if(this->boutons.size()<=this->boutonSelect && this->boutonSelect>=0)
		return 0;

	int taille = 40;

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
	std::string nom = this->categorie->getNom();
	std::string desc = this->categorie->getDescription();
	if(this->boutonSelect>=0)
	{
		nom = this->boutons[this->boutonSelect]->getNom();
		desc = this->boutons[this->boutonSelect]->getDescription();
	}
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

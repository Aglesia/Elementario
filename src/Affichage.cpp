#include "Affichage.h"

Affichage::Affichage(std::string name)
{
	// On crée la fenêtre
	this->pWindow = SDL_CreateWindow(name.c_str(),
									 SDL_WINDOWPOS_UNDEFINED,
									 SDL_WINDOWPOS_UNDEFINED,
									 TAILLE_FENETRE_X,
									 TAILLE_FENETRE_Y,
									 SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
	if(this->pWindow)
		SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Ouverture de la fenêtre\n");
	else
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur de création de la fenêtre : %s\n",SDL_GetError());
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	// On crée le renderer
	this->renderer = SDL_CreateRenderer(this->pWindow, -1, SDL_RENDERER_PRESENTVSYNC|SDL_RENDERER_ACCELERATED);
	if(this->renderer)
	{
		SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Création du renderer\n");
		SDL_SetRenderDrawBlendMode(this->renderer, SDL_BLENDMODE_BLEND);
	}
	else
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur de création du renderer : %s\n",SDL_GetError());

	// On initialise les ticks
	this->dernierTempsTick = SDL_GetTicks();
}

Affichage::~Affichage() // TODO : vider toutes les textures
{
	this->lock.lock();

	// On ferme les polices
	if(this->policeChargement)
		TTF_CloseFont(this->policeChargement);

	// On détruit toutes les textures
	if(this->fond)
		SDL_DestroyTexture(this->fond);
	if(this->gifChargement)
		SDL_DestroyTexture(this->gifChargement);
	if(this->barreChargement)
		SDL_DestroyTexture(this->barreChargement);

	// On détruit le renderer
	if(this->renderer)
		SDL_DestroyRenderer(this->renderer);
	// On détruit la fenêtre
	if(this->pWindow)
		SDL_DestroyWindow(this->pWindow);
	if(this->icone)
		SDL_FreeSurface(this->icone);
	this->lock.unlock();
}

/**
 * Active ou désactive le mode plein écran de la fenêtre
 * Valeur par défaut : activer le mode plein écran (true)
 */
void Affichage::modePleinEcran(bool pleinEcran, int* x, int* y)
{
	this->lock.lock();
	// On modifie le mode plein écran si la fenêtre est bien créée
	if(this->pWindow)
	{
		if(SDL_SetWindowFullscreen(this->pWindow, (pleinEcran)?SDL_WINDOW_FULLSCREEN_DESKTOP:0))
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de changer l'état de la fenêtre : %s\n", SDL_GetError());
		this->pleinEcran = pleinEcran;
	}
	// Si on a demandé la taille
	if(x != nullptr && y != nullptr)
		SDL_GetWindowSize(this->pWindow, x, y);
	this->lock.unlock();
}

/**
 * Met à jour l'affichage de la fenêtre, appel bloquant à cause du VSync
 */
void Affichage::update()
{
	this->lock.lock();

	if(SDL_RenderClear(this->renderer))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de mettre à jour l'écran : %s\n", SDL_GetError());
	}

	// Si le fond est chargé, on l'affiche
	if(this->fond != nullptr)
	{
		// On calcul la taille
		SDL_Rect pos;
		SDL_QueryTexture(this->fond, NULL, NULL, &(pos.w), &(pos.h));
		SDL_GetWindowSize(this->pWindow, &(pos.x), &(pos.y));

		// Selon le rapport, on tronque d'un côté
		if((float)((float)pos.w/(float)pos.h) < (float)((float)pos.x/(float)pos.y))
		{
			// On tronque le haut/bas
			int taille = pos.w*((float)pos.y/(float)pos.x);
			pos.x = 0;
			pos.y = (pos.h - taille)/2;
			pos.h = taille;
		}
		else if((float)((float)pos.w/(float)pos.h) > (float)((float)pos.x/(float)pos.y))
		{
			// On tronque les côtés
			int taille = pos.h*((float)pos.x/(float)pos.y);
			pos.x = (pos.w - taille)/2;
			pos.y = 0;
			pos.w = taille;
		}
		else
		{
			pos.x = 0;
			pos.y = 0;
		}
		SDL_RenderCopy(this->renderer, this->fond, &pos, nullptr);
	}

	// Selon le mode actuel, on appelle la bonne méthode
	switch(this->modeAffichage){
		case MODE_AFFICHAGE_CHARGEMENT:
			this->afficherEcranChargement();
			break;

		case MODE_AFFICHAGE_CONFIG_TOUCHES1:
			this->afficherMenuConfigTouches1();
			this->afficherInfoConfigurationTouches1();
			break;

		case MODE_AFFICHAGE_CONFIG_TOUCHES2:
			{
				int xRef;
				int yRef;
				SDL_GetWindowSize(this->pWindow, &xRef, &yRef);
				xRef = ((xRef*2)/3) - (TAILLE_ICONE_CONFIG_TOUCHE*(1.2+NIVEAU_ZOOM_BOUTON_ACTIF));
				this->afficherMenuConfigTouches1(-1, xRef);
				this->afficherMenuConfigTouches2();
				this->afficherInfoConfigurationTouches1();
			}
			break;

		case MODE_AFFICHAGE_CONFIG_TOUCHES3:
			{
				int xRef;
				int yRef;
				SDL_GetWindowSize(this->pWindow, &xRef, &yRef);
				xRef = ((xRef*2)/3) - (TAILLE_ICONE_CONFIG_TOUCHE*(1.2+NIVEAU_ZOOM_BOUTON_ACTIF));
				this->afficherMenuConfigTouches2(-1, xRef);
				xRef -= (TAILLE_ICONE_CONFIG_TOUCHE*(1.2+NIVEAU_ZOOM_BOUTON_ACTIF));
				this->afficherMenuConfigTouches1(-1, xRef);
				this->afficherMenuConfigTouches3();
				this->afficherInfoConfigurationTouches1();
			}
			break;

		case MODE_AFFICHAGE_CONFIG_TOUCHES4:
			{
				int xRef;
				int yRef;
				SDL_GetWindowSize(this->pWindow, &xRef, &yRef);
				xRef = ((xRef*2)/3) - (TAILLE_ICONE_CONFIG_TOUCHE*(1.2+NIVEAU_ZOOM_BOUTON_ACTIF));
				this->afficherMenuConfigTouches3(-1, -1, xRef);
				xRef -= (TAILLE_ICONE_CONFIG_TOUCHE*(1.2+NIVEAU_ZOOM_BOUTON_ACTIF));
				this->afficherMenuConfigTouches2(-1, xRef);
				xRef -= (TAILLE_ICONE_CONFIG_TOUCHE*(1.2+NIVEAU_ZOOM_BOUTON_ACTIF));
				this->afficherMenuConfigTouches1(-1, xRef);
				this->afficherMenuConfigTouches4();
				this->afficherInfoConfigurationTouches2();
			}
			break;

		case MODE_TRANSITION_CONFIG_TOUCHES12:
			{
				int nbT = nbTicks(this->nbTicksDebutTransition);
				// Si c'est la fin de la transition
				if(nbT>=NB_TICKS_DEPLACEMENT_ECRAN)
				{
					nbT = NB_TICKS_DEPLACEMENT_ECRAN;
					this->modeAffichage = MODE_AFFICHAGE_CONFIG_TOUCHES2;
				}

				// On décale de x pixels
				int xRef;
				int yRef;
				SDL_GetWindowSize(this->pWindow, &xRef, &yRef);
				xRef = ((xRef*2)/3)-(((TAILLE_ICONE_CONFIG_TOUCHE*(1.2+NIVEAU_ZOOM_BOUTON_ACTIF))*nbT)/NB_TICKS_DEPLACEMENT_ECRAN);

				// On opacifie de x %
				int opacite = (255*nbT)/NB_TICKS_DEPLACEMENT_ECRAN;

				// On affiche
				this->afficherMenuConfigTouches1(-1, xRef);
				this->afficherMenuConfigTouches2(-1, -1, opacite);
				this->afficherInfoConfigurationTouches1();
			}
			break;

		case MODE_TRANSITION_CONFIG_TOUCHES21:
			{
				int nbT = nbTicks(this->nbTicksDebutTransition);
				// Si c'est la fin de la transition
				if(nbT>=NB_TICKS_DEPLACEMENT_ECRAN)
				{
					nbT = NB_TICKS_DEPLACEMENT_ECRAN;
					this->modeAffichage = MODE_AFFICHAGE_CONFIG_TOUCHES1;
				}

				// On décale de x pixels
				int xRef;
				int yRef;
				SDL_GetWindowSize(this->pWindow, &xRef, &yRef);
				xRef = ((xRef*2)/3)-(((TAILLE_ICONE_CONFIG_TOUCHE*(1.2+NIVEAU_ZOOM_BOUTON_ACTIF))*(NB_TICKS_DEPLACEMENT_ECRAN-nbT))/NB_TICKS_DEPLACEMENT_ECRAN);

				// On opacifie de x %
				int opacite = (255*(NB_TICKS_DEPLACEMENT_ECRAN-nbT))/NB_TICKS_DEPLACEMENT_ECRAN;

				// On affiche
				this->afficherMenuConfigTouches1(-1, xRef);
				this->afficherMenuConfigTouches2(-1, -1, opacite);
				this->afficherInfoConfigurationTouches1();
			}
			break;

		case MODE_TRANSITION_CONFIG_TOUCHES23:
			{
				int nbT = nbTicks(this->nbTicksDebutTransition);
				// Si c'est la fin de la transition
				if(nbT>=NB_TICKS_DEPLACEMENT_ECRAN)
				{
					nbT = NB_TICKS_DEPLACEMENT_ECRAN;
					this->modeAffichage = MODE_AFFICHAGE_CONFIG_TOUCHES3;
				}

				// On décale de x pixels
				int xRef;
				int yRef;
				SDL_GetWindowSize(this->pWindow, &xRef, &yRef);
				xRef = ((xRef*2)/3)-(((TAILLE_ICONE_CONFIG_TOUCHE*(1.2+NIVEAU_ZOOM_BOUTON_ACTIF))*nbT)/NB_TICKS_DEPLACEMENT_ECRAN);

				// On opacifie de x %
				int opacite = (255*nbT)/NB_TICKS_DEPLACEMENT_ECRAN;

				// On affiche
				this->afficherMenuConfigTouches2(-1, xRef);
				xRef -= (TAILLE_ICONE_CONFIG_TOUCHE*(1.2+NIVEAU_ZOOM_BOUTON_ACTIF));
				this->afficherMenuConfigTouches1(-1, xRef);
				this->afficherMenuConfigTouches3(-1, -1, -1, opacite);
				this->afficherInfoConfigurationTouches1();
			}
			break;

		case MODE_TRANSITION_CONFIG_TOUCHES32:
			{
				int nbT = nbTicks(this->nbTicksDebutTransition);
				// Si c'est la fin de la transition
				if(nbT>=NB_TICKS_DEPLACEMENT_ECRAN)
				{
					nbT = NB_TICKS_DEPLACEMENT_ECRAN;
					this->modeAffichage = MODE_AFFICHAGE_CONFIG_TOUCHES2;
				}

				// On décale de x pixels
				int xRef;
				int yRef;
				SDL_GetWindowSize(this->pWindow, &xRef, &yRef);
				xRef = ((xRef*2)/3)-(((TAILLE_ICONE_CONFIG_TOUCHE*(1.2+NIVEAU_ZOOM_BOUTON_ACTIF))*(NB_TICKS_DEPLACEMENT_ECRAN-nbT))/NB_TICKS_DEPLACEMENT_ECRAN);

				// On opacifie de x %
				int opacite = (255*(NB_TICKS_DEPLACEMENT_ECRAN-nbT))/NB_TICKS_DEPLACEMENT_ECRAN;

				// On affiche
				this->afficherMenuConfigTouches2(-1, xRef);
				xRef -= (TAILLE_ICONE_CONFIG_TOUCHE*(1.2+NIVEAU_ZOOM_BOUTON_ACTIF));
				this->afficherMenuConfigTouches1(-1, xRef);
				this->afficherMenuConfigTouches3(-1, -1, -1, opacite);
				this->afficherInfoConfigurationTouches1();
			}
			break;

		case MODE_TRANSITION_CONFIG_TOUCHES34:
			{
				int nbT = nbTicks(this->nbTicksDebutTransition);
				// Si c'est la fin de la transition
				if(nbT>=NB_TICKS_DEPLACEMENT_ECRAN)
				{
					nbT = NB_TICKS_DEPLACEMENT_ECRAN;
					this->modeAffichage = MODE_AFFICHAGE_CONFIG_TOUCHES4;
				}

				// On décale de x pixels
				int xRef;
				int yRef;
				SDL_GetWindowSize(this->pWindow, &xRef, &yRef);
				xRef = ((xRef*2)/3)-(((TAILLE_ICONE_CONFIG_TOUCHE*(1.2+NIVEAU_ZOOM_BOUTON_ACTIF))*nbT)/NB_TICKS_DEPLACEMENT_ECRAN);

				// On opacifie de x %
				int opacite = (255*nbT)/NB_TICKS_DEPLACEMENT_ECRAN;

				// On affiche
				this->afficherMenuConfigTouches3(-1, -1, xRef);
				xRef -= (TAILLE_ICONE_CONFIG_TOUCHE*(1.2+NIVEAU_ZOOM_BOUTON_ACTIF));
				this->afficherMenuConfigTouches2(-1, xRef);
				xRef -= (TAILLE_ICONE_CONFIG_TOUCHE*(1.2+NIVEAU_ZOOM_BOUTON_ACTIF));
				this->afficherMenuConfigTouches1(-1, xRef);
				this->afficherMenuConfigTouches4(nullptr, -1, -1, opacite);
				this->afficherInfoConfigurationTouches2();
			}
			break;

		case MODE_TRANSITION_CONFIG_TOUCHES43:
			{
				int nbT = nbTicks(this->nbTicksDebutTransition);
				// Si c'est la fin de la transition
				if(nbT>=NB_TICKS_DEPLACEMENT_ECRAN)
				{
					nbT = NB_TICKS_DEPLACEMENT_ECRAN;
					this->modeAffichage = MODE_AFFICHAGE_CONFIG_TOUCHES3;
				}

				// On décale de x pixels
				int xRef;
				int yRef;
				SDL_GetWindowSize(this->pWindow, &xRef, &yRef);
				xRef = ((xRef*2)/3)-(((TAILLE_ICONE_CONFIG_TOUCHE*(1.2+NIVEAU_ZOOM_BOUTON_ACTIF))*(NB_TICKS_DEPLACEMENT_ECRAN-nbT))/NB_TICKS_DEPLACEMENT_ECRAN);

				// On opacifie de x %
				int opacite = (255*(NB_TICKS_DEPLACEMENT_ECRAN-nbT))/NB_TICKS_DEPLACEMENT_ECRAN;

				// On affiche
				this->afficherMenuConfigTouches3(-1, -1, xRef);
				xRef -= (TAILLE_ICONE_CONFIG_TOUCHE*(1.2+NIVEAU_ZOOM_BOUTON_ACTIF));
				this->afficherMenuConfigTouches2(-1, xRef);
				xRef -= (TAILLE_ICONE_CONFIG_TOUCHE*(1.2+NIVEAU_ZOOM_BOUTON_ACTIF));
				this->afficherMenuConfigTouches1(-1, xRef);
				this->afficherMenuConfigTouches4(nullptr, -1, -1, opacite);
				this->afficherInfoConfigurationTouches2();
			}
			break;

		default:
			break;
	}

	// Si la police est chargée
	if(this->policeChargement != nullptr)
	{
		// On crée le texte des fps
		SDL_Color couleurTexte = {COULEUR_TEXTE_CHARGEMENT_R, COULEUR_TEXTE_CHARGEMENT_G, COULEUR_TEXTE_CHARGEMENT_B};
		SDL_Surface* texte = TTF_RenderUTF8_Blended(this->policeChargement, "60", couleurTexte);
		if(texte != nullptr)
		{
			// On calcul sa position
			SDL_Rect r;
			SDL_GetWindowSize(this->pWindow, &(r.w), &(r.h));
			int h = r.h;
			int w = r.w;
			r.x = 5;
			r.y = h - texte->h - 5;
			r.h = texte->h;
			r.w = texte->w;
			SDL_Texture* texteT = SDL_CreateTextureFromSurface(this->renderer, texte);
			SDL_FreeSurface(texte);

			// On affiche le texte
			SDL_RenderCopy(this->renderer, texteT, nullptr, &r);
			SDL_DestroyTexture(texteT);
		}
	}

	// On rend l'image
	SDL_RenderPresent(this->renderer);
	this->lock.unlock();
}

int Affichage::init() // TODO : Ajouter tous les éléments à charger + image de fond + calculer % entre chaque chargement
{
	int error = 0;

	// On crée un fond noir pour l'écran de chargement
	SDL_Surface* fondC = IMG_Load(IMAGE_FOND_FILENAME);
	if(fondC)
	{
		this->lock.lock();
		this->fond = SDL_CreateTextureFromSurface(this->renderer, fondC);
		this->lock.unlock();
		SDL_FreeSurface(fondC);
	}
	else
	{
		error = 1;
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image de fond : %s\n", IMG_GetError());
	}

	// On crée la barre de chargement
	this->afficherEcranChargement(25, "Chargement...");
	fondC = SDL_CreateRGBSurface(0, 1, 1, 32, rmask, gmask, bmask, amask);
	SDL_FillRect(fondC, NULL, SDL_MapRGB(fondC->format, COULEUR_BARRE_CHARGEMENT_R, COULEUR_BARRE_CHARGEMENT_G, COULEUR_BARRE_CHARGEMENT_B));
	this->lock.lock();
	this->barreChargement = SDL_CreateTextureFromSurface(this->renderer, fondC);
	this->lock.unlock();
	SDL_FreeSurface(fondC);

	// On charge l'icône de l'appli (qui sera aussi le gif de chargement)
	this->afficherEcranChargement(50, "Chargement de l'icône du jeu");
	this->lock.lock();
	this->icone = IMG_Load(ICONE_FILENAME);
	this->lock.unlock();
	if(this->icone)
	{
		this->lock.lock();
		this->gifChargement = SDL_CreateTextureFromSurface(this->renderer, this->icone);
		this->lock.unlock();
		SDL_SetWindowIcon(this->pWindow, this->icone);
	}
	else
	{
		error = 1;
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'icône : %s\n", IMG_GetError());
	}

	// On charge les polices d'écriture
	this->afficherEcranChargement(75, "Chargement de la police d'écriture 01/02");
	this->lock.lock();
	this->policeChargement = TTF_OpenFont(POLICE_CHARGEMENT_FILENAME, POLICE_CHARGEMENT_TAILLE);
	this->lock.unlock();
	if(this->policeChargement == nullptr)
	{
		error = 2;
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger la police de chargement : %s\n", TTF_GetError());
	}
	this->afficherEcranChargement(75, "Chargement de la police d'écriture 02/02");
	this->lock.lock();
	this->policeMenu = TTF_OpenFont(POLICE_MENU_FILENAME, POLICE_MENU_TAILLE);
	this->policeTitreMenu = TTF_OpenFont(POLICE_MENU_FILENAME, POLICE_MENU_TITRE_TAILLE);
	this->lock.unlock();
	if(this->policeMenu == nullptr)
	{
		error = 2;
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger la police de chargement : %s\n", TTF_GetError());
	}

	// On charge les icônes de configuration des touches
	for(int i=0; i<CONFIG_TOUCHES_NB_CATEGORIES; i++)
	{
		std::string temp = "Chargement de l'icône config_touches_1 (";
		temp += SSTR(i);
		temp += "/";
		temp += SSTR(CONFIG_TOUCHES_NB_CATEGORIES);
		temp += ")";
		this->afficherEcranChargement(60, temp.c_str());
		std::string temp2 = BOUTON_CONFIG_DIRNAME;
		temp2 += "categorie/";
		temp2 += SSTR(i);
		temp2 += ".";
		temp2 += IMAGE_FORMAT;
		fondC = IMG_Load(temp2.c_str());
		if(fondC)
		{
			this->lock.lock();
			Bouton* b = new Bouton(SDL_CreateTextureFromSurface(this->renderer, fondC), this->renderer, TAILLE_ICONE_CONFIG_TOUCHE);
			this->configTouches1Boutons.push_back(b);
			this->lock.unlock();
			SDL_FreeSurface(fondC);
		}
		else
		{
			error = 3;
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image : %s\n", IMG_GetError());
		}
	}
	// On charge les icônes de configuration des touches
	for(int i=0; i<CONFIG_TOUCHES_NB_TOUCHE_JEU; i++)
	{
		std::string temp = "Chargement de l'icône config_touches_2 (";
		temp += SSTR(i);
		temp += "/";
		temp += SSTR(CONFIG_TOUCHES_NB_TOUCHE_JEU);
		temp += ")";
		this->afficherEcranChargement(60, temp.c_str());
		std::string temp2 = BOUTON_CONFIG_DIRNAME;
		temp2 += "touche/";
		temp2 += SSTR(i);
		temp2 += ".";
		temp2 += IMAGE_FORMAT;
		fondC = IMG_Load(temp2.c_str());
		if(fondC)
		{
			this->lock.lock();
			Bouton* b = new Bouton(SDL_CreateTextureFromSurface(this->renderer, fondC), this->renderer, TAILLE_ICONE_CONFIG_TOUCHE);
			this->configTouches2Boutons.push_back(b);
			this->lock.unlock();
			SDL_FreeSurface(fondC);
		}
		else
		{
			error = 3;
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image : %s\n", IMG_GetError());
		}
	}

	// icône souris
	{
		std::string temp = "Chargement de l'icône config_touche_3 (1/2)";
		this->afficherEcranChargement(60, temp.c_str());
		std::string temp2 = BOUTON_CONFIG_DIRNAME;
		temp2 += "souris.";
		temp2 += IMAGE_FORMAT;
		fondC = IMG_Load(temp2.c_str());
		if(fondC)
		{
			this->lock.lock();
			Bouton* b = new Bouton(SDL_CreateTextureFromSurface(this->renderer, fondC), this->renderer, TAILLE_ICONE_CONFIG_TOUCHE);
			this->configTouches3Boutons.push_back(b);
			this->lock.unlock();
			SDL_FreeSurface(fondC);
		}
		else
		{
			error = 3;
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image : %s\n", IMG_GetError());
		}
	}

	// icône clavier
	{
		std::string temp = "Chargement de l'icône config_touche_3 (2/2)";
		this->afficherEcranChargement(60, temp.c_str());
		std::string temp2 = BOUTON_CONFIG_DIRNAME;
		temp2 += "clavier.";
		temp2 += IMAGE_FORMAT;
		fondC = IMG_Load(temp2.c_str());
		if(fondC)
		{
			this->lock.lock();
			Bouton* b = new Bouton(SDL_CreateTextureFromSurface(this->renderer, fondC), this->renderer, TAILLE_ICONE_CONFIG_TOUCHE);
			this->configTouches3Boutons.push_back(b);
			this->lock.unlock();
			SDL_FreeSurface(fondC);
		}
		else
		{
			error = 3;
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image : %s\n", IMG_GetError());
		}
	}

	// icône bouton
	{
		std::string temp2 = BOUTON_CONFIG_DIRNAME;
		temp2 += "bouton.";
		temp2 += IMAGE_FORMAT;
		SDL_Surface* fondC = IMG_Load(temp2.c_str());
		if(fondC)
		{
			this->configTouches4TextureBouton = SDL_CreateTextureFromSurface(this->renderer, fondC);
			SDL_FreeSurface(fondC);
		}
		else
		{
			error = 3;
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image : %s\n", IMG_GetError());
		}
	}

	// icône axe
	{
		std::string temp2 = BOUTON_CONFIG_DIRNAME;
		temp2 += "axe.";
		temp2 += IMAGE_FORMAT;
		SDL_Surface* fondC = IMG_Load(temp2.c_str());
		if(fondC)
		{
			this->configTouches4TextureAxe = SDL_CreateTextureFromSurface(this->renderer, fondC);
			SDL_FreeSurface(fondC);
		}
		else
		{
			error = 3;
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image : %s\n", IMG_GetError());
		}
	}

	// icône croix
	{
		std::string temp2 = BOUTON_CONFIG_DIRNAME;
		temp2 += "croix.";
		temp2 += IMAGE_FORMAT;
		SDL_Surface* fondC = IMG_Load(temp2.c_str());
		if(fondC)
		{
			this->configTouches4TextureCroix = SDL_CreateTextureFromSurface(this->renderer, fondC);
			SDL_FreeSurface(fondC);
		}
		else
		{
			error = 3;
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image : %s\n", IMG_GetError());
		}
	}

	// icône molette
	{
		std::string temp2 = BOUTON_CONFIG_DIRNAME;
		temp2 += "molette.";
		temp2 += IMAGE_FORMAT;
		SDL_Surface* fondC = IMG_Load(temp2.c_str());
		if(fondC)
		{
			this->configTouches4TextureMolette = SDL_CreateTextureFromSurface(this->renderer, fondC);
			SDL_FreeSurface(fondC);
		}
		else
		{
			error = 3;
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image : %s\n", IMG_GetError());
		}
	}

	// icône new
	{
		std::string temp2 = BOUTON_CONFIG_DIRNAME;
		temp2 += "new.";
		temp2 += IMAGE_FORMAT;
		SDL_Surface* fondC = IMG_Load(temp2.c_str());
		if(fondC)
		{
			this->configTouches4TextureNew = SDL_CreateTextureFromSurface(this->renderer, fondC);
			SDL_FreeSurface(fondC);
		}
		else
		{
			error = 3;
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image : %s\n", IMG_GetError());
		}
	}

	this->afficherEcranChargement(100, "Initialisation terminée");

	return error;
}

// Retourne le nombre de ticks écoulés depuis l'instant "t" ticks (valeur par défaut : 0 = lancement du programme)
unsigned int Affichage::nbTicks(unsigned int t)
{
	this->lockTick.lock();
	// On calcul le nouveau nombre de ticks
	while(SDL_GetTicks() - this->dernierTempsTick > TEMPS_TICKS)
	{
		this->dernierTempsTick += TEMPS_TICKS;
		this->nbT++;
	}

	// On retourne le nombre de ticks écoulés depuis la valeur indiquée
	int ret = this->nbT - t;
	this->lockTick.unlock();
	return ret;
}

/**
 * Ensemble des fonctions d'affichage
 */
void Affichage::setEcran(int ecran)
{
	this->lock.lock();
	unsigned int nbT = this->nbTicks();
	// On fait une animation selon la transition demandée
	if(this->modeAffichage == MODE_AFFICHAGE_CONFIG_TOUCHES1 && ecran == MODE_AFFICHAGE_CONFIG_TOUCHES2)
	{
		this->modeAffichage = MODE_TRANSITION_CONFIG_TOUCHES12;
		this->configTouches2noToucheJeu = this->configTouches2noToucheJeuD;
		this->nbTicksDebutTransition = nbTicks();
	}
	else if(this->modeAffichage == MODE_AFFICHAGE_CONFIG_TOUCHES2 && ecran == MODE_AFFICHAGE_CONFIG_TOUCHES1)
	{
		this->modeAffichage = MODE_TRANSITION_CONFIG_TOUCHES21;
		this->nbTicksDebutTransition = nbTicks();
	}
	else if(this->modeAffichage == MODE_AFFICHAGE_CONFIG_TOUCHES2 && ecran == MODE_AFFICHAGE_CONFIG_TOUCHES3)
	{
		this->modeAffichage = MODE_TRANSITION_CONFIG_TOUCHES23;
		this->nbTicksDebutTransition = nbTicks();
	}
	else if(this->modeAffichage == MODE_AFFICHAGE_CONFIG_TOUCHES3 && ecran == MODE_AFFICHAGE_CONFIG_TOUCHES2)
	{
		this->modeAffichage = MODE_TRANSITION_CONFIG_TOUCHES32;
		this->nbTicksDebutTransition = nbTicks();
	}
	else if(this->modeAffichage == MODE_AFFICHAGE_CONFIG_TOUCHES3 && ecran == MODE_AFFICHAGE_CONFIG_TOUCHES4)
	{
		this->modeAffichage = MODE_TRANSITION_CONFIG_TOUCHES34;
		this->nbTicksDebutTransition = nbTicks();
	}
	else if(this->modeAffichage == MODE_AFFICHAGE_CONFIG_TOUCHES4 && ecran == MODE_AFFICHAGE_CONFIG_TOUCHES3)
	{
		this->modeAffichage = MODE_TRANSITION_CONFIG_TOUCHES43;
		this->nbTicksDebutTransition = nbTicks();
	}
	else
		this->modeAffichage = ecran;
	this->lock.unlock();
}

void Affichage::afficherEcranChargement(int pourcentage, std::string message)
{
	// Si on doit juste changer les paramètres
	if(pourcentage != -1)
	{
		// On change les attributs en tenant compte des mutex
		this->lock.lock();
		this->modeAffichage = MODE_AFFICHAGE_CHARGEMENT;
		if(pourcentage >= 0 && pourcentage <= 100)
			this->pourcentage = pourcentage;
		this->texteChargement = message;
		this->lock.unlock();
	}
	else // Appelé depuis le thread, normalement
	{
		// Si la barre de chargement est chargée, on l'affiche
		if(this->barreChargement != nullptr)
		{
			// On calcul sa position
			SDL_Rect r;
			SDL_GetWindowSize(this->pWindow, &(r.w), &(r.h));
			int h = r.h;
			r.h /= 30;
			r.w *= this->pourcentage;
			r.w /= 100;
			r.x = 0;
			r.y = h - r.h;
			// On l'affiche
			SDL_RenderCopy(this->renderer, this->barreChargement, nullptr, &r);
		}

		// Si l'image est chargée
		if(this->gifChargement != nullptr)
		{
			// On calcul sa position
			SDL_Rect r;
			SDL_GetWindowSize(this->pWindow, &(r.w), &(r.h));
			int h = r.h;
			int w = r.w;
			r.x = (w/2) - (TAILLE_ICONE_CHARGEMENT/2);
			r.y = (h/2) - (TAILLE_ICONE_CHARGEMENT/2);
			r.h = TAILLE_ICONE_CHARGEMENT;
			r.w = TAILLE_ICONE_CHARGEMENT;

			// On l'affiche
			SDL_RenderCopyEx(this->renderer, this->gifChargement, nullptr, &r, this->rotation+=this->nbTicks(this->dernierTickChargement), nullptr, SDL_FLIP_NONE);
			if(this->rotation>360)
				this->rotation = 0;
			this->dernierTickChargement = this->nbT;
		}

		// Si la police est chargée
		if(this->policeChargement != nullptr)
		{
			// On crée le texte
			SDL_Color couleurTexte = {COULEUR_TEXTE_CHARGEMENT_R, COULEUR_TEXTE_CHARGEMENT_G, COULEUR_TEXTE_CHARGEMENT_B, 255};
			SDL_Surface* texte = TTF_RenderUTF8_Blended(this->policeChargement, this->texteChargement.c_str(), couleurTexte);
			if(texte != nullptr)
			{
				// On calcul sa position
				SDL_Rect r;
				SDL_GetWindowSize(this->pWindow, &(r.w), &(r.h));
				int h = r.h;
				int w = r.w;
				r.x = (w/2) - (texte->w/2);
				r.y = h - (((h/2) - (TAILLE_ICONE_CHARGEMENT/2))/2) - (texte->h/2);
				r.h = texte->h;
				r.w = texte->w;
				SDL_Texture* texteT = SDL_CreateTextureFromSurface(this->renderer, texte);
				SDL_FreeSurface(texte);

				// On modélise l'image de fond de texte (un hexagone allongé)
				int ymin = r.y - (EPAISSEUR_HEXAGONE_CHARGEMENT*2);
				int ymax = r.y + r.h + (EPAISSEUR_HEXAGONE_CHARGEMENT*2);
				int ymoy = (ymin+ymax)/2;
				double xmin = r.x - EPAISSEUR_HEXAGONE_CHARGEMENT;
				double xmax = r.x + r.w + EPAISSEUR_HEXAGONE_CHARGEMENT;
				// On dessine
				for(int y=ymin; y<ymax; y++)
				{
					// On dessine 3 lignes : ligne + fond + ligne
					SDL_SetRenderDrawColor(this->renderer, COULEUR_HEXAGONE_CHARGEMENT_R, COULEUR_HEXAGONE_CHARGEMENT_G, COULEUR_HEXAGONE_CHARGEMENT_B, 255);
					if(y<(r.y - EPAISSEUR_HEXAGONE_CHARGEMENT) || (y>(r.y + r.h + EPAISSEUR_HEXAGONE_CHARGEMENT)))
						SDL_RenderDrawLine(this->renderer, xmin, y, xmax, y);
					else
					{
						SDL_RenderDrawLine(this->renderer, xmin, y, xmin+EPAISSEUR_HEXAGONE_CHARGEMENT, y);
						SDL_SetRenderDrawColor(this->renderer, COULEUR_HEXAGONE_CHARGEMENT_R, COULEUR_HEXAGONE_CHARGEMENT_G, COULEUR_HEXAGONE_CHARGEMENT_B, OPACITE_HEXAGONE_CHARGEMENT);
						SDL_RenderDrawLine(this->renderer, xmin+EPAISSEUR_HEXAGONE_CHARGEMENT, y, xmax-EPAISSEUR_HEXAGONE_CHARGEMENT, y);
						SDL_SetRenderDrawColor(this->renderer, COULEUR_HEXAGONE_CHARGEMENT_R, COULEUR_HEXAGONE_CHARGEMENT_G, COULEUR_HEXAGONE_CHARGEMENT_B, 255);
						SDL_RenderDrawLine(this->renderer, xmax-EPAISSEUR_HEXAGONE_CHARGEMENT, y, xmax, y);
					}

					// On écarte/rapproche les X
					if(y<ymoy)
					{
						xmin-=0.57735026918963;
						xmax+=0.57735026918963;
					}
					else
					{
						xmin+=0.57735026918963;
						xmax-=0.57735026918963;
					}
				}
				// On affiche le texte
				SDL_RenderCopy(this->renderer, texteT, nullptr, &r);
				SDL_DestroyTexture(texteT);
			}
		}
	}
}

void Affichage::afficherMenuConfigTouches1(int categorie, int xReference, int opacite)
{
	// Si on change le menu
	if(categorie > -1)
	{
		this->lock.lock();
		this->configTouches1Categorie = this->configTouches1CategorieD;
		this->configTouches1CategorieD = categorie;
		this->configTouchesNbTicksAnimation = this->nbTicks();
		this->lock.unlock();
	}
	// On dessine
	else
	{
		// On calcul la position des boutons
		int xRef = 0;
		int yRef = 0;
		SDL_GetWindowSize(this->pWindow, &xRef, &yRef);
		xRef = (xRef*2)/3; // Centre droite
		if(xReference != -1)
			xRef = xReference;
		yRef/=2; // Centre
		yRef-=(this->configTouches1Categorie*TAILLE_ICONE_CONFIG_TOUCHE*(1+(NIVEAU_ZOOM_BOUTON_ACTIF/2))); // Ligne actuelle
		if(this->configTouches1Categorie != this->configTouches1CategorieD)
		{
			// On déplace selon le temps
			if(this->configTouches1Categorie < this->configTouches1CategorieD)
				yRef -= TAILLE_ICONE_CONFIG_TOUCHE*(1+(NIVEAU_ZOOM_BOUTON_ACTIF/2))*(this->configTouches1CategorieD-this->configTouches1Categorie)*(this->nbTicks() - this->configTouchesNbTicksAnimation)/NB_TICKS_DEPLACEMENT_BOUTON;
			else if(this->configTouches1Categorie > this->configTouches1CategorieD)
				yRef += TAILLE_ICONE_CONFIG_TOUCHE*(1+(NIVEAU_ZOOM_BOUTON_ACTIF/2))*(this->configTouches1Categorie-this->configTouches1CategorieD)*(this->nbTicks() - this->configTouchesNbTicksAnimation)/NB_TICKS_DEPLACEMENT_BOUTON;

			// Si on a fini l'animation on l'arrête
			if(this->nbTicks() - this->configTouchesNbTicksAnimation >= NB_TICKS_DEPLACEMENT_BOUTON)
			{
				this->configTouches1Categorie = this->configTouches1CategorieD;
			}
		}

		SDL_Rect pos;
		pos.x = xRef;
		pos.y = yRef;
		for(int i=0; i<CONFIG_TOUCHES_NB_CATEGORIES; i++)
		{
			this->configTouches1Boutons[i]->afficher(&pos, this->nbTicks(), (this->configTouches1CategorieD == i), opacite);
			pos.y += TAILLE_ICONE_CONFIG_TOUCHE*(1+(NIVEAU_ZOOM_BOUTON_ACTIF/2));
		}
	}
}

void Affichage::afficherMenuConfigTouches2(int noToucheJeu, int xReference, int opacite)
{
	// Si on change le menu
	if(noToucheJeu > -1)
	{
		this->lock.lock();
		this->configTouches1Categorie = this->configTouches1CategorieD;
		this->configTouches2noToucheJeu = this->configTouches2noToucheJeuD;
		this->configTouches2noToucheJeuD = noToucheJeu;
		this->configTouchesNbTicksAnimation = this->nbTicks();
		this->lock.unlock();
	}
	// On dessine
	else
	{
		// On calcul la position des boutons
		int xRef = 0;
		int yRef = 0;
		SDL_GetWindowSize(this->pWindow, &xRef, &yRef);
		xRef = (xRef*2)/3; // Centre droite
		if(xReference != -1)
			xRef = xReference;
		yRef/=2; // Centre
		yRef-=(this->configTouches2noToucheJeu*TAILLE_ICONE_CONFIG_TOUCHE*(1+(NIVEAU_ZOOM_BOUTON_ACTIF/2))); // Ligne actuelle
		if(this->configTouches2noToucheJeu != this->configTouches2noToucheJeuD)
		{
			// On déplace selon le temps
			if(this->configTouches2noToucheJeu < this->configTouches2noToucheJeuD)
				yRef -= TAILLE_ICONE_CONFIG_TOUCHE*(1+(NIVEAU_ZOOM_BOUTON_ACTIF/2))*(this->configTouches2noToucheJeuD-this->configTouches2noToucheJeu)*(this->nbTicks() - this->configTouchesNbTicksAnimation)/NB_TICKS_DEPLACEMENT_BOUTON;
			else if(this->configTouches2noToucheJeu > this->configTouches2noToucheJeuD)
				yRef += TAILLE_ICONE_CONFIG_TOUCHE*(1+(NIVEAU_ZOOM_BOUTON_ACTIF/2))*(this->configTouches2noToucheJeu-this->configTouches2noToucheJeuD)*(this->nbTicks() - this->configTouchesNbTicksAnimation)/NB_TICKS_DEPLACEMENT_BOUTON;

			// Si on a fini l'animation on l'arrête
			if(this->nbTicks() - this->configTouchesNbTicksAnimation >= NB_TICKS_DEPLACEMENT_BOUTON)
			{
				this->configTouches2noToucheJeu = this->configTouches2noToucheJeuD;
			}
		}

		// Boutons ToucheJeu
		SDL_Rect pos;
		pos.x = xRef;
		pos.y = yRef;
		for(int i=0; i<CONFIG_TOUCHES_NB_TOUCHE_JEU; i++)
		{
			this->configTouches2Boutons[i]->afficher(&pos, this->nbTicks(), (this->configTouches2noToucheJeuD == i), opacite);
			pos.y += TAILLE_ICONE_CONFIG_TOUCHE*(1+(NIVEAU_ZOOM_BOUTON_ACTIF/2));
		}
	}
}

void Affichage::afficherMenuConfigTouches3(int nbController, int noController, int xReference, int opacite)
{
	// Si on change le menu
	if(nbController > -1)
	{
		this->lock.lock();
		noController += 2;
		this->configTouches2noToucheJeu = this->configTouches2noToucheJeuD;
		this->configTouches3nbController = nbController;
		this->configTouches3noController = this->configTouches3noControllerD;
		this->configTouches3noControllerD = noController;
		this->configTouchesNbTicksAnimation = this->nbTicks();
		// Si on doit rajouter des boutons
		for(int i=configTouches3Boutons.size(); i<nbController+2; i++)
		{
			std::string temp2 = BOUTON_CONFIG_DIRNAME;
			temp2 += "manette.";
			temp2 += IMAGE_FORMAT;
			SDL_Surface* fondC = IMG_Load(temp2.c_str());
			if(fondC)
			{
				this->configTouches3Boutons.push_back(new Bouton(SDL_CreateTextureFromSurface(this->renderer, fondC), this->renderer, TAILLE_ICONE_CONFIG_TOUCHE));
				SDL_FreeSurface(fondC);
			}
			else
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'icône : %s\n", IMG_GetError());
		}
		this->lock.unlock();
	}
	// On dessine
	else
	{
		// On calcul la position des boutons
		int xRef = 0;
		int yRef = 0;
		SDL_GetWindowSize(this->pWindow, &xRef, &yRef);
		xRef = (xRef*2)/3; // Centre droite
		if(xReference != -1)
			xRef = xReference;
		yRef/=2; // Centre
		yRef-=(this->configTouches3noController*TAILLE_ICONE_CONFIG_TOUCHE*(1+(NIVEAU_ZOOM_BOUTON_ACTIF/2))); // Ligne actuelle
		if(this->configTouches3noController != this->configTouches3noControllerD)
		{
			// On déplace selon le temps
			if(this->configTouches3noController < this->configTouches3noControllerD)
				yRef -= TAILLE_ICONE_CONFIG_TOUCHE*(1+(NIVEAU_ZOOM_BOUTON_ACTIF/2))*(this->configTouches3noControllerD-this->configTouches3noController)*(this->nbTicks() - this->configTouchesNbTicksAnimation)/NB_TICKS_DEPLACEMENT_BOUTON;
			else if(this->configTouches3noController > this->configTouches3noControllerD)
				yRef += TAILLE_ICONE_CONFIG_TOUCHE*(1+(NIVEAU_ZOOM_BOUTON_ACTIF/2))*(this->configTouches3noController-this->configTouches3noControllerD)*(this->nbTicks() - this->configTouchesNbTicksAnimation)/NB_TICKS_DEPLACEMENT_BOUTON;

			// Si on a fini l'animation on l'arrête
			if(this->nbTicks() - this->configTouchesNbTicksAnimation >= NB_TICKS_DEPLACEMENT_BOUTON)
			{
				this->configTouches3noController = this->configTouches3noControllerD;
			}
		}

		// Boutons ToucheJeu
		SDL_Rect pos;
		pos.x = xRef;
		pos.y = yRef;
		for(int i=0; i<this->configTouches3nbController+2 && i<this->configTouches3Boutons.size(); i++)
		{
			this->configTouches3Boutons[i]->afficher(&pos, this->nbTicks(), (this->configTouches3noControllerD == i), opacite);
			pos.y += TAILLE_ICONE_CONFIG_TOUCHE*(1+(NIVEAU_ZOOM_BOUTON_ACTIF/2));
		}
	}
}

void Affichage::afficherMenuConfigTouches4(std::vector<Touche*>* t, int noTouche, int xReference, int opacite)
{
	// Si on change le menu
	if(t == (std::vector<Touche*>*)-1)
	{
		this->configTouches4noController = nullptr;
	}
	else if(t != nullptr)
	{
		this->lock.lock();
		this->configTouches3noController = this->configTouches3noControllerD;
		this->configTouches4noTouche = this->configTouches4noToucheD;
		this->configTouches4noToucheD = noTouche;
		this->configTouchesNbTicksAnimation = this->nbTicks();
		// On liste les boutons
		if(t != this->configTouches4noController)
		{
			this->configTouches4noController = t;
			// On vide les boutons actuels
			for(int i=0; i<this->configTouches4Boutons.size(); i++)
				delete this->configTouches4Boutons[i];
			this->configTouches4Boutons.clear();
			// On recrée les boutons
			for(int i=0; i<t->size(); i++)
			{
				SDL_Texture* tex = this->configTouches4TextureCroix;
				switch((*t)[i]->getType())
				{
					case TYPE_TOUCHE_MOLETTE:
						tex = this->configTouches4TextureMolette;
						break;

					case TYPE_TOUCHE_BOUTON:
						tex = this->configTouches4TextureBouton;
						break;

					case TYPE_TOUCHE_JOYSTIC:
						tex = this->configTouches4TextureAxe;
						break;

					default:
						break;
				}
				this->configTouches4Boutons.push_back(new Bouton(tex, this->renderer, TAILLE_ICONE_CONFIG_TOUCHE));
			}
			this->configTouches4Boutons.push_back(new Bouton(this->configTouches4TextureNew, this->renderer, TAILLE_ICONE_CONFIG_TOUCHE));
		}
		this->lock.unlock();
	}
	// On dessine
	else
	{
		// On calcul la position des boutons
		int xRef = 0;
		int yRef = 0;
		SDL_GetWindowSize(this->pWindow, &xRef, &yRef);
		xRef = (xRef*2)/3; // Centre droite
		if(xReference != -1)
			xRef = xReference;
		yRef/=2; // Centre
		yRef-=(this->configTouches4noTouche*TAILLE_ICONE_CONFIG_TOUCHE*(1+(NIVEAU_ZOOM_BOUTON_ACTIF/2))); // Ligne actuelle
		if(this->configTouches4noTouche != this->configTouches4noToucheD)
		{
			// On déplace selon le temps
			if(this->configTouches4noTouche < this->configTouches4noToucheD)
				yRef -= TAILLE_ICONE_CONFIG_TOUCHE*(1+(NIVEAU_ZOOM_BOUTON_ACTIF/2))*(this->configTouches4noToucheD-this->configTouches4noTouche)*(this->nbTicks() - this->configTouchesNbTicksAnimation)/NB_TICKS_DEPLACEMENT_BOUTON;
			else if(this->configTouches4noTouche > this->configTouches4noToucheD)
				yRef += TAILLE_ICONE_CONFIG_TOUCHE*(1+(NIVEAU_ZOOM_BOUTON_ACTIF/2))*(this->configTouches4noTouche-this->configTouches4noToucheD)*(this->nbTicks() - this->configTouchesNbTicksAnimation)/NB_TICKS_DEPLACEMENT_BOUTON;

			// Si on a fini l'animation on l'arrête
			if(this->nbTicks() - this->configTouchesNbTicksAnimation >= NB_TICKS_DEPLACEMENT_BOUTON)
			{
				this->configTouches4noTouche = this->configTouches4noToucheD;
			}
		}

		// Boutons manette
		SDL_Rect pos;
		pos.x = xRef;
		pos.y = yRef;
		for(int i=0; i<this->configTouches4Boutons.size(); i++)
		{
			this->configTouches4Boutons[i]->afficher(&pos, this->nbTicks(), (this->configTouches4noToucheD == i), opacite);
			pos.y += TAILLE_ICONE_CONFIG_TOUCHE*(1+(NIVEAU_ZOOM_BOUTON_ACTIF/2));
		}
	}
}

void Affichage::afficherInfoConfigurationTouches1(std::string nom, std::string description)
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
}

void Affichage::afficherInfoConfigurationTouches2(Touche* t)
{
	if(t != nullptr)
	{
		this->lock.lock();
		this->configTouchesManette = t;
		this->lock.unlock();
	}
	else if(this->configTouchesManette != nullptr)
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
		std::string configTouchesNom = this->configTouchesManette->nom;
		std::string configTouchesDescription = "";
		if(configTouchesNom != "Nouvelle touche")
		{
			configTouchesDescription = "- Appuyé : ";
			configTouchesDescription += (this->configTouchesManette->isPressed())?"Oui":"Non";
			configTouchesDescription += "\n- Valeur brut de l'axe : ";
			configTouchesDescription += SSTR(this->configTouchesManette->getValBrutAxe());
			configTouchesDescription += "\n- Valeur minimum : ";
			configTouchesDescription += SSTR(this->configTouchesManette->getValeurMin());
			configTouchesDescription += "\n- Valeur maximum : ";
			configTouchesDescription += SSTR(this->configTouchesManette->getValeurMax());
			configTouchesDescription += "\n- Valeur lorsque cliqué : ";
			configTouchesDescription += SSTR(this->configTouchesManette->getValeurClic());
			configTouchesDescription += "\n- Valeur finale de l'axe : ";
			configTouchesDescription += SSTR(this->configTouchesManette->getValAxe());
		}

		// On dessine le titre
		SDL_Color couleurTexte = {COULEUR_TEXTE_MENU_R, COULEUR_TEXTE_MENU_G, COULEUR_TEXTE_MENU_B};
		SDL_Surface* titreSurface = TTF_RenderUTF8_Solid(this->policeTitreMenu, configTouchesNom.c_str(), couleurTexte);
		if(titreSurface == nullptr)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de créer le texte : %s\n", TTF_GetError());
			return;
		}

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
		std::istringstream split(configTouchesDescription);
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
}

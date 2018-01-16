#include "Affichage.h"

Affichage::Affichage(std::string name, std::string path, ToucheJeu** touchesJeu)
{
	this->path = path;
	this->touchesJeu = touchesJeu;

	// On crée la fenêtre
	this->pWindow = SDL_CreateWindow(name.c_str(),
									 SDL_WINDOWPOS_UNDEFINED,
									 SDL_WINDOWPOS_UNDEFINED,
									 TAILLE_FENETRE_X,
									 TAILLE_FENETRE_Y,
									 SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_ALLOW_HIGHDPI);
	if(this->pWindow)
		SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Ouverture de la fenêtre\n");
	else
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur de création de la fenêtre : %s\n",SDL_GetError());
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	// On crée le renderer
	this->renderer = SDL_CreateRenderer(this->pWindow, -1, SDL_RENDERER_PRESENTVSYNC|SDL_RENDERER_ACCELERATED|SDL_RENDERER_TARGETTEXTURE);
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

Affichage::~Affichage() // TODO : vider toutes les textures/surfaces
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

	// On détruit toutes les surfaces
	if(this->icone)
		SDL_FreeSurface(this->icone);
	if(this->fond_surface)
		SDL_FreeSurface(this->fond_surface);
	if(this->barreChargement_surface)
		SDL_FreeSurface(this->barreChargement_surface);

	//On détruit les boutons
    for(int i=0; i<this->boutons.size(); i++)
        delete this->boutons[i];

	// On détruit le renderer
	if(this->renderer)
		SDL_DestroyRenderer(this->renderer);

	// On détruit la fenêtre
	if(this->pWindow)
		SDL_DestroyWindow(this->pWindow);
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
 * Active ou désactive le mode plein écran de la fenêtre
 * Valeur par défaut : activer le mode plein écran (true)
 */
void Affichage::getScreenSize(int* x, int* y)
{
	this->lock.lock();
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

	// Barre de chargement :
	if(this->barreChargement_surface != nullptr && this->barreChargement == nullptr)
		this->barreChargement = SDL_CreateTextureFromSurface(this->renderer, this->barreChargement_surface);

	// Icône
	if(this->icone != nullptr && this->gifChargement == nullptr)
		this->gifChargement = SDL_CreateTextureFromSurface(this->renderer, this->icone);

	// Si le fond est chargé, on l'affiche
	if(this->fond_surface != nullptr)
	{
		if(this->fond == nullptr)
			// Conversion de la surface en texture
			this->fond = SDL_CreateTextureFromSurface(this->renderer, this->fond_surface);

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

	// Si la taille de la fenêtre a changée, on recharge l'écran
	int x, y;
	SDL_GetWindowSize(this->pWindow, &x, &y);
	if(x != this->ancienneTailleX || y != this->ancienneTailleY)
	{
		this->ancienneTailleX = x;
		this->ancienneTailleY = y;
		this->lock.unlock();
		this->setEcran(-1);
		this->lock.lock();
	}

	// Selon le mode actuel, on appelle la bonne méthode
	switch(this->ecran){
		case MODE_AFFICHAGE_CHARGEMENT:
			this->afficherEcranChargement();
			break;

		case MODE_AFFICHAGE_CONFIG_TOUCHES_CATEGORIES:
			this->menu->update(this->nbTicks());
			break;

		default:
			break;
	}

	// Si la police est chargée
	if(this->policeChargement != nullptr); // TODO : afficher les fps correctement, et uniquement si demandé
	if(false)
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

/**
 *
 * @return une erreur s'il y a, 0 sinon
 */
int Affichage::init_main(){
	int error = 0;

    // Icones des boutons
    for(int i=0; i<this->boutons.size(); i++)
        this->boutons[i]->init();

    return error;
}

int Affichage::init() // TODO : Ajouter tous les éléments à charger + image de fond + calculer % entre chaque chargement
{
	int error = 0;

	// On crée un fond noir pour l'écran de chargement
	std::string temp2 = this->path;
	temp2 += IMAGE_FOND_FILENAME;
	this->lock.lock();
	this->fond_surface = IMG_Load(temp2.c_str());
	this->lock.unlock();
	if(this->fond_surface == nullptr)
	{
		error = 1;
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image de fond : %s\n", IMG_GetError());
	}

	// On crée la barre de chargement
	this->afficherEcranChargement(25, "Chargement...");
	this->lock.lock();
	this->barreChargement_surface = SDL_CreateRGBSurface(0, 1, 1, 32, rmask, gmask, bmask, amask);
	SDL_FillRect(this->barreChargement_surface, NULL, SDL_MapRGB(this->barreChargement_surface->format, COULEUR_BARRE_CHARGEMENT_R, COULEUR_BARRE_CHARGEMENT_G, COULEUR_BARRE_CHARGEMENT_B));
	this->lock.unlock();

	// On charge l'icône de l'appli (qui sera aussi le gif de chargement)
	this->afficherEcranChargement(50, "Chargement de l'icône du jeu");
	temp2 = this->path;
	temp2 += ICONE_FILENAME;
	this->lock.lock();
	this->icone = IMG_Load(temp2.c_str());
	this->lock.unlock();
	if(this->icone)
		SDL_SetWindowIcon(this->pWindow, this->icone);
	else
	{
		error = 1;
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'icône : %s\n", IMG_GetError());
	}

	// On charge les polices d'écriture
	this->afficherEcranChargement(75, "Chargement de la police d'écriture 01/02");
	this->lock.lock();
	temp2 = this->path;
	temp2 += POLICE_CHARGEMENT_FILENAME;
	this->policeChargement = TTF_OpenFont(temp2.c_str(), POLICE_CHARGEMENT_TAILLE*this->tailleRef);
	this->lock.unlock();
	if(this->policeChargement == nullptr)
	{
		error = 2;
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger la police de chargement : %s\n", TTF_GetError());
	}
	this->afficherEcranChargement(75, "Chargement de la police d'écriture 02/02");
	this->lock.lock();
	temp2 = this->path;
	temp2 += POLICE_MENU_FILENAME;
	this->policeMenu = TTF_OpenFont(temp2.c_str(), POLICE_MENU_TAILLE*this->tailleRef);
	this->policeTitreMenu = TTF_OpenFont(temp2.c_str(), POLICE_MENU_TITRE_TAILLE*this->tailleRef);
	this->lock.unlock();
	if(this->policeMenu == nullptr)
	{
		error = 2;
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger la police de chargement : %s\n", TTF_GetError());
	}

	// On crée les menus
	this->menus = new Menu*[NB_MENUS];
	for(int i=0; i<NB_MENUS; i++)
		this->menus[i] = new Menu(this->touchesJeu, this->renderer, this->policeMenu);

	// On met une alerte si le nombre de bundle change (il faut recoder les touches en dur)
	if(NB_BUNDLE != 4)
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "ATTENTION : le nombre de bundle est à changer, dans Affichage::init() !!!");

	// On charge les icônes de configuration des touches
	// Catégorie persistante
	{
		this->afficherEcranChargement(60, "Chargement de l'icône config_touches_catégorie persistante");
		std::string temp2 = this->path;
		temp2 += BOUTON_CONFIG_DIRNAME;
		temp2 += "categorie/1.";;
		temp2 += IMAGE_FORMAT;
		SDL_Surface* fondC = IMG_Load(temp2.c_str());

		std::vector<Bouton*> bts;
		bts.clear();

		if(fondC)
		{
			Bouton* b = new Bouton(fondC, this->renderer, TAILLE_ICONE_CONFIG_TOUCHE*this->tailleRef, "Catégorie persistante", "Ensemble des touches actionnables partout");
			bts.push_back(b);
			this->lock.lock();
			this->boutons.push_back(b);
			this->lock.unlock();
		}
		else
		{
			error = 3;
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image : %s\n", IMG_GetError());
		}

		// On charge toutes les icônes de sous-catégorie de la catégorie
		for(int i=TOUCHE_PLEIN_ECRAN; i<TOUCHE_NAVIGATION_DEPLACER_AXE_HAUT_BAS; i++)
		{
			std::string temp = "Chargement de l'icône config_touches (";
			temp += Affichage::intToString(i);
			temp += "/";
			temp += Affichage::intToString(NB_TOUCHES_JEU);
			temp += ")";
			this->afficherEcranChargement(61+i, temp.c_str());
			std::string temp2 = this->path;
			temp2 += BOUTON_CONFIG_DIRNAME;
			temp2 += "touche/";
			temp2 += Affichage::intToString(i);
			temp2 += ".";
			temp2 += IMAGE_FORMAT;

			SDL_Surface* fondC = IMG_Load(temp2.c_str());
			if(fondC)
			{
				Bouton* b = new Bouton(fondC, this->renderer, TAILLE_ICONE_CONFIG_TOUCHE*this->tailleRef, this->touchesJeu[i]->getNom(), this->touchesJeu[i]->getDescription());
				bts.push_back(b);
				this->lock.lock();
				this->boutons.push_back(b);
				this->lock.unlock();
			}
			else
			{
				error = 3;
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image : %s\n", IMG_GetError());
			}
		}

		// On ajoute les boutons au menu
		this->menus[MENU_CONFIG_TOUCHES_CHOIX_TOUCHE]->ajoutCategorie(bts);
	}

	// Catégorie navigation
	{
		this->afficherEcranChargement(60, "Chargement de l'icône config_touches_catégorie navigation");
		std::string temp2 = this->path;
		temp2 += BOUTON_CONFIG_DIRNAME;
		temp2 += "categorie/2.";;
		temp2 += IMAGE_FORMAT;
		SDL_Surface* fondC = IMG_Load(temp2.c_str());

		std::vector<Bouton*> bts;
		bts.clear();

		if(fondC)
		{
			Bouton* b = new Bouton(fondC, this->renderer, TAILLE_ICONE_CONFIG_TOUCHE*this->tailleRef, "Catégorie navigation", "Toutes les touches gérant la navigation dans les menus");
			bts.push_back(b);
			this->lock.lock();
			this->boutons.push_back(b);
			this->lock.unlock();
		}
		else
		{
			error = 3;
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image : %s\n", IMG_GetError());
		}

		// On charge toutes les icônes de sous-catégorie de la catégorie
		for(int i=TOUCHE_NAVIGATION_DEPLACER_AXE_HAUT_BAS; i<TOUCHE_ANGULAIRE_BOUTON_ANGLE; i++)
		{
			std::string temp = "Chargement de l'icône config_touches (";
			temp += Affichage::intToString(i);
			temp += "/";
			temp += Affichage::intToString(NB_TOUCHES_JEU);
			temp += ")";
			this->afficherEcranChargement(61+i, temp.c_str());
			std::string temp2 = this->path;
			temp2 += BOUTON_CONFIG_DIRNAME;
			temp2 += "touche/";
			temp2 += Affichage::intToString(i);
			temp2 += ".";
			temp2 += IMAGE_FORMAT;

			SDL_Surface* fondC = IMG_Load(temp2.c_str());
			if(fondC)
			{
				Bouton* b = new Bouton(fondC, this->renderer, TAILLE_ICONE_CONFIG_TOUCHE*this->tailleRef, this->touchesJeu[i]->getNom(), this->touchesJeu[i]->getDescription());
				bts.push_back(b);
				this->lock.lock();
				this->boutons.push_back(b);
				this->lock.unlock();
			}
			else
			{
				error = 3;
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image : %s\n", IMG_GetError());
			}
		}

		// On ajoute les boutons au menu
		this->menus[MENU_CONFIG_TOUCHES_CHOIX_TOUCHE]->ajoutCategorie(bts);
	}

	// Catégorie angulaire
	{
		this->afficherEcranChargement(60, "Chargement de l'icône config_touches_catégorie sélection angulaire");
		std::string temp2 = this->path;
		temp2 += BOUTON_CONFIG_DIRNAME;
		temp2 += "categorie/3.";;
		temp2 += IMAGE_FORMAT;
		SDL_Surface* fondC = IMG_Load(temp2.c_str());

		std::vector<Bouton*> bts;
		bts.clear();

		if(fondC)
		{
			Bouton* b = new Bouton(fondC, this->renderer, TAILLE_ICONE_CONFIG_TOUCHE*this->tailleRef, "Catégorie sélection angulaire", "Toutes les touches gérant la navigation dans les sélections angulaires");
			bts.push_back(b);
			this->lock.lock();
			this->boutons.push_back(b);
			this->lock.unlock();
		}
		else
		{
			error = 3;
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image : %s\n", IMG_GetError());
		}

		// On charge toutes les icônes de sous-catégorie de la catégorie
		for(int i=TOUCHE_ANGULAIRE_BOUTON_ANGLE; i<TOUCHE_PLATEAU_MENU; i++)
		{
			std::string temp = "Chargement de l'icône config_touches (";
			temp += Affichage::intToString(i);
			temp += "/";
			temp += Affichage::intToString(NB_TOUCHES_JEU);
			temp += ")";
			this->afficherEcranChargement(61+i, temp.c_str());
			std::string temp2 = this->path;
			temp2 += BOUTON_CONFIG_DIRNAME;
			temp2 += "touche/";
			temp2 += Affichage::intToString(i);
			temp2 += ".";
			temp2 += IMAGE_FORMAT;

			SDL_Surface* fondC = IMG_Load(temp2.c_str());
			if(fondC)
			{
				Bouton* b = new Bouton(fondC, this->renderer, TAILLE_ICONE_CONFIG_TOUCHE*this->tailleRef, this->touchesJeu[i]->getNom(), this->touchesJeu[i]->getDescription());
				bts.push_back(b);
				this->lock.lock();
				this->boutons.push_back(b);
				this->lock.unlock();
			}
			else
			{
				error = 3;
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image : %s\n", IMG_GetError());
			}
		}

		// On ajoute les boutons au menu
		this->menus[MENU_CONFIG_TOUCHES_CHOIX_TOUCHE]->ajoutCategorie(bts);
	}

	// Catégorie plateau
	{
		this->afficherEcranChargement(60, "Chargement de l'icône config_touches_catégorie plateau");
		std::string temp2 = this->path;
		temp2 += BOUTON_CONFIG_DIRNAME;
		temp2 += "categorie/4.";;
		temp2 += IMAGE_FORMAT;
		SDL_Surface* fondC = IMG_Load(temp2.c_str());

		std::vector<Bouton*> bts;
		bts.clear();

		if(fondC)
		{
			Bouton* b = new Bouton(fondC, this->renderer, TAILLE_ICONE_CONFIG_TOUCHE*this->tailleRef, "Catégorie plateau", "Toutes les touches utilisables sur l'écran du plateau");
			bts.push_back(b);
			this->lock.lock();
			this->boutons.push_back(b);
			this->lock.unlock();
		}
		else
		{
			error = 3;
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image : %s\n", IMG_GetError());
		}

		// On charge toutes les icônes de sous-catégorie de la catégorie
		for(int i=TOUCHE_PLATEAU_MENU; i<TOUCHE_PLEIN_ECRAN+NB_TOUCHES_JEU; i++)
		{
			std::string temp = "Chargement de l'icône config_touches (";
			temp += Affichage::intToString(i);
			temp += "/";
			temp += Affichage::intToString(NB_TOUCHES_JEU);
			temp += ")";
			this->afficherEcranChargement(61+i, temp.c_str());
			std::string temp2 = this->path;
			temp2 += BOUTON_CONFIG_DIRNAME;
			temp2 += "touche/";
			temp2 += Affichage::intToString(i);
			temp2 += ".";
			temp2 += IMAGE_FORMAT;

			SDL_Surface* fondC = IMG_Load(temp2.c_str());
			if(fondC)
			{
				Bouton* b = new Bouton(fondC, this->renderer, TAILLE_ICONE_CONFIG_TOUCHE*this->tailleRef, this->touchesJeu[i]->getNom(), this->touchesJeu[i]->getDescription());
				bts.push_back(b);
				this->lock.lock();
				this->boutons.push_back(b);
				this->lock.unlock();
			}
			else
			{
				error = 3;
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image : %s\n", IMG_GetError());
			}
		}

		// On ajoute les boutons au menu
		this->menus[MENU_CONFIG_TOUCHES_CHOIX_TOUCHE]->ajoutCategorie(bts);
	}

	// icône souris
	{
		std::string temp = "Chargement de l'icône config_touche_3 (1/2)";
		this->afficherEcranChargement(60, temp.c_str());
		std::string temp2 = this->path;
		temp2 += BOUTON_CONFIG_DIRNAME;
		temp2 += "souris.";
		temp2 += IMAGE_FORMAT;

        SDL_Surface* fondC = IMG_Load(temp2.c_str());
		if(fondC)
		{
			Bouton* b = new Bouton(fondC, this->renderer, TAILLE_ICONE_CONFIG_TOUCHE*this->tailleRef);
			this->lock.lock();
			this->boutons.push_back(b);
			this->lock.unlock();
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
		std::string temp2 = this->path;
		temp2 += BOUTON_CONFIG_DIRNAME;
		temp2 += "clavier.";
		temp2 += IMAGE_FORMAT;

		SDL_Surface* fondC = IMG_Load(temp2.c_str());
		if(fondC)
		{
			Bouton* b = new Bouton(fondC, this->renderer, TAILLE_ICONE_CONFIG_TOUCHE*this->tailleRef);
			this->lock.lock();
			this->boutons.push_back(b);
			this->lock.unlock();
		}
		else
		{
			error = 3;
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image : %s\n", IMG_GetError());
		}
	}

	// icône bouton
	{
		std::string temp2 = this->path;
		temp2 += BOUTON_CONFIG_DIRNAME;
		temp2 += "bouton.";
		temp2 += IMAGE_FORMAT;

		SDL_Surface* fondC = IMG_Load(temp2.c_str());
		if(fondC)
		{
			Bouton* b = new Bouton(fondC, this->renderer, TAILLE_ICONE_CONFIG_TOUCHE*this->tailleRef);
			this->lock.lock();
			this->boutons.push_back(b);
			this->lock.unlock();
		}
		else
		{
			error = 3;
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image : %s\n", IMG_GetError());
		}
	}

	// icône axe
	{
		std::string temp2 = this->path;
		temp2 += BOUTON_CONFIG_DIRNAME;
		temp2 += "axe.";
		temp2 += IMAGE_FORMAT;

		SDL_Surface* fondC = IMG_Load(temp2.c_str());
		if(fondC)
		{
			Bouton* b = new Bouton(fondC, this->renderer, TAILLE_ICONE_CONFIG_TOUCHE*this->tailleRef);
			this->lock.lock();
			this->boutons.push_back(b);
			this->lock.unlock();
		}
		else
		{
			error = 3;
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image : %s\n", IMG_GetError());
		}
	}

	// icône croix
	{
		std::string temp2 = this->path;
		temp2 += BOUTON_CONFIG_DIRNAME;
		temp2 += "croix.";
		temp2 += IMAGE_FORMAT;

		SDL_Surface* fondC = IMG_Load(temp2.c_str());
		if(fondC)
		{
			Bouton* b = new Bouton(fondC, this->renderer, TAILLE_ICONE_CONFIG_TOUCHE*this->tailleRef);
			this->lock.lock();
			this->boutons.push_back(b);
			this->lock.unlock();
		}
		else
		{
			error = 3;
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image : %s\n", IMG_GetError());
		}
	}

	// icône molette
	{
		std::string temp2 = this->path;
		temp2 += BOUTON_CONFIG_DIRNAME;
		temp2 += "molette.";
		temp2 += IMAGE_FORMAT;

		SDL_Surface* fondC = IMG_Load(temp2.c_str());
		if(fondC)
		{
			Bouton* b = new Bouton(fondC, this->renderer, TAILLE_ICONE_CONFIG_TOUCHE*this->tailleRef);
			this->lock.lock();
			this->boutons.push_back(b);
			this->lock.unlock();
		}
		else
		{
			error = 3;
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible de charger l'image : %s\n", IMG_GetError());
		}
	}

	// icône new
	{
		std::string temp2 = this->path;
		temp2 += BOUTON_CONFIG_DIRNAME;
		temp2 += "new.";
		temp2 += IMAGE_FORMAT;

		SDL_Surface* fondC = IMG_Load(temp2.c_str());
		if(fondC)
		{
			Bouton* b = new Bouton(fondC, this->renderer, TAILLE_ICONE_CONFIG_TOUCHE*this->tailleRef);
			this->lock.lock();
			this->boutons.push_back(b);
			this->lock.unlock();
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

Bouton* Affichage::getBouton(int noBouton)
{
	Bouton* ret = nullptr;
	this->lock.lock();
	if(this->boutons.size()>noBouton && noBouton>0)
		ret = this->boutons[noBouton];
	this->lock.unlock();
	return ret;
}

/**
 * Ensemble des fonctions d'affichage
 */
void Affichage::setEcran(int ecran) // TODO : Gérer les transitions
{
	if(ecran == -1)
		ecran = this->ecran;
	this->lock.lock();
	unsigned int nbT = this->nbTicks();
	// On ouvre le menu de config touche - choix
	if(this->ecran != MODE_AFFICHAGE_CONFIG_TOUCHES_CATEGORIES && ecran == MODE_AFFICHAGE_CONFIG_TOUCHES_CATEGORIES)
	{
		// On crée le menu des Touches
		this->menu = menus[MENU_CONFIG_TOUCHES_CHOIX_TOUCHE];
		// Le menu prend tout l'écran avec une marge de 10 pixels
		this->menu->setTailleAffichage(this->ancienneTailleX-20, this->ancienneTailleY-20, TAILLE_ICONE_CONFIG_TOUCHE*this->tailleRef);
		this->menu->setPositionAffichage(10, 10);
		this->menu->update(0);
	}
	// On ferme le menu de config touche - choix
	if(this->ecran == MODE_AFFICHAGE_CONFIG_TOUCHES_CATEGORIES && ecran != MODE_AFFICHAGE_CONFIG_TOUCHES_CATEGORIES)
	{
		this->menu->update(-1); // TODO : gérer animations
	}
	// On recharge la fenêtre du menu de config touche - choix
	if(this->ecran == MODE_AFFICHAGE_CONFIG_TOUCHES_CATEGORIES && ecran == MODE_AFFICHAGE_CONFIG_TOUCHES_CATEGORIES)
	{
		this->menu->setTailleAffichage(this->ancienneTailleX-20, this->ancienneTailleY-20, TAILLE_ICONE_CONFIG_TOUCHE*this->tailleRef);
	}
	this->ecran = ecran;
	this->lock.unlock();
}

Menu* Affichage::getMenu()
{
	Menu* ret;
	this->lock.lock();
	ret = this->menu;
	this->lock.unlock();
	return ret;
}

void Affichage::afficherEcranChargement(int pourcentage, std::string message)
{
	// Si on doit juste changer les paramètres
	if(pourcentage != -1)
	{
		// On change les attributs en tenant compte des mutex
		this->lock.lock();
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

std::string Affichage::intToString(int i) {
	std::ostringstream oss;
	oss << i;
	return oss.str();
}

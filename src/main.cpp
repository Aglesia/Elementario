#include <Affichage.h>
#include <Controller.h>
#include <ToucheJeu.h>
#include <Touche.h>
#include <defs.h>

typedef struct VarGame
{
	ToucheJeu** touches = nullptr; // Tableau des touchesJeu
	int configTouchesCategorie = 0; // Catégorie actuelle dans l'écran de configuration des touches
	int configTouchesTouche = 0; // Touche actuelle dans l'écran de configuration des touches
	int configTouchesController = -2; // Controller actuel dans l'écran de configuration des touches
	int configTouchesBoutonController = 0; // Bouton du controller actuel dans l'écran de configuration des touches
	Touche* configTouchesToucheController = nullptr; // Bouton du controller actuel dans l'écran de configuration des touches
} VarGame;

typedef struct DataThread
{
	Affichage* aff = nullptr; // Objet affichage
	Controller* c = nullptr; // Objet controller
	bool isStop = false; // Le thread a fini
	bool mustStop = false; // Le thread doit être arreté
	int ret = 0; // Retour de l'étape
	void* data = nullptr; // Donnée supplémentaire
	VarGame* var = nullptr; // Données du jeu (variables "globales")
	std::mutex mutex; // Mutex
} DataThread;

/**
 * Fonctions de thread
 */
// Initialisation du logiciel
static int threadInit(void* data) // TODO : Toutes les touches de base
{
	DataThread* t = (DataThread*)data;
	t->mutex.lock();
	t->isStop = false;
	t->mutex.unlock();

	// On active les touches nécessaires
	ToucheJeu** touches = t->var->touches;
	touches[TOUCHE_HAUT_BAS]->activer();
	touches[TOUCHE_GAUCHE_DROITE]->activer();
	touches[TOUCHE_PLEIN_ECRAN]->activer();

	// On associe les boutons de base
	Touche* touche = t->c->getTouche(-1, SDLK_UP, TYPE_TOUCHE_BOUTON);
	if(touche)
	{
		touche->setValeursClic(-255, 0);
		touches[TOUCHE_HAUT_BAS]->addTouche(touche);
	}
	touche = t->c->getTouche(-1, SDLK_DOWN, TYPE_TOUCHE_BOUTON);
	if(touche)
		touches[TOUCHE_HAUT_BAS]->addTouche(touche);
	touche = t->c->getTouche(-1, SDLK_LEFT, TYPE_TOUCHE_BOUTON);
	if(touche)
	{
		touche->setValeursClic(-255, 0);
		touches[TOUCHE_GAUCHE_DROITE]->addTouche(touche);
	}
	touche = t->c->getTouche(-1, SDLK_RIGHT, TYPE_TOUCHE_BOUTON);
	if(touche)
		touches[TOUCHE_GAUCHE_DROITE]->addTouche(touche);
	touche = t->c->getTouche(-1, SDLK_z, TYPE_TOUCHE_BOUTON);
	if(touche)
	{
		touche->setValeursClic(-PRECISION_VALEUR_AXE_TOUCHE/2, 0);
		touches[TOUCHE_HAUT_BAS]->addTouche(touche);
	}
	touche = t->c->getTouche(-1, SDLK_s, TYPE_TOUCHE_BOUTON);
	if(touche)
	{
		touche->setValeursClic(PRECISION_VALEUR_AXE_TOUCHE/2, 0);
		touches[TOUCHE_HAUT_BAS]->addTouche(touche);
	}
	touche = t->c->getTouche(-1, SDLK_q, TYPE_TOUCHE_BOUTON);
	if(touche)
	{
		touche->setValeursClic(-PRECISION_VALEUR_AXE_TOUCHE/2, 0);
		touches[TOUCHE_GAUCHE_DROITE]->addTouche(touche);
	}
	touche = t->c->getTouche(-1, SDLK_d, TYPE_TOUCHE_BOUTON);
	if(touche)
	{
		touche->setValeursClic(PRECISION_VALEUR_AXE_TOUCHE/2, 0);
		touches[TOUCHE_GAUCHE_DROITE]->addTouche(touche);
	}
	touche = t->c->getTouche(-1, SDLK_F11, TYPE_TOUCHE_BOUTON);
	if(touche)
		touches[TOUCHE_PLEIN_ECRAN]->addTouche(touche);

	// On initialise les graphismes
	t->ret = t->aff->init();
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Jeu chargé\n");

	t->mutex.lock();
	t->isStop = true;
	t->mutex.unlock();

	return t->ret;
}

// Configuration des touches étape 1
static int threadConfigTouches1(void* data) // TODO : exporter les string (fichier à part)
{
	DataThread* t = (DataThread*)data;
	t->mutex.lock();
	t->isStop = false;
	t->mutex.unlock();

	// On affiche le menu
	t->aff->setEcran(MODE_AFFICHAGE_CONFIG_TOUCHES1);
	bool stop = false;
	bool modif = true;
	while(!stop)
	{
		SDL_Delay(15);
		t->mutex.lock();
		stop = t->mustStop;
		t->mutex.unlock();

		// Si on a demandé à descendre
		ToucheJeu* touche = t->var->touches[TOUCHE_HAUT_BAS];
		if(touche != nullptr)
		{
			if(touche->getVal()>0)
			{
				if(t->var->configTouchesCategorie<CONFIG_TOUCHES_NB_CATEGORIES-1)
					t->var->configTouchesCategorie++;
				touche->setVal(0);
				modif = true;

			}
			else if(touche->getVal()<0)
			{
				if(t->var->configTouchesCategorie>0)
					t->var->configTouchesCategorie--;
				touche->setVal(0);
				modif = true;
			}
			if(modif)
			{
				modif = false;
				std::string titre;
				std::string description;
				switch(t->var->configTouchesCategorie)
				{
					case 0:
						titre = "Déplacements";
						description = "Description de ouf gueudin !!";
						break;

					case 1:
						titre = "Attaques";
						description = "Description de ouf gueudin !!";
						break;

					case 2:
						titre = "Messagerie";
						description = "Description de ouf gueudin !!";
						break;

					case 3:
						titre = "Affichage";
						description = "Description de ouf gueudin !!";
						break;

					default:
						titre = "lol";
						description = "lolilol";
				}
				t->aff->afficherMenuConfigTouches1(t->var->configTouchesCategorie);
				t->aff->afficherInfoConfigurationTouches1(titre, description);
			}
		}

		// Si on a demandé à quitter
		touche = t->var->touches[TOUCHE_GAUCHE_DROITE];
		if(touche != nullptr)
		{
			if(touche->getVal()>0)
			{
				touche->setVal(0, false);
				t->ret = 1; // Passage à la colonne suivante
				switch(t->var->configTouchesCategorie)
				{
					case 0:
						t->var->configTouchesTouche = 0;
						break;

					case 1:
						t->var->configTouchesTouche = 3;
						break;

					case 2:
						t->var->configTouchesTouche = 6;
						break;

					case 3:
						t->var->configTouchesTouche = 8;
						break;

					default:
						break;
				}
				stop = true;
				t->aff->afficherMenuConfigTouches2(t->var->configTouchesTouche);
			}
			else if(touche->getVal()<0)
			{
				touche->setVal(0, false);
				t->ret = 2;
				stop = true;
			}
		}
	}

	t->mutex.lock();
	t->isStop = true;
	t->mutex.unlock();

	return t->ret;
}

// Configuration des touches étape 2
static int threadConfigTouches2(void* data) // TODO : exporter les string (fichier à part)
{
	DataThread* t = (DataThread*)data;
	t->mutex.lock();
	t->isStop = false;
	t->mutex.unlock();

	// On affiche le menu
	t->aff->setEcran(MODE_AFFICHAGE_CONFIG_TOUCHES2);
	bool stop = false;
	bool modif = true;
	while(!stop)
	{
		SDL_Delay(15);
		t->mutex.lock();
		stop = t->mustStop;
		t->mutex.unlock();

		// Si on a demandé à descendre
		ToucheJeu* touche = t->var->touches[TOUCHE_HAUT_BAS];
		if(touche != nullptr)
		{
			if(touche->getVal()>0)
			{
				if(t->var->configTouchesTouche<CONFIG_TOUCHES_NB_TOUCHE_JEU-1)
					t->var->configTouchesTouche++;
				if(t->var->configTouchesTouche>7)
					t->var->configTouchesCategorie = 3;
				else if(t->var->configTouchesTouche>5)
					t->var->configTouchesCategorie = 2;
				else if(t->var->configTouchesTouche>2)
					t->var->configTouchesCategorie = 1;
				else
					t->var->configTouchesCategorie = 0;
				touche->setVal(0);
				modif = true;

			}
			else if(touche->getVal()<0)
			{
				if(t->var->configTouchesTouche>0)
					t->var->configTouchesTouche--;
				if(t->var->configTouchesTouche<=2)
					t->var->configTouchesCategorie = 0;
				else if(t->var->configTouchesTouche<=5)
					t->var->configTouchesCategorie = 1;
				else if(t->var->configTouchesTouche<=7)
					t->var->configTouchesCategorie = 2;
				else
					t->var->configTouchesCategorie = 3;
				touche->setVal(0);
				modif = true;
			}
			if(modif)
			{
				modif = false;
				std::string titre;
				std::string description;
				switch(t->var->configTouchesTouche)
				{
					case 0:
						titre = "Déplacements";
						description = "Description de ouf gueudin !!";
						break;

					default:
						titre = "lol";
						description = "lolilol";
				}
				t->aff->afficherMenuConfigTouches1(t->var->configTouchesCategorie);
				t->aff->afficherMenuConfigTouches2(t->var->configTouchesTouche);
				t->aff->afficherInfoConfigurationTouches1(titre, description);
			}
		}

		// Si on a demandé à quitter
		touche = t->var->touches[TOUCHE_GAUCHE_DROITE];
		if(touche != nullptr)
		{
			if(touche->getVal()>0)
			{
				touche->setVal(0);
				t->ret = 1;
				stop = true;
			}
			else if(touche->getVal()<0)
			{
				touche->setVal(0);
				t->ret = 2;
				stop = true;
			}
		}
	}

	t->mutex.lock();
	t->isStop = true;
	t->mutex.unlock();

	return t->ret;
}

// Configuration des touches étape 3
static int threadConfigTouches3(void* data)
{
	DataThread* t = (DataThread*)data;
	t->mutex.lock();
	t->isStop = false;
	t->mutex.unlock();

	// On affiche le menu
	t->aff->setEcran(MODE_AFFICHAGE_CONFIG_TOUCHES3);
	bool stop = false;
	bool modif = true;
	while(!stop)
	{
		SDL_Delay(15);
		t->mutex.lock();
		stop = t->mustStop;
		t->mutex.unlock();

		// Si on a demandé à descendre
		ToucheJeu* touche = t->var->touches[TOUCHE_HAUT_BAS];
		if(touche != nullptr)
		{
			if(touche->getVal()>0)
			{
				if(t->var->configTouchesController<t->c->nbController()-1)
					t->var->configTouchesController++;
				touche->setVal(0);
				modif = true;

			}
			else if(touche->getVal()<0)
			{
				if(t->var->configTouchesController>-2)
					t->var->configTouchesController--;
				touche->setVal(0);
				modif = true;
			}
			if(modif)
			{
				modif = false;
				std::string titre;
				std::string description;
				Manette* m = t->c->getController(t->var->configTouchesController);
				if(m != nullptr)
				{
					titre = m->name;
					description = "- Nombre de boutons : ";
					description += SSTR(m->nbBouton);
					description += "\n- Nombre d'axes : ";
					description += SSTR(m->nbJoystick + (m->nbDirection*2));
				}
				else
				{
					titre = "Inconnu";
					description = "Manette inconnue";
				}
				t->aff->afficherMenuConfigTouches3(t->c->nbController(), t->var->configTouchesController);
				t->aff->afficherInfoConfigurationTouches1(titre, description);
			}
		}

		// Si on a demandé à quitter
		touche = t->var->touches[TOUCHE_GAUCHE_DROITE];
		if(touche != nullptr)
		{
			if(touche->getVal()>0)
			{
				touche->setVal(0);
				t->ret = 1;
				stop = true;
			}
			else if(touche->getVal()<0)
			{
				touche->setVal(0);
				t->ret = 2;
				stop = true;
			}
		}
	}

	t->mutex.lock();
	t->isStop = true;
	t->mutex.unlock();

	return t->ret;
}

// Configuration des touches étape 4
static int threadConfigTouches4(void* data)
{
	DataThread* t = (DataThread*)data;
	t->mutex.lock();
	t->isStop = false;
	t->mutex.unlock();

	// On récupère le contrôleur
	bool stop = false;
	Manette* m = t->c->getController(t->var->configTouchesController);
	std::vector<Touche*> touches;
	ToucheJeu* to = nullptr;
	if(t->var->configTouchesTouche<NB_TOUCHES_JEU)
		to = t->var->touches[t->var->configTouchesTouche];
	if(m == nullptr || to == nullptr)
	{
		t->mutex.lock();
		t->ret = 4;
		t->mutex.unlock();
		stop = true;
	}

	// On liste les touches
	else if(m->id != -1)
	{
		for(int i=1; i<=m->nbBouton; i++)
			if(to->touchePresente(t->c->getTouche(m->id, i, TYPE_TOUCHE_BOUTON)))
				touches.push_back(t->c->getTouche(m->id, i, TYPE_TOUCHE_BOUTON));
		for(int i=1; i<=m->nbJoystick; i++)
			if(to->touchePresente(t->c->getTouche(m->id, i, TYPE_TOUCHE_JOYSTIC)))
				touches.push_back(t->c->getTouche(m->id, i, TYPE_TOUCHE_JOYSTIC));
		for(int i=1; i<=m->nbDirection*2; i++)
			if(to->touchePresente(t->c->getTouche(m->id, i, TYPE_TOUCHE_DIRECTION)))
				touches.push_back(t->c->getTouche(m->id, i, TYPE_TOUCHE_DIRECTION));
		for(int i=1; i<=m->nbMolette; i++)
			if(to->touchePresente(t->c->getTouche(m->id, i, TYPE_TOUCHE_MOLETTE)))
				touches.push_back(t->c->getTouche(m->id, i, TYPE_TOUCHE_MOLETTE));
	}
	else
	{
		for(int i=0; i<=127; i++)
			if(to->touchePresente(t->c->getTouche(m->id, i, TYPE_TOUCHE_BOUTON)))
				touches.push_back(t->c->getTouche(m->id, i, TYPE_TOUCHE_BOUTON));
		for(int i=1073741881; i<=1073742106; i++)
			if(to->touchePresente(t->c->getTouche(m->id, i, TYPE_TOUCHE_BOUTON)))
				touches.push_back(t->c->getTouche(m->id, i, TYPE_TOUCHE_BOUTON));
	}
	Touche* toucheFake = new Touche(0, 0, 0);
	toucheFake->nom = "Nouvelle touche";

	// On affiche le menu
	t->aff->afficherMenuConfigTouches4((std::vector<Touche*>*)-1, 0);
	t->aff->setEcran(MODE_AFFICHAGE_CONFIG_TOUCHES4);
	t->aff->afficherMenuConfigTouches4(&touches, 0);
	t->var->configTouchesBoutonController = 0;
	t->var->configTouchesToucheController = (touches.size()>0)?touches[0]:nullptr;
	bool modif = true;
	while(!stop)
	{
		SDL_Delay(15);
		t->mutex.lock();
		stop = t->mustStop;
		t->mutex.unlock();

		// Si on a demandé à descendre
		ToucheJeu* touche = t->var->touches[TOUCHE_HAUT_BAS];
		if(touche != nullptr)
		{
			if(touche->getVal()>0)
			{
				if(t->var->configTouchesBoutonController<touches.size())
					t->var->configTouchesBoutonController++;
				touche->setVal(0);
				modif = true;

			}
			else if(touche->getVal()<0)
			{
				if(t->var->configTouchesBoutonController>0)
					t->var->configTouchesBoutonController--;
				touche->setVal(0);
				modif = true;
			}
			if(modif)
			{
				modif = false;
				t->aff->afficherMenuConfigTouches4(&touches, t->var->configTouchesBoutonController);
				if(t->var->configTouchesBoutonController<touches.size())
				{
					t->aff->afficherInfoConfigurationTouches2(touches[t->var->configTouchesBoutonController]);
					t->var->configTouchesToucheController = touches[t->var->configTouchesBoutonController];
				}
				else
				{
					t->aff->afficherInfoConfigurationTouches2(toucheFake);
					t->var->configTouchesToucheController = nullptr;
				}
				if(t->var->configTouchesBoutonController == touches.size())
					t->ret = 1;
				else
					t->ret = 2;
			}
		}

		// Si on a demandé à quitter
		touche = t->var->touches[TOUCHE_GAUCHE_DROITE];
		if(touche != nullptr)
		{
			if(touche->getVal()>0)
			{
				touche->setVal(0);
				stop = true;
			}
			else if(touche->getVal()<0)
			{
				touche->setVal(0);
				t->ret = 3;
				stop = true;
			}
		}
	}

	t->mutex.lock();
	t->isStop = true;
	t->mutex.unlock();

	return t->ret;
}

// COnfiguration des touches étape 5
static int threadConfigTouches5(void* data) // TODO : Afficher les propriétés de la touche
{
	DataThread* t = (DataThread*)data;
	t->mutex.lock();
	t->isStop = false;
	t->mutex.unlock();

	ToucheJeu* tj = t->var->touches[t->var->configTouchesTouche];
	Manette* m = t->c->getController(t->var->configTouchesController);
	if(m != nullptr && tj != nullptr)
	{
		bool quitter = false;
		bool continuer = false;
		// Si c'est une nouvelle touche
		if(t->var->configTouchesToucheController == nullptr)
		{
			int noT = 0;
			int typeT = 0;
			do{
				Touche touche(m->id, 0, 0);
				t->c->listen(&touche);
				SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Attente d'appuie...");
				while(touche.getType() == TYPE_TOUCHE_INCONNU && !quitter)
				{
					t->mutex.lock();
					quitter = t->mustStop;
					t->mutex.unlock();
					SDL_Delay(1);
				}
				SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Touche appuyée : %d de type %d. Appuyé : %d\n", touche.getNoTouche(), touche.getType(), touche.isPressed());
				noT = touche.getNoTouche();
				typeT = touche.getType();
				continuer = !touche.isPressed();
			}while(continuer && !quitter);
			// On ajoute la touche
			Touche* touche = t->c->getTouche(m->id, noT, typeT);
			if(touche != nullptr){
				t->var->configTouchesToucheController = touche;
				tj->addTouche(touche);
			}
		}

		if(t->var->configTouchesToucheController == nullptr)
			t->ret = -1;
		else
		{
			// On affiche les propriétés de la touche
			Touche* touche = t->var->configTouchesToucheController;
		}
	}

	t->mutex.lock();
	t->isStop = true;
	t->mutex.unlock();

	return t->ret;
}

int main_(int argc, char* argv[]) {

    SDL_Window *window;                    // Declare a pointer

    SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2

    // Create an application window with the following settings:
    window = SDL_CreateWindow(
        "An SDL2 window",                  // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        640,                               // width, in pixels
        480,                               // height, in pixels
        SDL_WINDOW_OPENGL                  // flags - see below
    );

    // Check that the window was successfully created
    if (window == NULL) {
        // In the case that the window could not be made...
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    // The window is open: could enter program loop here (see SDL_PollEvent())
    // Setup renderer
    SDL_Renderer* renderer = NULL;
    renderer =  SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);

    // Set render color to red ( background will be rendered in this color )
    SDL_SetRenderDrawColor( renderer, 255, 0, 0, 255 );

    // Clear winow
    SDL_RenderClear( renderer );

    // Creat a rect at pos ( 50, 50 ) that's 50 pixels wide and 50 pixels high.
    SDL_Rect r;
    r.x = 50;
    r.y = 50;
    r.w = 50;
    r.h = 50;

    // Set render color to blue ( rect will be rendered in this color )
    SDL_SetRenderDrawColor( renderer, 0, 0, 255, 255 );

    // Render rect
    SDL_RenderFillRect( renderer, &r );

    // Render the rect to the screen
    SDL_RenderPresent(renderer);

    SDL_Delay(8000);  // Pause execution for 3000 milliseconds, for example

    // Close and destroy the window
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();
    return 0;
}

int main(int argc, char** argv)
{
	/* Initialisation simple */
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0 )
	{
		fprintf(stdout,"Échec de l'initialisation de la SDL (%s)\n", SDL_GetError());
		return -1;
	}
	SDL_LogSetAllPriority(LOG_LEVEL);
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "SDL chargé");
	if(TTF_Init() != 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Échec de l'initialisation de TTF (%s)\n", TTF_GetError());
		return -1;
	}
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "SDL_ttf chargé");
	if((IMG_Init(IMG_INIT_FORMAT) & IMG_INIT_FORMAT) != IMG_INIT_FORMAT)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Échec de l'initialisation de la SDL_Image (%s)\n", IMG_GetError());
		return -1;
	}
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "SDL_Image chargé");

	// Variables du jeu
	VarGame var;
	// On crée toutes les touches
	ToucheJeu* touches[NB_TOUCHES_JEU];
	var.touches = touches;
	for(int i=0; i<NB_TOUCHES_JEU; i++)
		touches[i] = nullptr;
	touches[TOUCHE_HAUT_BAS] = new ToucheJeu("haut/bas");
	touches[TOUCHE_GAUCHE_DROITE] = new ToucheJeu("gauche/droite");
	touches[TOUCHE_PLEIN_ECRAN] = new ToucheJeu("Plein écran");

	Affichage* aff = new Affichage("Elementario");
	Controller* c = new Controller(var.touches);
	c->update();

	// Boucle principale
	int etat = ETAT_CHARGEMENT_JEU;
	bool pe = false; // Plein écran
	int etatER = ETAT_ERREUR;
	int retER = 0;
	while(etat != ETAT_ERREUR && etat != ETAT_QUITTER)
	{
		// On crée le thread
		DataThread t;
		t.aff = aff;
		t.c = c;
		t.var = &var;

		// Selon l'état actuel, on lance le bon thread
		int (*func)(void*);
		func = nullptr;
		switch(etat)
		{
			case ETAT_CHARGEMENT_JEU:
				func = threadInit;
				break;

			case ETAT_CONFIG_TOUCHES_1:
				func = threadConfigTouches1;
				break;

			case ETAT_CONFIG_TOUCHES_2:
				func = threadConfigTouches2;
				break;

			case ETAT_CONFIG_TOUCHES_3:
				func = threadConfigTouches3;
				break;

			case ETAT_CONFIG_TOUCHES_4:
				func = threadConfigTouches4;
				break;

			case ETAT_CONFIG_TOUCHES_5:
				func = threadConfigTouches5;
				break;

			default:
				etat = ETAT_ERREUR;
				break;
		}

		// On crée et éxecute le thread
		if(func != nullptr)
		{
			std::thread th(func, &t);
			bool stopThread = false;
			while(!stopThread)
			{
				t.mutex.lock();
				stopThread = t.isStop;
				t.mutex.unlock();

				// On regarde si on doit demander au thread de s'arreter
				if(c->quitter())
				{
					t.mutex.lock();
					t.mustStop = true;
					t.mutex.unlock();
					etat = ETAT_QUITTER;

					// Gestion d'erreur
					etatER = ETAT_QUITTER;
					retER = 0;
				}

				// On regarde si on doit mettre/enlever le plein écran
				if(touches[TOUCHE_PLEIN_ECRAN]->getVal(true))
				{
					// On relâche la touche
					touches[TOUCHE_PLEIN_ECRAN]->setVal(0, false);
					// On change l'état
					int x = 0, y = 0;
					aff->modePleinEcran(pe = !pe, &x, &y);
					c->tailleFenetre(x, y);
				}

				aff->update();
				c->update();
			}
			// On attend que le thread ai bien quitté
			th.join();

			// Selon l'état, on traite le retour
			if(etat != ETAT_QUITTER)
			{
				// En cas d'erreur
				etatER = etat;
				retER = t.ret;
				switch(etat)
				{
					case ETAT_CHARGEMENT_JEU:
						etat = ETAT_CONFIG_TOUCHES_1;
						if(t.ret > 0)
							etat = ETAT_ERREUR;
						break;

					case ETAT_CONFIG_TOUCHES_1:
						if(t.ret == 2) // Flèche de gauche
							etat = ETAT_QUITTER;
						else
							etat = ETAT_CONFIG_TOUCHES_2;
						break;

					case ETAT_CONFIG_TOUCHES_2:
						if(t.ret == 2) // Flèche de gauche
							etat = ETAT_CONFIG_TOUCHES_1;
						else
							etat = ETAT_CONFIG_TOUCHES_3;
						break;

					case ETAT_CONFIG_TOUCHES_3:
						if(t.ret == 2) // Flèche de gauche
							etat = ETAT_CONFIG_TOUCHES_2;
						else
							etat = ETAT_CONFIG_TOUCHES_4;
						break;

					case ETAT_CONFIG_TOUCHES_4:
						if(t.ret > 2) // Flèche de gauche
							etat = ETAT_CONFIG_TOUCHES_3;
						else
							etat = ETAT_CONFIG_TOUCHES_5;
						break;

					case ETAT_CONFIG_TOUCHES_5:
							etat = ETAT_CONFIG_TOUCHES_4;
						break;

					default:
						etat = ETAT_QUITTER;
						break;
				}
			}
		}
		else
		{
			etatER = etat;
			retER = 0;
			etat = ETAT_ERREUR;
		}
	}

	delete aff;
	delete c;

	if(etat == ETAT_QUITTER)
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Jeu terminé correctement\n");
	else
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Jeu terminé avec une erreur,\nEtat:%d, retour:%d\n", etatER, retER);

	IMG_Quit();
	TTF_Quit();
	SDL_Quit();

	return 0;
}

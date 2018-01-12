#include <Affichage.h>
#include <Controller.h>
#include <ToucheJeu.h>
#include <Touche.h>
#include <defs.h>

//#include <jni.h>
typedef struct VarGame
{
	BundleTouche** bundles = nullptr; // Tableau des touchesJeu
	ToucheJeu** touchesJeu = nullptr; // Ensemble de toutes les touchesJeu
	int configTouchesCategorie = 0; // Catégorie actuelle dans l'écran de configuration des touches
	int configTouchesTouche = 0; // Touche actuelle dans l'écran de configuration des touches
	int configTouchesController = -2; // Controller actuel dans l'écran de configuration des touches
	int configTouchesBoutonController = 0; // Bouton du controller actuel dans l'écran de configuration des touches
	Touche* configTouchesToucheController = nullptr; // Bouton du controller actuel dans l'écran de configuration des touches
	std::mutex mutex; // Mutex
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
static int thread_Init(void* data) // TODO : Toutes les touches de base
{
	DataThread* t = (DataThread*)data;
	t->mutex.lock();
	t->isStop = false;
	t->mutex.unlock();

	// On crée toutes les touches-jeu // TODO : les noms et descriptions dans un fichier externe
	t->var->mutex.lock();
	t->var->touchesJeu[TOUCHE_PLEIN_ECRAN] = new ToucheJeu("plein écran", "Change le mode d'affichage du jeu entre fenêtré et plein écran", 0, 0, 1);
	t->var->touchesJeu[TOUCHE_AFFICHAGE_FPS] = new ToucheJeu("afficher les fps", "Affiche ou cache le nombre d'images par secondes calculés par l'ordinateur (en bas à gauche)", 0, 0, 1);

	t->var->touchesJeu[TOUCHE_NAVIGATION_DEPLACER_AXE_HAUT_BAS] = new ToucheJeu("déplacement vertical", "Déplace les boutons du menu vers le haut ou vers le bas");
	t->var->touchesJeu[TOUCHE_NAVIGATION_DEPLACER_AXE_GAUCHE_DROITE] = new ToucheJeu("déplacement horizontal", "Déplace les boutons des catégories vers la gauche ou vers la droite");
	t->var->touchesJeu[TOUCHE_NAVIGATION_SELECTION_BOUTON_HAUT_BAS] = new ToucheJeu("sélection verticale (bouton)", "Sélectionne le bouton vers le haut ou vers le bas", 0, -1, 1);
	t->var->touchesJeu[TOUCHE_NAVIGATION_SELECTION_BOUTON_GAUCHE_DROITE] = new ToucheJeu("sélection horizontale (bouton)", "Sélectionne le bouton vers à gauche ou vers la droite", 0, -1, 1);
	t->var->touchesJeu[TOUCHE_NAVIGATION_SELECTION_AXE_HAUT_BAS] = new ToucheJeu("sélection verticale (axe)", "Sélectionne les boutons affichés à l'écran, vers le haut ou vers le bas");
	t->var->touchesJeu[TOUCHE_NAVIGATION_SELECTION_AXE_GAUCHE_DROITE] = new ToucheJeu("sélection horizontale (axe)", "Sélectionne les boutons affichés à l'écran, vers la gauche ou vers la droite");
	t->var->touchesJeu[TOUCHE_NAVIGATION_VALIDER] = new ToucheJeu("valider", "Valide la sélection actuelle, le bouton actuellement sélectionné", 0, 0, 1);
	t->var->touchesJeu[TOUCHE_NAVIGATION_RETOUR] = new ToucheJeu("annuler", "Annule le menu et reviens à l'écran précédent", 0, 0, 1);
	t->var->touchesJeu[TOUCHE_NAVIGATION_AVANCE] = new ToucheJeu("avancé", "Accède à des options avancées ou des infos, pour la sélection actuelle", 0, 0, 1);

	t->var->touchesJeu[TOUCHE_ANGULAIRE_BOUTON_ANGLE] = new ToucheJeu("angle (bouton)", "sélectionne le bouton suivant/précédent", 0, -1, 1);
	t->var->touchesJeu[TOUCHE_ANGULAIRE_AXE_ANGLE] = new ToucheJeu("angle (axe)", "Angle actuel du joystic, pour sélectionner le bouton correspondant", 0, 0, 360);
	t->var->touchesJeu[TOUCHE_ANGULAIRE_VALIDER] = new ToucheJeu("valider", "Valide la sélection actuelle, le bouton actuellement sélectionné", 0, 0, 1);
	t->var->touchesJeu[TOUCHE_ANGULAIRE_RETOUR] = new ToucheJeu("annuler", "Annule le menu et reviens à l'écran précédent", 0, 0, 1);
	t->var->touchesJeu[TOUCHE_ANGULAIRE_AVANCE] = new ToucheJeu("avancé", "Accède à des options avancées ou des infos, pour la sélection actuelle", 0, 0, 1);

	t->var->touchesJeu[TOUCHE_PLATEAU_MENU] = new ToucheJeu("menu", "Accède au menu principale pendant la partie", 0, 0, 1);
	t->var->touchesJeu[TOUCHE_PLATEAU_DEPLACER_X] = new ToucheJeu("déplacement X", "Déplace le plateau de jeu verticalement");
	t->var->touchesJeu[TOUCHE_PLATEAU_DEPLACER_Y] = new ToucheJeu("déplacement Y", "Déplace le plateau de jeu horizontalement");
	t->var->touchesJeu[TOUCHE_PLATEAU_SELECTION_BOUTON_HAUT_BAS] = new ToucheJeu("sélection verticale (bouton)", "Sélectionne la case du plateau, juste au dessus ou juste en dessous");
	t->var->touchesJeu[TOUCHE_PLATEAU_SELECTION_BOUTON_GAUCHE_DROITE] = new ToucheJeu("sélection horizontale (bouton)", "Sélectionne la case du plateau, juste à gauche ou juste à droite");
	t->var->touchesJeu[TOUCHE_PLATEAU_SELECTION_AXE_HAUT_BAS] = new ToucheJeu("sélection verticale (axe)", "Sélectionne les cases du plateau, vers le haut ou vers le bas");
	t->var->touchesJeu[TOUCHE_PLATEAU_SELECTION_AXE_GAUCHE_DROITE] = new ToucheJeu("sélection horizontale (axe)", "Sélectionne les cases du plateau, vers la gauche ou vers la droite");
	t->var->touchesJeu[TOUCHE_PLATEAU_VALIDER_CASE] = new ToucheJeu("action", "Valide la case du plateau actuellement sélectionnée et affiche les actions possibles", 0, 0, 1);
	t->var->touchesJeu[TOUCHE_PLATEAU_ZOOM] = new ToucheJeu("zoom", "Zoom ou dézoom l'affichage du plateau");
	t->var->mutex.unlock();

	// Si le fichier de configuration existe, on le lit // TODO : automatiser à partir d'un fichier externe
	// Sinon on crée les touches de base
	//else
	{
		Touche* touche = t->c->getTouche(-1, SDLK_F11, TYPE_TOUCHE_BOUTON); // plein écran
		if(touche)
		{
			LiaisonTouche* lt = new LiaisonTouche(t->var->touchesJeu[TOUCHE_PLEIN_ECRAN], touche);
			lt->setMode(MODE_APPUIE_UNIQUE);
			t->var->bundles[BUNDLE_PERSISTANT]->ajouterLiaison(lt);
		}

		touche = t->c->getTouche(-1, SDLK_UP, TYPE_TOUCHE_BOUTON);
		if(touche)
		{
			LiaisonTouche* lt = new LiaisonTouche(t->var->touchesJeu[TOUCHE_NAVIGATION_SELECTION_BOUTON_HAUT_BAS], touche);
			lt->setMode(MODE_APPUIE_UNIQUE);
			lt->setMinMaxTJ(0, -1);
			t->var->bundles[BUNDLE_NAVIGATION]->ajouterLiaison(lt);
		}

		touche = t->c->getTouche(-1, SDLK_DOWN, TYPE_TOUCHE_BOUTON);
		if(touche)
		{
			LiaisonTouche* lt = new LiaisonTouche(t->var->touchesJeu[TOUCHE_NAVIGATION_SELECTION_BOUTON_HAUT_BAS], touche);
			lt->setMode(MODE_APPUIE_UNIQUE);
			lt->setMinMaxTJ(0, 1);
			t->var->bundles[BUNDLE_NAVIGATION]->ajouterLiaison(lt);
		}

		touche = t->c->getTouche(-1, SDLK_LEFT, TYPE_TOUCHE_BOUTON);
		if(touche)
		{
			LiaisonTouche* lt = new LiaisonTouche(t->var->touchesJeu[TOUCHE_NAVIGATION_SELECTION_BOUTON_GAUCHE_DROITE], touche);
			lt->setMode(MODE_APPUIE_UNIQUE);
			lt->setMinMaxTJ(0, -1);
			t->var->bundles[BUNDLE_NAVIGATION]->ajouterLiaison(lt);
		}

		touche = t->c->getTouche(-1, SDLK_RIGHT, TYPE_TOUCHE_BOUTON);
		if(touche)
		{
			LiaisonTouche* lt = new LiaisonTouche(t->var->touchesJeu[TOUCHE_NAVIGATION_SELECTION_BOUTON_GAUCHE_DROITE], touche);
			lt->setMode(MODE_APPUIE_UNIQUE);
			lt->setMinMaxTJ(0, 1);
			t->var->bundles[BUNDLE_NAVIGATION]->ajouterLiaison(lt);
		}

		touche = t->c->getTouche(-1, SDLK_RETURN, TYPE_TOUCHE_BOUTON);
		if(touche)
		{
			LiaisonTouche* lt = new LiaisonTouche(t->var->touchesJeu[TOUCHE_NAVIGATION_VALIDER], touche);
			lt->setMode(MODE_APPUIE_UNIQUE);
			t->var->bundles[BUNDLE_NAVIGATION]->ajouterLiaison(lt);
		}

		touche = t->c->getTouche(-1, SDLK_KP_ENTER, TYPE_TOUCHE_BOUTON);
		if(touche)
		{
			LiaisonTouche* lt = new LiaisonTouche(t->var->touchesJeu[TOUCHE_NAVIGATION_VALIDER], touche);
			lt->setMode(MODE_APPUIE_UNIQUE);
			t->var->bundles[BUNDLE_NAVIGATION]->ajouterLiaison(lt);
		}

		touche = t->c->getTouche(-1, SDLK_ESCAPE, TYPE_TOUCHE_BOUTON);
		if(touche)
		{
			LiaisonTouche* lt = new LiaisonTouche(t->var->touchesJeu[TOUCHE_NAVIGATION_RETOUR], touche);
			lt->setMode(MODE_APPUIE_UNIQUE);
			t->var->bundles[BUNDLE_NAVIGATION]->ajouterLiaison(lt);
		}

		touche = t->c->getTouche(-2, 1, TYPE_TOUCHE_JOYSTIC); // Pointeur X
		if(touche)
		{
			LiaisonTouche* lt = new LiaisonTouche(t->var->touchesJeu[TOUCHE_NAVIGATION_SELECTION_AXE_GAUCHE_DROITE], touche);
			lt->setMode(MODE_AXE_ABSOLUE);
			t->var->bundles[BUNDLE_NAVIGATION]->ajouterLiaison(lt);
		}

		touche = t->c->getTouche(-2, 2, TYPE_TOUCHE_JOYSTIC); // Pointeur Y
		if(touche)
		{
			LiaisonTouche* lt = new LiaisonTouche(t->var->touchesJeu[TOUCHE_NAVIGATION_SELECTION_AXE_HAUT_BAS], touche);
			lt->setMode(MODE_AXE_ABSOLUE);
			t->var->bundles[BUNDLE_NAVIGATION]->ajouterLiaison(lt);
		}

		touche = t->c->getTouche(-2, 1, TYPE_TOUCHE_MOLETTE); // Molette X
		if(touche)
		{
			LiaisonTouche* lt = new LiaisonTouche(t->var->touchesJeu[TOUCHE_NAVIGATION_DEPLACER_AXE_GAUCHE_DROITE], touche);
			lt->setMode(MODE_AXE_RELATIF);
			t->var->bundles[BUNDLE_NAVIGATION]->ajouterLiaison(lt);
		}

		touche = t->c->getTouche(-2, 2, TYPE_TOUCHE_MOLETTE); // Molette Y
		if(touche)
		{
			LiaisonTouche* lt = new LiaisonTouche(t->var->touchesJeu[TOUCHE_NAVIGATION_DEPLACER_AXE_HAUT_BAS], touche);
			lt->setMode(MODE_AXE_RELATIF);
			t->var->bundles[BUNDLE_NAVIGATION]->ajouterLiaison(lt);
		}
	}

	// On initialise les graphismes
	t->ret = t->aff->init();
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Jeu chargé\n");

	t->mutex.lock();
	t->isStop = true;
	t->mutex.unlock();

	return t->ret;
}

// Configuration des touches étape 1 : sélection de la catégorie et du bouton
// Retour : -1 pour retour, [n° touche] pour suivant, -2 pour erreur
static int thread_ConfigTouches_categories(void* data)
{
	DataThread* t = (DataThread*)data;
	t->mutex.lock();
	t->isStop = false;
	t->ret = -2;
	t->mutex.unlock();

	// On affiche le menu
	t->c->setBundle(t->var->bundles[BUNDLE_NAVIGATION]);
	t->aff->setEcran(MODE_AFFICHAGE_CONFIG_TOUCHES_CATEGORIES);
	Menu* menu = t->aff->getMenu();

	bool stop = false;
	bool modif = true;

	while(!stop)
	{
		SDL_Delay(15);
		t->mutex.lock();
		stop = t->mustStop;
		t->mutex.unlock();

		// On regarde si l'utilisateur a fait son choix sur le menu
		int result = menu->getResult();
		if(result > 0)
		{
			t->mutex.lock();
			t->ret = result-1;
			t->isStop = true;
			t->mutex.unlock();
		}
		else if(result == -1)
		{
			t->mutex.lock();
			t->ret = result;
			t->isStop = true;
			t->mutex.unlock();
		}
	}

	t->mutex.lock();
	t->isStop = true;
	t->mutex.unlock();

	return t->ret;
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
	std::string path = "";
	if(argc > 1){
		path = argv[1];
		path += "/";
	}
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Chemin d'accès aux fichiers : %s", path.c_str());

	// On crée tous les bundles
	BundleTouche* bundlePersistant = new BundleTouche();
	BundleTouche* bundleNavigation = new BundleTouche();
	BundleTouche* bundleSelectionAngulaire = new BundleTouche();
	BundleTouche* bundlePlateau = new BundleTouche();

	// Variables du jeu
	VarGame var;
	var.bundles = new BundleTouche*[4];
	var.bundles[BUNDLE_PERSISTANT] = bundlePersistant;
	var.bundles[BUNDLE_NAVIGATION] = bundleNavigation;
	var.bundles[BUNDLE_ANGULAIRE] = bundleSelectionAngulaire;
	var.bundles[BUNDLE_PLATEAU] = bundlePlateau;
	ToucheJeu* touchesJeu[NB_TOUCHES_JEU];
	for(int i=0; i<NB_TOUCHES_JEU; i++)
		touchesJeu[i] = nullptr;
	var.touchesJeu = touchesJeu;
	Affichage* aff = new Affichage("Elementario", path, touchesJeu);
	Controller* c = new Controller(bundlePersistant);
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
				func = thread_Init;
				break;

			case ETAT_CONFIG_TOUCHES_CATEGORIES:
				func = thread_ConfigTouches_categories;
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
				var.mutex.lock();
				if(var.touchesJeu[TOUCHE_PLEIN_ECRAN] != nullptr)
					if(var.touchesJeu[TOUCHE_PLEIN_ECRAN]->getVal() > 0 && var.touchesJeu[TOUCHE_PLEIN_ECRAN]->isEvent())
					{
						// On relâche la touche
						var.touchesJeu[TOUCHE_PLEIN_ECRAN]->setVal(0);
						// On change l'état
						int x = 0, y = 0;
						aff->modePleinEcran(pe = !pe, &x, &y); // TODO : corriger bug !!!
						c->tailleFenetre(x, y);
					}
				var.mutex.unlock();

				// On met à jour les entrées/sorties
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
						etat = ETAT_CONFIG_TOUCHES_CATEGORIES;
						if(t.ret > 0)
							etat = ETAT_ERREUR;
						else
							aff->init_main();
						break;

					case ETAT_CONFIG_TOUCHES_CATEGORIES:
						if(t.ret == 2) // Bouton annuler
							etat = ETAT_QUITTER;
						else
							etat = ETAT_QUITTER; // TODO : a changer
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

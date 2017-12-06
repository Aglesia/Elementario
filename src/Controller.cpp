#include "Controller.h"

Controller::Controller(ToucheJeu** touchesJeu)
{
	this->touchesJeu = touchesJeu;
	this->manettes.clear();
	this->touchesInput.clear();
	// Gestion des évènements pour les manettes
	SDL_JoystickEventState(SDL_ENABLE);
	this->manetteSouris = new Manette();
	this->manetteSouris->nbBouton = NB_TOUCHES_SOURIS;
	this->manetteSouris->nbJoystick = 2;
	this->manetteSouris->nbMolette = 2;
	this->manetteSouris->nbDirection = 0;
	this->manetteSouris->name = "Souris";
	this->manetteSouris->id = -2;
	this->manetteClavier = new Manette();
	this->manetteClavier->nbBouton = 62; // TODO : corriger
	this->manetteClavier->nbJoystick = 0;
	this->manetteClavier->nbMolette = 0;
	this->manetteClavier->nbDirection = 0;
	this->manetteClavier->name = "Clavier";
	this->manetteClavier->id = -1;
	// Boutons souris
	for(int i=1; i<=NB_TOUCHES_SOURIS; i++)
	{
		Touche* t = new Touche(-2, i, TYPE_TOUCHE_BOUTON);
		this->touchesInput.push_back(t);
		t->configure = true;
		t->nom = "Bouton "+SSTR(i);
	}
	Touche* t = new Touche(-2, 1, TYPE_TOUCHE_MOLETTE); // Molette x
	this->touchesInput.push_back(t);
	t->setValeurs(-100, 100);
	t->configure = true;
	t->nom = "Molette X";

	t = new Touche(-2, 2, TYPE_TOUCHE_MOLETTE); // Molette y
	this->touchesInput.push_back(t);
	t->setValeurs(-100, 100);
	t->configure = true;
	t->nom = "Molette Y";

	t = new Touche(-2, 1, TYPE_TOUCHE_JOYSTIC); // Pointeur x
	this->touchesInput.push_back(t);
	t->setValeurs(0, TAILLE_FENETRE_X);
	t->configure = true;
	t->nom = "Pointeur X";

	t = new Touche(-2, 2, TYPE_TOUCHE_JOYSTIC); // Pointeur y
	this->touchesInput.push_back(t);
	t->setValeurs(0, TAILLE_FENETRE_Y);
	t->configure = true;
	t->nom = "Pointeur Y";

	// On crée les boutons du clavier
	for(int i=0; i<=127; i++)
	{
		t = new Touche(-1, i, TYPE_TOUCHE_BOUTON);
		t->nom = SDL_GetKeyName(i);
		t->configure = true;
		this->touchesInput.push_back(t);
	}
	for(int i=1073741881; i<=1073742106; i++)
	{
		t = new Touche(-1, i, TYPE_TOUCHE_BOUTON);
		t->nom = SDL_GetKeyName(i);
		t->configure = true;
		this->touchesInput.push_back(t);
	}

}

Controller::~Controller()
{
	// On ferme tous les contrôlleurs
	for(int i=0; i<this->manettes.size(); i++)
		delete this->manettes[i];
	for(int i=0; i<this->touchesInput.size(); i++)
		delete this->touchesInput[i];
	this->manettes.clear();
	this->touchesInput.clear();
}

void Controller::update()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		this->lock.lock();
		switch(event.type)
		{
			// Nouvelle manette connectée
			case SDL_JOYDEVICEADDED:
				SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Connexion d'un joystick : %d\n", event.jdevice.which);

				{
					SDL_Joystick* j = SDL_JoystickOpen(event.jdevice.which);
					if(j != nullptr){
						Manette* m = new Manette();
						this->manettes.push_back(m);
						m->joy = j;
						m->id = SDL_JoystickInstanceID(j);
						m->nbBouton = SDL_JoystickNumButtons(j);
						m->nbJoystick = SDL_JoystickNumAxes(j);
						m->nbDirection = SDL_JoystickNumHats(j);
						m->name = SDL_JoystickName(j);
						SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Nouvelle manette détectée : %s\n\tNombre d'axes : %d\n\tNombre de boutons : %d\n\tNombre de chapeaux : %d\n",
							   SDL_JoystickName(j), SDL_JoystickNumAxes(j), SDL_JoystickNumButtons(j), SDL_JoystickNumHats(j));

						// On crée chaque bouton
						for(int i=1; i<=m->nbBouton; i++)
						{
							Touche* t = new Touche(m->id, i, TYPE_TOUCHE_BOUTON);
							this->touchesInput.push_back(t);
							t->nom = "Bouton "+SSTR(i);
							t->configure = true;
							SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Nouveau bouton : %s\n", t->nom.c_str());
						}
						for(int i=1; i<=m->nbJoystick; i++)
						{
							Touche* t = new Touche(m->id, i, TYPE_TOUCHE_JOYSTIC);
							this->touchesInput.push_back(t);
							t->nom = "Axe "+SSTR(i);
							t->configure = true;
							SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Nouvel axe : %s\n", t->nom.c_str());
						}
						for(int i=1; i<=m->nbDirection*2; i+=2)
						{
							Touche* t = new Touche(m->id, i, TYPE_TOUCHE_DIRECTION);
							this->touchesInput.push_back(t);
							t->nom = "Croix direction "+SSTR(i);
							t->setValeurs(-100, 100);
							t->configure = true;
							SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Nouvel axe directionnel : %s\n", t->nom.c_str());

							t = new Touche(m->id, i+1, TYPE_TOUCHE_DIRECTION);
							this->touchesInput.push_back(t);
							t->nom = "Croix direction "+SSTR(i+1);
							t->setValeurs(-100, 100);
							t->configure = true;
							SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Nouvel axe directionnel : %s\n", t->nom.c_str());
						}

						this->lock.unlock();
						this->mapManette(m);
						this->lock.lock();
					}
				}

				break;

			// Manette déconnectée
			case SDL_JOYDEVICEREMOVED:
				SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Déconnexion d'un joystick : %d\n", event.jdevice.which);

				{
					//SDL_JoystickClose((SDL_Joystick*)event.jdevice.which);
					for(unsigned int i=0; i<this->manettes.size(); i++)
						if(this->manettes.at(i)->id == event.jdevice.which){
							SDL_JoystickClose(this->manettes.at(i)->joy);
							SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Adresse : %d\n", this->manettes.at(i)->joy);
							this->manettes.erase(this->manettes.begin() + i);
						}
				}
				break;

			case SDL_QUIT:
				this->doitQuitter = true;
				break;

			// Bouton clavier appuyé
			case SDL_KEYDOWN:
				{
					this->lock.unlock();
					Touche* t = this->getTouche(-1, event.key.keysym.sym, TYPE_TOUCHE_BOUTON);
					this->lock.lock();
					if(t != nullptr)
					{
						t->setPressed(true);
						this->event(t);
					}
					if(this->toucheEcoutee != nullptr)
					{
						if(this->toucheEcoutee->getNoManette() == -1)
						{
							this->toucheEcoutee->setNoTouche(event.key.keysym.sym);
							this->toucheEcoutee->setType(TYPE_TOUCHE_BOUTON);
							this->toucheEcoutee->setValAxe(this->toucheEcoutee->getValeurMax());
							this->toucheEcoutee->setPressed(true);
							this->toucheEcoutee = nullptr;
						}
					}
				}
				break;

			// Bouton clavier relâché
			case SDL_KEYUP:
				{
					this->lock.unlock();
					Touche* t = this->getTouche(-1, event.key.keysym.sym, TYPE_TOUCHE_BOUTON);
					this->lock.lock();
					if(t != nullptr)
					{
						t->setPressed(false);
						this->event(t);
					}
					if(this->toucheEcoutee != nullptr)
					{
						if(this->toucheEcoutee->getNoManette() == -1)
						{
							this->toucheEcoutee->setNoTouche(event.key.keysym.sym);
							this->toucheEcoutee->setType(TYPE_TOUCHE_BOUTON);
							this->toucheEcoutee->setValAxe(this->toucheEcoutee->getValeurMin());
							this->toucheEcoutee->setPressed(false);
							this->toucheEcoutee = nullptr;
						}
					}
				}
				break;

			// Déplacement de la souris
			case SDL_MOUSEMOTION:
				{
					this->lock.unlock();
					Touche* tx = this->getTouche(-2, 1, TYPE_TOUCHE_JOYSTIC);
					Touche* ty = this->getTouche(-2, 2, TYPE_TOUCHE_JOYSTIC);
					this->lock.lock();
					if(tx != nullptr)
					{
						tx->setValAxe(event.motion.x);
						this->event(tx);
						if(this->toucheEcoutee != nullptr)
						{
							if(this->toucheEcoutee->getNoManette() == -2)
							{
								this->toucheEcoutee->setNoTouche(1);
								this->toucheEcoutee->setType(TYPE_TOUCHE_JOYSTIC);
								this->toucheEcoutee->setValAxe(event.motion.x);
								this->toucheEcoutee = nullptr;
							}
						}
					}
					if(ty != nullptr)
					{
						ty->setValAxe(event.motion.y);
						this->event(ty);
						if(this->toucheEcoutee != nullptr)
						{
							if(this->toucheEcoutee->getNoManette() == -2)
							{
								this->toucheEcoutee->setNoTouche(2);
								this->toucheEcoutee->setType(TYPE_TOUCHE_JOYSTIC);
								this->toucheEcoutee->setValAxe(event.motion.y);
								this->toucheEcoutee = nullptr;
							}
						}
					}
				}
				break;

			// Bouton souris appuyé
			case SDL_MOUSEBUTTONDOWN:
				{
					this->lock.unlock();
					Touche* t = this->getTouche(-2, event.button.button, TYPE_TOUCHE_BOUTON);
					this->lock.lock();
					if(t != nullptr)
					{
						t->setPressed(true);
						this->event(t);
					}
					if(this->toucheEcoutee != nullptr)
					{
						if(this->toucheEcoutee->getNoManette() == -2)
						{
							this->toucheEcoutee->setNoTouche(event.button.button);
							this->toucheEcoutee->setType(TYPE_TOUCHE_BOUTON);
							this->toucheEcoutee->setValAxe(this->toucheEcoutee->getValeurMax());
							this->toucheEcoutee->setPressed(true);
							this->toucheEcoutee = nullptr;
						}
					}
				}
				break;

			// Bouton souris relâché
			case SDL_MOUSEBUTTONUP:
				{
					this->lock.unlock();
					Touche* t = this->getTouche(-2, event.button.button, TYPE_TOUCHE_BOUTON);
					this->lock.lock();
					if(t != nullptr)
					{
						t->setPressed(false);
						this->event(t);
						if(this->toucheEcoutee != nullptr)
						{
							if(this->toucheEcoutee->getNoManette() == -2)
							{
								this->toucheEcoutee->setNoTouche(event.button.button);
								this->toucheEcoutee->setType(TYPE_TOUCHE_BOUTON);
								this->toucheEcoutee->setValAxe(this->toucheEcoutee->getValeurMin());
								this->toucheEcoutee->setPressed(false);
								this->toucheEcoutee = nullptr;
							}
						}
					}
				}
				break;

			// Molette de la souris
			case SDL_MOUSEWHEEL:
				{
					this->lock.unlock();
					Touche* tx = this->getTouche(-2, 1, TYPE_TOUCHE_MOLETTE);
					Touche* ty = this->getTouche(-2, 2, TYPE_TOUCHE_MOLETTE);
					this->lock.lock();
					if(tx != nullptr)
					{
						tx->setValAxe(event.wheel.x);
						this->event(tx);
						if(this->toucheEcoutee != nullptr)
						{
							if(this->toucheEcoutee->getNoManette() == -2)
							{
								this->toucheEcoutee->setNoTouche(1);
								this->toucheEcoutee->setType(TYPE_TOUCHE_MOLETTE);
								this->toucheEcoutee->setValAxe(event.motion.y);
								this->toucheEcoutee = nullptr;
							}
						}
					}
					if(ty != nullptr)
					{
						ty->setValAxe(event.wheel.y);
						this->event(ty);
						if(this->toucheEcoutee != nullptr)
						{
							if(this->toucheEcoutee->getNoManette() == -2)
							{
								this->toucheEcoutee->setNoTouche(2);
								this->toucheEcoutee->setType(TYPE_TOUCHE_MOLETTE);
								this->toucheEcoutee->setValAxe(event.motion.y);
								this->toucheEcoutee = nullptr;
							}
						}
					}
				}
				break;

			// Déplacement d'un joystick
			case SDL_JOYAXISMOTION:
				{
					this->lock.unlock();
					Touche* t = this->getTouche(event.jaxis.which, event.jaxis.axis+1, TYPE_TOUCHE_JOYSTIC);
					this->lock.lock();
					if(t != nullptr)
					{
						// On calcul la direction
						t->setValAxe(event.jaxis.value);
						this->event(t);
						if(this->toucheEcoutee != nullptr)
						{
							if(this->toucheEcoutee->getNoManette() == event.jaxis.which)
							{
								this->toucheEcoutee->setNoTouche(event.jaxis.axis+1);
								this->toucheEcoutee->setType(TYPE_TOUCHE_JOYSTIC);
								this->toucheEcoutee->setValAxe(event.jaxis.value);
								this->toucheEcoutee = nullptr;
							}
						}
					}
				}
				break;

			// Bouton d'une manette appuyé
			case SDL_JOYBUTTONDOWN:
				{
					this->lock.unlock();
					Touche* t = this->getTouche(event.jbutton.which, event.jbutton.button+1, TYPE_TOUCHE_BOUTON);
					this->lock.lock();
					if(t != nullptr)
					{
						t->setPressed(true);
						this->event(t);
					}
					if(this->toucheEcoutee != nullptr)
					{
						if(this->toucheEcoutee->getNoManette() == event.jbutton.which)
						{
							this->toucheEcoutee->setNoTouche(event.jbutton.button+1);
							this->toucheEcoutee->setType(TYPE_TOUCHE_BOUTON);
							this->toucheEcoutee->setValAxe(this->toucheEcoutee->getValeurMax());
							this->toucheEcoutee->setPressed(true);
							this->toucheEcoutee = nullptr;
						}
					}
				}
				break;

			// Bouton d'une manette relâché
			case SDL_JOYBUTTONUP:
				{
					this->lock.unlock();
					Touche* t = this->getTouche(event.jbutton.which, event.jbutton.button+1, TYPE_TOUCHE_BOUTON);
					this->lock.lock();
					if(t != nullptr)
					{
						t->setPressed(false);
						this->event(t);
					}
					if(this->toucheEcoutee != nullptr)
					{
						if(this->toucheEcoutee->getNoManette() == event.jbutton.which)
						{
							this->toucheEcoutee->setNoTouche(event.jbutton.button+1);
							this->toucheEcoutee->setType(TYPE_TOUCHE_BOUTON);
							this->toucheEcoutee->setValAxe(this->toucheEcoutee->getValeurMin());
							this->toucheEcoutee->setPressed(false);
							this->toucheEcoutee = nullptr;
						}
					}
				}
				break;

			// Trackball d'une manette a bougée
			case SDL_JOYBALLMOTION:
				SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Trackball\n");
				break;

			// Chapeau d'une manette a bougé
			case SDL_JOYHATMOTION:
				{
					this->lock.unlock();
					Touche* t = this->getTouche(event.jbutton.which, (event.jhat.hat*2)+1, TYPE_TOUCHE_DIRECTION);
					this->lock.lock();
					if(t != nullptr)
					{
						// On calcul la direction
						switch(event.jhat.value)
						{
							case SDL_HAT_LEFT:
							case SDL_HAT_LEFTUP:
							case SDL_HAT_LEFTDOWN:
								t->setValAxe(-100);
								break;

							case SDL_HAT_RIGHT:
							case SDL_HAT_RIGHTUP:
							case SDL_HAT_RIGHTDOWN:
								t->setValAxe(100);
								break;

							default:
								t->setValAxe(0);
						}
						this->event(t);
						if(this->toucheEcoutee != nullptr)
						{
							if(this->toucheEcoutee->getNoManette() == event.jbutton.which)
							{
								this->toucheEcoutee->setNoTouche((event.jhat.hat*2)+1);
								this->toucheEcoutee->setType(TYPE_TOUCHE_DIRECTION);
								this->toucheEcoutee->setPressed(t->getValBrutAxe() != 0);
								this->toucheEcoutee = nullptr;
							}
						}
					}
					this->lock.unlock();
					t = this->getTouche(event.jbutton.which, (event.jhat.hat*2)+2, TYPE_TOUCHE_DIRECTION);
					this->lock.lock();
					if(t != nullptr)
					{
						// On calcul la direction
						switch(event.jhat.value)
						{
							case SDL_HAT_UP:
							case SDL_HAT_RIGHTUP:
							case SDL_HAT_LEFTUP:
								t->setValAxe(-100);
								break;

							case SDL_HAT_DOWN:
							case SDL_HAT_RIGHTDOWN:
							case SDL_HAT_LEFTDOWN:
								t->setValAxe(100);
								break;

							default:
								t->setValAxe(0);
						}
						this->event(t);
						if(this->toucheEcoutee != nullptr)
						{
							if(this->toucheEcoutee->getNoManette() == event.jbutton.which)
							{
								this->toucheEcoutee->setNoTouche((event.jhat.hat*2)+2);
								this->toucheEcoutee->setType(TYPE_TOUCHE_DIRECTION);
								this->toucheEcoutee->setPressed(t->getValBrutAxe() != 0);
								this->toucheEcoutee = nullptr;
							}
						}
					}
				}
				break;

			// Redimensionnement
			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_SHOWN:
						SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Window %d shown", event.window.windowID);
						break;
					case SDL_WINDOWEVENT_HIDDEN:
						SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Window %d hidden", event.window.windowID);
						break;
					case SDL_WINDOWEVENT_EXPOSED:
						SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Window %d exposed", event.window.windowID);
						break;
					case SDL_WINDOWEVENT_MOVED:
						SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Window %d moved to %d,%d",
								event.window.windowID, event.window.data1,
								event.window.data2);
						break;
					case SDL_WINDOWEVENT_RESIZED:
						SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Window %d resized to %dx%d",
								event.window.windowID, event.window.data1,
								event.window.data2);
						break;
					case SDL_WINDOWEVENT_SIZE_CHANGED:
						SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Window %d size changed to %dx%d",
								event.window.windowID, event.window.data1,
								event.window.data2);
						break;
					case SDL_WINDOWEVENT_MINIMIZED:
						SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Window %d minimized", event.window.windowID);
						break;
					case SDL_WINDOWEVENT_MAXIMIZED:
						SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Window %d maximized", event.window.windowID);
						break;
					case SDL_WINDOWEVENT_RESTORED:
						SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Window %d restored", event.window.windowID);
						break;
					case SDL_WINDOWEVENT_ENTER:
						SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Mouse entered window %d",
								event.window.windowID);
						break;
					case SDL_WINDOWEVENT_LEAVE:
						SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Mouse left window %d", event.window.windowID);
						break;
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Window %d gained keyboard focus",
								event.window.windowID);
						break;
					case SDL_WINDOWEVENT_FOCUS_LOST:
						SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Window %d lost keyboard focus",
								event.window.windowID);
						break;
					case SDL_WINDOWEVENT_CLOSE:
						SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Window %d closed", event.window.windowID);
						break;

					default:
						break;
				}
				break;

			default:
				break;
		}
		this->lock.unlock();
	}
}

bool Controller::quitter()
{
	bool ret = false;
	this->lock.lock();
	ret = this->doitQuitter;
	this->lock.unlock();
	return ret;
}

Touche* Controller::getTouche(int noJoystic, int noBouton, int typeBouton)
{
	Touche* ret = nullptr;
	this->lock.lock();
	for(int i=0; i<this->touchesInput.size(); i++)
		if(this->touchesInput[i]->isTouche(noJoystic, noBouton, typeBouton))
			ret = this->touchesInput[i];
	this->lock.unlock();
	return ret;
}

void Controller::event(Touche* t)
{
	// On indique à tout le monde que la touche vient de changer
	for(int i=0; i<NB_TOUCHES_JEU; i++)
		if(this->touchesJeu[i] != nullptr)
			this->touchesJeu[i]->nouvelEvenement(t);
}

std::vector<Touche*> Controller::getTouches()
{
	std::vector<Touche*> ret;
	this->lock.lock();
	ret = this->touchesInput,
	this->lock.unlock();
	return ret;
}

void Controller::mapManette(Manette* m) // TODO
{
	if(this->touchesJeu != nullptr)
	{
		this->lock.lock();
		// On regarde si une configuration correspondant à la manette existe (même nom, nb touches, nb axe et nb direction)
		// Si oui, on map en suivant cette configuration
		// Sinon on fait une configuration de base
		//else
		{
			// Touches directionnelles : se déplacer dans les menus
			for(int i=1; i<=m->nbDirection*2; i+=2)
			{
				this->lock.unlock();
				Touche* t = this->getTouche(m->id, i+1, TYPE_TOUCHE_DIRECTION);
				this->lock.lock();
				if(t != nullptr && this->touchesJeu[TOUCHE_HAUT_BAS] != nullptr)
					this->touchesJeu[TOUCHE_HAUT_BAS]->addTouche(t);
				this->lock.unlock();
				t = this->getTouche(m->id, i, TYPE_TOUCHE_DIRECTION);
				this->lock.lock();
				if(t != nullptr && this->touchesJeu[TOUCHE_GAUCHE_DROITE] != nullptr)
					this->touchesJeu[TOUCHE_GAUCHE_DROITE]->addTouche(t);
			}
		}
		this->lock.unlock();
	}
}

Manette* Controller::getController(unsigned int noC)
{
	Manette* ret = nullptr;
	this->lock.lock();
	if(noC == -2)
		ret = this->manetteSouris;
	else if(noC == -1)
		ret = this->manetteClavier;
	else if(noC<this->manettes.size())
		ret = this->manettes[noC];
	this->lock.unlock();
	return ret;
}

int Controller::nbController()
{
	int ret = 0;
	this->lock.lock();
	ret = this->manettes.size();
	this->lock.unlock();
	return ret;
}

void Controller::listen(Touche* t)
{
	this->lock.lock();
	this->toucheEcoutee = t;
	this->lock.unlock();
}

void Controller::tailleFenetre(int x, int y)
{
	this->lock.lock();
	// On met à jour les axes X/Y
	Touche* t = this->getTouche(-2, 1, TYPE_TOUCHE_JOYSTIC); // Pointeur x
	t->setValeurs(0, x);
	t = this->getTouche(-2, 2, TYPE_TOUCHE_JOYSTIC); // Pointeur y
	t->setValeurs(0, y);
	this->lock.unlock();
}
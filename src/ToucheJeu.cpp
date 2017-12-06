#include <ToucheJeu.h>

ToucheJeu::ToucheJeu(std::string nom)
{
	this->nom = nom;
	this->touches.clear();
}

ToucheJeu::~ToucheJeu()
{
	this->touches.clear();
}

void ToucheJeu::nouvelEvenement(Touche* touche)
{
	this->mutex.lock();
	if(this->actif)
	{
		// On regarde si la touche nous concerne
		for(int i=0; i<this->touches.size(); i++)
			if(this->touches[i] == touche)
			{
				// On met à jour notre valeur si besoin
				if(this->valeurAxe != touche->getValAxe() || this->valeurBool != touche->isPressed())
				{
					this->valeurAxe = touche->getValAxe();
					this->valeurBool = touche->isPressed();
					i=this->touches.size();
					SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Touche jeu %s : %d(%d)\n", this->nom.c_str(), this->valeurAxe, this->valeurBool);
				}
			}
	}
	this->mutex.unlock();
}

void ToucheJeu::activer(bool actif)
{
	this->mutex.lock();
	this->actif = actif;
	this->mutex.unlock();
}

int ToucheJeu::getVal(bool boolean)
{
	int ret = 0;
	this->mutex.lock();
	if(boolean)
		ret = this->valeurBool;
	else
		ret = this->valeurAxe;
	this->mutex.unlock();
	return ret;
}

void ToucheJeu::addTouche(Touche* touche)
{
	if(!touchePresente(touche))
	{
		this->mutex.lock();
		SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Touche %s ajouté à %s\n", touche->nom.c_str(), this->nom.c_str());
		this->touches.push_back(touche);
		this->mutex.unlock();
	}
}

void ToucheJeu::removeTouche(Touche* touche)
{
	this->mutex.lock();
	for(int i=0; i<this->touches.size(); i++)
		if(this->touches[i] == touche)
		{
			SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Touche %s retirée de %s\n", touche->nom.c_str(), this->nom.c_str());
			this->touches.erase(this->touches.begin() + i);
		}
	this->mutex.unlock();
}

bool ToucheJeu::touchePresente(Touche* touche)
{
	bool ret = false;
	this->mutex.lock();
	for(int i=0; i<this->touches.size(); i++)
		if(this->touches[i] == touche)
			ret = true;
	this->mutex.unlock();
	return ret;
}

void ToucheJeu::setVal(int axe, bool boolean)
{
	this->mutex.lock();
	this->valeurAxe = axe;
	this->valeurBool = boolean;
	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Valeur touche \"%s\" modifiée : %d(%d)\n", this->nom.c_str(), axe, boolean);
	this->mutex.unlock();
}
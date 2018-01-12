#include <ToucheJeu.h>

ToucheJeu::ToucheJeu(std::string nom, std::string description, int valeurDefaut, int min, int max)
{
	this->nom = nom;
	this->description = description;
	this->valeurAxe = valeurDefaut;
	this->def = valeurDefaut;
	this->min = min;
	this->max = max;
}

ToucheJeu::~ToucheJeu(){}

int ToucheJeu::getVal()
{
	int ret = 0;
	this->mutex.lock();
	ret = this->valeurAxe;
	this->mutex.unlock();
	return ret;
}

void ToucheJeu::setVal(int val)
{
	this->mutex.lock();
	this->valeurAxe = val;
	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Valeur touche \"%s\" modifiée : %d\n", this->nom.c_str(), val);
	this->mutex.unlock();
}

void ToucheJeu::setEvent()
{
	this->mutex.lock();
	this->event = true;
	this->mutex.unlock();
}

bool ToucheJeu::isEvent()
{
	bool ret = false;
	this->mutex.lock();
	ret = this->event;
	this->event = false;
	this->mutex.unlock();
	return ret;
}

void ToucheJeu::setDefaut()
{
	this->mutex.lock();
	this->event = false;
	this->valeurAxe = this->def;
	this->mutex.unlock();
}

int ToucheJeu::getValeurMin()
{
	int ret = 0;
	this->mutex.lock();
	ret = this->min;
	this->mutex.unlock();
	return ret;
}

int ToucheJeu::getValeurMax()
{
	int ret = 0;
	this->mutex.lock();
	ret = this->max;
	this->mutex.unlock();
	return ret;
}


std::string ToucheJeu::getNom()
{
	return this->nom;
}

std::string ToucheJeu::getDescription()
{
	return this->description;
}
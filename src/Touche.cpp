#include <Touche.h>
#include "../include/Touche.h"

Touche::Touche(int noManette, int noTouche, int typeTouche)
{
	this->noManette = noManette;
	this->noTouche = noTouche;
	this->typeTouche = typeTouche;
}

Touche::~Touche(){}

bool Touche::isTouche(int noManette, int noTouche, int typeTouche)
{
	this->lock.lock();
	bool ret =  (this->noManette == noManette &&
				this->noTouche == noTouche &&
				this->typeTouche == typeTouche);
	this->lock.unlock();
	return ret;
}

/**
 * Retourne la valeur actuelle de l'axe
 * Si le bouton est une molette, remet sa valeur à 0
 * @param sys Utilisé par Controller.cpp
 * @return valeur de l'axe
 */
int Touche::getValAxe(bool sys)
{
	this->lock.lock();
	int ret = this->valeur;
	if(this->typeTouche == TYPE_TOUCHE_MOLETTE && !sys)
		this->valeur = 0;
	this->lock.unlock();
	return ret;
}

/**
 * Prend la nouvelle valeur brut de l'axe, et la transforme en une valeur entre -100 et 100
 * Si le bouton est une molette, ajoute la valeur au lieu de l'écraser
 * @param valeurBrut Valeur brut de l'axe
 * @param ignoreInversion Ignore l'inversion à effectuer sur l'axe
 */
void Touche::setValAxe(int valeurBrut, bool ignoreInversion)
{
	this->lock.lock();
	// Si c'est en dehors des limites, on le met aux limites
	if(valeurBrut<this->minBrut)
		valeurBrut = this->minBrut;
	if(valeurBrut>this->maxBrut)
		valeurBrut = this->maxBrut;

	// On calcul la nouvelle valeur
	if(this->typeTouche != TYPE_TOUCHE_MOLETTE)
		this->valeur = (((valeurBrut-this->minBrut)*(this->max-this->min))/(this->maxBrut-this->minBrut))+this->min;
	else
	{
		this->valeur += (((valeurBrut-this->minBrut)*(this->max-this->min))/(this->maxBrut-this->minBrut))+this->min;
		if(this->valeur > this->max)
			this->valeur = this->max;
		if(this->valeur < this->min)
			this->valeur = this->min;
	}

	// On inverse si besoin
	if(this->inversion && !ignoreInversion)
	{
		int val = this->valeur - this->min;
		this->valeur = this->max - val;
	}

	//SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "val %s : %d (brut=%d, min/max=%d/%d)", this->nom.c_str(), this->valeur, valeurBrut, this->minBrut, this->maxBrut);
	this->lock.unlock();
}

/**
 * Définit si le sens de l'axe ou l'etat du clic sont inversés
 * @param clic Si inversé, est appuyé lorsque le bouton est relâché
 * @param axe  Si inversé, bascule de -100 -> 100 à 100 -> -100
 */
void Touche::setInversion(bool inversion)
{
	this->lock.lock();
	this->inversion = inversion;
	this->lock.unlock();
}

/**
 * Définit les limites haute et basse de l'axe, pour la valeur logiques
 * Inverse l'axe si besoin
 * @param min Valeur minimum logique
 * @param max Valeur maximum logique
 */
void Touche::setValeurs(int min, int max)
{
	this->lock.lock();
	if(max > min)
	{
		this->min = min;
		this->max = max;
		this->inversion = false;
	}
	else
	{
		this->min = max;
		this->max = min;
		this->inversion = true;
	}
	this->lock.unlock();
	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Modification des limites virtuelles de %s : (%d.%d)", this->nom.c_str(), this->min, this->max);
}

/**
 * Définit les limites haute et basse de l'axe, pour la valeur brut
 * @param min Valeur minimum brut
 * @param max Valeur maximum brut
 */
void Touche::setValeursBrut(int min, int max)
{
	this->lock.lock();
	if(max > min)
	{
		this->minBrut = min;
		this->maxBrut = max;
	}
	else
	{
		this->minBrut = max;
		this->maxBrut = min;
	}
	this->lock.unlock();
	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Modification des limites bruts de %s : (%d.%d)", this->nom.c_str(), this->minBrut, this->maxBrut);
}

bool Touche::getInversion()
{
	bool ret;
	this->lock.lock();
	ret = this->inversion;
	this->lock.unlock();
	return ret;
}

int Touche::getValeurMin()
{
	int ret;
	this->lock.lock();
	ret = this->min;
	this->lock.unlock();
	return ret;
}

int Touche::getValeurMax()
{
	int ret;
	this->lock.lock();
	ret = this->max;
	this->lock.unlock();
	return ret;
}

int Touche::getNoManette()
{
	int ret = 0;
	this->lock.lock();
	ret = this->noManette;
	this->lock.unlock();
	return ret;
}

int Touche::getNoTouche()
{
	int ret = 0;
	this->lock.lock();
	ret = this->noTouche;
	this->lock.unlock();
	return ret;
}

int Touche::getType()
{
	int ret = 0;
	this->lock.lock();
	ret = this->typeTouche;
	this->lock.unlock();
	return ret;
}

int Touche::setNoManette(int noManette)
{
	int ret = 0;
	this->lock.lock();
	this->noManette = noManette;
	this->lock.unlock();
}

int Touche::setNoTouche(int noTouche)
{
	int ret = 0;
	this->lock.lock();
	this->noTouche = noTouche;
	this->lock.unlock();
}

int Touche::setType(int typeTouche)
{
	int ret = 0;
	this->lock.lock();
	this->typeTouche = typeTouche;
	this->lock.unlock();
}

void Touche::activer()
{
	this->lock.lock();
	this->desactive = false;
	this->lock.unlock();
}

void Touche::desactiver()
{
	this->lock.lock();
	this->desactive = true;
	this->lock.unlock();
}

bool Touche::actif()
{
	bool ret;
	this->lock.lock();
	ret = !this->desactive;
	this->lock.unlock();
	return ret;
}

bool Touche::appeller(bool val)
{
	int ret;
	this->lock.lock();
	ret = this->appel;
	this->appel = val;
	this->lock.unlock();
	return ret;

}
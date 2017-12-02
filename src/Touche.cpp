#include <Touche.h>

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
 * @return valeur de l'axe
 */
int Touche::getValAxe()
{
	this->lock.lock();
	int ret = this->valeur;
	if(this->typeTouche == TYPE_TOUCHE_MOLETTE)
		this->valeur = 0;
	this->lock.unlock();
	return ret;
}

int Touche::getValBrutAxe()
{
	this->lock.lock();
	int ret = this->valeurBrut;
	this->lock.unlock();
	return ret;
}

/**
 * Indique si la touche est considérée comme pressée
 * Si la touche est une touche directionnelle, demande la direction
 * @return touche pressée
 */
bool Touche::isPressed(int direction)
{
	// Selon le sens
	this->lock.lock();
	bool ret = (!this->inverserClic && this->valeur >= this->valClic ||
				this->inverserClic && this->valeur <= this->valClic);
	this->lock.unlock();
	return ret;
}

/**
 * Prend la nouvelle valeur brut de l'axe, et la transforme en une valeur entre -100 et 100
 * Si le bouton est une molette, ajoute la valeur au lieu de l'écraser
 * @param valeurBrut Valeur brut de l'axe
 * @param ignoreInversion Ignore l'inversion à effectuer sur l'axe. Ne devrait pas être utilisé (à part par setPressed()) !!
 */
void Touche::setValAxe(int valeurBrut, bool ignoreInversion)
{
	this->lock.lock();
	this->valeurBrut = valeurBrut;
	// Si c'est en dehors des limites, on le met aux limites
	if(valeurBrut<this->min)
		valeurBrut = this->min;
	if(valeurBrut>this->max)
		valeurBrut = this->max;

	// On calcul la nouvelle valeur
	if(this->typeTouche != TYPE_TOUCHE_MOLETTE)
		this->valeur = (((valeurBrut-this->min)*PRECISION_VALEUR_AXE_TOUCHE)/(this->max-this->min))-(PRECISION_VALEUR_AXE_TOUCHE/2);
	else
	{
		this->valeur += (((valeurBrut-this->min)*PRECISION_VALEUR_AXE_TOUCHE)/(this->max-this->min))-(PRECISION_VALEUR_AXE_TOUCHE/2);
		if(this->valeur > PRECISION_VALEUR_AXE_TOUCHE/2)
			this->valeur = PRECISION_VALEUR_AXE_TOUCHE/2;
		if(this->valeur*-1 > PRECISION_VALEUR_AXE_TOUCHE/2)
			this->valeur = (PRECISION_VALEUR_AXE_TOUCHE/2)*-1;
	}

	// On inverse si besoin
	if(this->inverserAxe && !ignoreInversion)
		this->valeur *= -1;

	SDL_Log("val %s : %d\n", this->nom.c_str(), this->valeur);
	this->lock.unlock();
}


void Touche::setPressed(bool pressed)
{
	this->lock.lock();
	this->valeur = (pressed)?this->valClic:this->valUnclic;
	SDL_Log("val %s : %d\n", this->nom.c_str(), this->valeur);
	this->lock.unlock();
}

/**
 * Définit si le sens de l'axe ou l'etat du clic sont inversés
 * @param clic Si inversé, est appuyé lorsque le bouton est relâché
 * @param axe  Si inversé, bascule de -100 -> 100 à 100 -> -100
 */
void Touche::setInversion(bool clic, bool axe)
{
	this->lock.lock();
	this->inverserClic = clic;
	this->inverserAxe = axe;
	this->lock.unlock();
}

/**
 * Définit les limites haute et basse de l'axe, pour la valeur brut
 * Inverse l'axe si besoin
 * @param min Valeur minimum brut
 * @param max Valeur maximum brut
 */
void Touche::setValeurs(int min, int max)
{
	this->lock.lock();
	if(max > min)
	{
		this->min = min;
		this->max = max;
		this->inverserAxe = false;
	}
	else
	{
		this->min = max;
		this->max = min;
		this->inverserAxe = true;
	}
	this->lock.unlock();
}

/**
 * Définit les valeurs à prendre lors d'un appuie/relâchement booléen
 * Détermine en même temps la valeur de l'axe à laquelle on considère comme appuyé
 * Inverse automatiquement le clic si besoin
 * @param clic   Valeur lors d'un appuie / Valeur limite entre considération d'appuie/relâchement
 * @param unclic Valeur lors d'un relâchement
 */
void Touche::setValeursClic(int clic, int unclic)
{
	this->lock.lock();
	this->valClic = clic;
	this->valUnclic = unclic;
	this->inverserClic = (clic < unclic);
	this->lock.unlock();
}


bool Touche::getInversionClic()
{
	bool ret;
	this->lock.lock();
	ret = this->inverserClic;
	this->lock.unlock();
	return ret;
}


bool Touche::getInversionAxe()
{
	bool ret;
	this->lock.lock();
	ret = this->inverserAxe;
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


int Touche::getValeurClic()
{
	int ret;
	this->lock.lock();
	ret = this->valClic;
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
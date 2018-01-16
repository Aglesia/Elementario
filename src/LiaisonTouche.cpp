#include <LiaisonTouche.h>

LiaisonTouche::LiaisonTouche(ToucheJeu* tj, Touche* t1, Touche* t2)
{
	this->tj = tj;
	this->t1 = t1;
	this->t2 = t2;
	this->minTJ = tj->getValeurMin();
	this->maxTJ = tj->getValeurMax();
	this->minT = t1->getValeurMin();
	this->maxT = t1->getValeurMax();
	if(t2 != nullptr)
	{
		this->minT = t2->getValeurMin();
		this->maxT = t2->getValeurMax();
	}
}

LiaisonTouche::~LiaisonTouche(){}

void LiaisonTouche::nouvelEvenement(Touche* touche) // TODO : inhiber aussi T2
{
	// On regarde si ça nous concerne
	if(!this->touchePresente(touche))
		return;

	int valeur = touche->getValAxe(true); // Valeur de min à max
	valeur = (valeur - this->minT) * (this->maxTJ - this->minTJ) / (this->maxTJ - this->minT) + this->minTJ;

	// Si c'est t1 et que t2 est null, on le traite normalement selon le mode
	if(this->t2 == nullptr)
	{
		switch(this->mode)
		{
			case MODE_APPUIE_UNIQUE:
				if(this->inverserT1)
					valeur = (valeur == 0)?1:0;
				if(valeur>0)
				{
					this->tj->setVal(this->maxTJ);
					this->tj->setEvent();
				}
				break;

			case MODE_AXE_RELATIF:
				if(this->inverserT1)
					valeur = this->maxT - (valeur - this->minT);
				this->tj->setVal(tj->getVal()+valeur);
				SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "valeur LiaisonTouche : %d", valeur);
				this->tj->setEvent();
				break;

			case MODE_AXE_ABSOLUE:
				if(this->inverserT1)
					valeur = this->maxT - (valeur - this->minT);
				this->tj->setVal(valeur);
				this->tj->setEvent();
				break;

			default:
				if(this->inverserT1)
					valeur = this->maxT - (valeur - this->minT);
				this->tj->setVal((valeur)?this->maxTJ:this->minTJ);
				this->tj->setEvent();
		}
	}
	// Si c'est t1 et que t2 n'est pas null, on marque la valeur actuelle de t2
	// Si c'est t2, on regarde si t1 est enclanché (>0) et on désactive t1 et t2 si besoin
}

bool LiaisonTouche::touchePresente(Touche* touche)
{
	// On regarde si la touche vaut t1 ou t2
	return (touche == this->t1 || touche == this->t2);
}

bool LiaisonTouche::toucheJeuPresente(ToucheJeu* touche)
{
	// On regarde si la touche vaut t1 ou t2
	return (touche == this->tj);
}

/**
 * Change le mode de lisaison
 * @param mode Nouveau mode de liaison
 */
void LiaisonTouche::setMode(unsigned int mode)
{
	this->mode = mode;
	if(mode>3)
		this->mode = MODE_INCONNU;
}

bool LiaisonTouche::composeT(Touche* touche)
{
	return ((touche == this->t1 && this->t2 != nullptr) || touche == this->t2);
}

void LiaisonTouche::setMinMaxT(int min, int max)
{
	this->mutex.lock();
	this->minT = min;
	this->maxT = max;
	this->mutex.unlock();
}

void LiaisonTouche::setMinMaxTJ(int min, int max)
{
	this->mutex.lock();
	this->minTJ = min;
	this->maxTJ = max;
	this->mutex.unlock();
}

int LiaisonTouche::getMinT()
{
	int ret = 0;
	this->mutex.lock();
	ret = this->minT;
	this->mutex.unlock();
	return ret;
}

int LiaisonTouche::getMaxT()
{
	int ret = 0;
	this->mutex.lock();
	ret = this->maxT;
	this->mutex.unlock();
	return ret;
}

int LiaisonTouche::getMinTJ()
{
	int ret = 0;
	this->mutex.lock();
	ret = this->minTJ;
	this->mutex.unlock();
	return ret;
}

int LiaisonTouche::getMaxTJ()
{
	int ret = 0;
	this->mutex.lock();
	ret = this->maxTJ;
	this->mutex.unlock();
	return ret;
}
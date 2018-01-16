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

void LiaisonTouche::nouvelEvenement(Touche* touche)
{
	// On regarde si ça nous concerne
	if(!this->touchePresente(touche))
		return;

	if(!touche->actif())
		return;

	int valeur = (touche->getValAxe(true) - this->minT) * (this->maxTJ - this->minTJ) / (this->maxT - this->minT) + this->minTJ;

	// Si c'est t1 et que t2 est null, on le traite normalement selon le mode
	if(this->t2 == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Gestion de la touche %s -> %s, type1", touche->nom.c_str(), this->tj->getNom().c_str());
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
	else if(this->t1 == touche)
	{
		valeur = touche->getValAxe(true);
		if(this->inverserT1)
			valeur = this->maxT - (valeur - this->minT);

		if(valeur && this->etatEvent == 0)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Activation de la touche %s -> %s, type2", touche->nom.c_str(), this->tj->getNom().c_str());
			this->valeurAxeT2 = this->t2->getValAxe();
			this->valeurAxeTJ = this->tj->getVal();
			this->etatEvent = 1;
		}
		else if(!valeur)
		{
			if(etatEvent == 2)
				touche->desactiver();
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Désactivation de la touche %s -> %s, type2", touche->nom.c_str(), this->tj->getNom().c_str());
			this->etatEvent = 0;
		}
	}
	// Si c'est t2, on regarde si t1 est enclanché (>0) et on désactive t1 et t2 si besoin
	else
	{
		if(this->etatEvent > 0)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Gestion de la touche %s -> %s, type3", touche->nom.c_str(), this->tj->getNom().c_str());
			etatEvent = 2;
			// On indique de ne pas prendre en compte t1 ni t2 pour les autres
			touche->desactiver();
			this->t1->desactiver();

			switch(this->mode)
			{
				case MODE_APPUIE_UNIQUE:
					if(this->inverserT2)
						valeur = (valeur == 0)?1:0;
					if(valeur>0)
					{
						this->tj->setVal(this->maxTJ);
						this->tj->setEvent();
					}
					break;

				case MODE_AXE_RELATIF:
					if(this->inverserT2)
						valeur = this->maxT - (valeur - this->minT);
					this->tj->setVal(tj->getVal()+valeur);
					this->tj->setEvent();
					// TODO : Ajouter un rappel dans le controller pour être rappelé en boucle même si la valeur ne change pas
					break;

				case MODE_AXE_ABSOLUE:
					if(this->inverserT2)
						valeur = this->maxT - (valeur - this->minT);
					this->tj->setVal(valeur);
					this->tj->setEvent();
					break;

				case MODE_AXE_RELATIF_CLIC:
					if(this->inverserT2)
						valeur = this->maxT - (valeur - this->minT);
					this->tj->setVal(this->tj->getVal()-this->valeurAxeT2+valeur);
					this->tj->setEvent();
					// TODO : Ajouter un rappel dans le controller pour être rappelé en boucle même si la valeur ne change pas
					break;

				case MODE_AXE_ABSOLUE_CLIC:
					if(this->inverserT2)
						valeur = this->maxT - (valeur - this->minT);
					this->tj->setVal(this->valeurAxeTJ-this->valeurAxeT2+valeur);
					this->tj->setEvent();
					break;

				default:
					if(this->inverserT2)
						valeur = this->maxT - (valeur - this->minT);
					this->tj->setVal((valeur)?this->maxTJ:this->minTJ);
					this->tj->setEvent();
			}
		}
	}
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
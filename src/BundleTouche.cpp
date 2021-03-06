#include <BundleTouche.h>

BundleTouche::BundleTouche()
{
	this->liaisons.clear();
}

BundleTouche::~BundleTouche()
{
	for(int i=0; i<this->liaisons.size(); i++)
		delete this->liaisons[i];
	this->liaisons.clear();
}

/**
 * Gère l'évènement si une touche virtuelle doit la traiter
 * @param  touche touche réelle qui a changé
 * @return        true si la touche est interceptée, false sinon
 */
bool BundleTouche::nouvelEvenement(Touche* touche)
{
	touche->activer();
	// Pour chaque touche, on liste les ToucheJeu qui en ont besoin
	std::vector<LiaisonTouche*> tempL;
	for(int i=0; i<this->liaisons.size(); i++)
		if(this->liaisons[i]->touchePresente(touche))
			tempL.push_back(this->liaisons[i]);
	// S'il n'y en a pas, on retourne false
	if(tempL.size() == 0)
		return false;
	touche->activer();

	int ret = 0;

	// On fait passer la touche par toutes les touches composées
	for(int i=0; i<tempL.size(); i++)
		if(tempL[i]->composeT(touche))
			ret += tempL[i]->nouvelEvenement(touche);

	// Si la touche n'est pas inhibée
	if(touche->actif())
	{
		// On filtre les appuies
		if(touche->getType() == TYPE_TOUCHE_BOUTON && touche->getValAxe(true) > 0)
			return true;
		// Si c'est un relâchement non inhibé, on envoie d'abord un appuie
		if(touche->getType() == TYPE_TOUCHE_BOUTON && touche->getValAxe(true) == 0 && touche->actif())
		{
			touche->setValAxe(1);
			for(int i=0; i<tempL.size(); i++)
				if(!tempL[i]->composeT(touche))
					ret += tempL[i]->nouvelEvenement(touche);
			touche->setValAxe(0);
		}
		// Si la touche n'est pas inhibée, on la fait passer dans les autres touches
		if(touche->actif())
			for(int i=0; i<tempL.size(); i++)
				if(!tempL[i]->composeT(touche))
					ret += tempL[i]->nouvelEvenement(touche);
	}

	// On remet la molette à 0 si besoin
	touche->getValAxe(false);
	// On retourne true
	return (ret>0);
}

/**
 * Ajoute une liaison au bundle
 * @param liaison Liaison à ajouter
 */
void BundleTouche::ajouterLiaison(LiaisonTouche* liaison)
{
	this->liaisons.push_back(liaison);
}

/**
 * Donne la liste de toutes les liaisons créées pour une toucheJeu donnée
 */
std::vector<LiaisonTouche*> BundleTouche::getLiaisons(ToucheJeu* touche){
	std::vector<LiaisonTouche*> ret;
	ret.clear();

	// Pour chaque liaison qu'on a, on l'ajoute à la liste si la touche est contenue dedans
	for(int i=0; i<this->liaisons.size(); i++)
		if(this->liaisons[i]->toucheJeuPresente(touche))
			ret.push_back(this->liaisons[i]);

	return ret;
}
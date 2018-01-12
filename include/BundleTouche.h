#ifndef BUNDLE_TOUCHE_H
#define BUNDLE_TOUCHE_H

#include <LiaisonTouche.h>
#include <ToucheJeu.h>
#include <Touche.h>
#include <defs.h>

class BundleTouche
{
	public:
		BundleTouche();
		virtual ~BundleTouche();
		/**
		 * Gère l'évènement si une touche virtuelle doit la traiter
		 * @param  touche touche réelle qui a changé
		 * @return        true si la touche est interceptée, false sinon
		 */
		bool nouvelEvenement(Touche* touche);

		/**
		 * Ajoute une liaison au bundle
		 * @param liaison Liaison à ajouter
		 */
		void ajouterLiaison(LiaisonTouche* liaison);

		/**
		 * Indique au bundle de remetre toutes les touches à 0
		 */
		void reset();

		/**
		 * Donne la liste de toutes les liaisons créées pour une toucheJeu donnée
		 */
		std::vector<LiaisonTouche*> getLiaisons(ToucheJeu* touche);

	protected:

	private:
		std::mutex mutex; // Accès partagée
		std::vector<LiaisonTouche*> liaisons; // Ensemble des liaisons du bundle
};

#endif
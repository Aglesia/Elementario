#ifndef LIAISON_TOUCHE_H
#define LIAISON_TOUCHE_H

#include <Touche.h>
#include <ToucheJeu.h>
#include <defs.h>

class LiaisonTouche
{
	public:
		LiaisonTouche(ToucheJeu* tj, Touche* t1, Touche* t2 = nullptr);
		virtual ~LiaisonTouche();
		// Retourne 0 si la touche n'a pas été traitée par la liaison touche
		int nouvelEvenement(Touche* touche);
		bool touchePresente(Touche* touche);
		bool toucheJeuPresente(ToucheJeu* touche);
		/**
		 * Change le mode de lisaison
		 * @param mode Nouveau mode de liaison
		 */
		void setMode(unsigned int mode);
		/**
		 * Indique si la touche est égale à t1, et que c'est une liaison à touche composée
		 * @param  touche Touche à tester
		 * @return        true si c'est une liaison composée, et que touche vaut t1
		 */
		bool composeT(Touche* touche);

		void setMinMaxT(int min, int max);
		int getMinT();
		int getMaxT();
		void setMinMaxTJ(int min, int max);
		int getMinTJ();
		int getMaxTJ();

	private:
		std::mutex mutex; // Accès partagée
		ToucheJeu* tj; // ToucheJeu à modifier
		Touche* t1; // Touche à prendre en compte
		Touche* t2; // Seconde touche à prendre en compte, pour les touches composées (un axe, normalement)
		int etatEvent = 0; // Etat actuel de l'enchainement d'évènements (0=idle, 1=t1 actif, 2=t2 a bougé)
		int valeurAxeT2 = 0; // Valeur de t2 lors de l'appuie sur t1
		int valeurAxeTJ = 0; // Valeur de tJ lors de l'appuie sur t1
		bool actif = false; // Indique si l'action de t1 a été pris en compte, et qu'il est maintenant inhibé. Si oui, les mouvements de t2 sont pris en compte
		int mode = MODE_INCONNU; // Mode de liaison avec la touche virtuelle
		bool inverserT1 = false; // Inverser le sens de T1
		bool inverserT2 = false; // Inverser le sens de T2
		int minT = 0; // Valeur minimal de t1 ou t2 (si composée)
		int maxT = 0; // Valeur maximale de t1 ou t2 (si composée)
		int minTJ = 0; // Valeur minimal de tj
		int maxTJ = 0; // Valeur maximale de tj
};

#endif
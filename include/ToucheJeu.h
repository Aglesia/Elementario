#ifndef TOUCHE_JEU_H
#define TOUCHE_JEU_H

#include <Touche.h>
#include <defs.h>

class ToucheJeu
{
	public:
		ToucheJeu(std::string nom);
		virtual ~ToucheJeu();
		void nouvelEvenement(Touche* touche);
		void activer(bool actif = true);
		int getVal(bool boolean = false);
		void addTouche(Touche* touche);
		void removeTouche(Touche* touche);
		bool touchePresente(Touche* touche);
		void setVal(int axe, bool boolean = false);

	protected:

	private:
		std::mutex mutex; // Accès partégée
		std::string nom; // Nom de la touche en jeu
		bool actif = false; // Indique si cette touche-jeu est active
		int valeurAxe = 0;
		bool valeurBool = 0;
		std::vector<Touche*> touches; // Ensemble des touches modifiant les propriétés de cette toucheJeu
};

#endif
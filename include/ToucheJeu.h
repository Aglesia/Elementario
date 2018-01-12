#ifndef TOUCHE_JEU_H
#define TOUCHE_JEU_H

#include <Touche.h>
#include <defs.h>

class ToucheJeu
{
	public:
		ToucheJeu(std::string nom, std::string description, int valeurDefaut=0, int min=-PRECISION_VALEUR_AXE_TOUCHE, int max=PRECISION_VALEUR_AXE_TOUCHE);
		virtual ~ToucheJeu();
		int getVal();
		void setVal(int axe);
		void setEvent();
		bool isEvent();
		void setDefaut();
		int getValeurMin();
		int getValeurMax();
		std::string getNom();
		std::string getDescription();

	private:
		std::mutex mutex; // Accès partagée
		std::string nom; // Nom de la touche en jeu
		std::string description; // Nom de la touche en jeu
		int valeurAxe; // Valeur actuelle de l'axe
		int def; // Valeur par défaut de la touche
		int min; // Valeur minimum de la touche
		int max; // Valeur maximum de la touche
		bool event = false; // Indique si un évènement est apparu depuis la dernière fois
};

#endif
#ifndef TOUCHE_H
#define TOUCHE_H

#include <defs.h>

class Touche
{
	public:
		Touche(int noManette, int noTouche, int typeTouche);
		virtual ~Touche();

		// access
		bool isTouche(int noManette, int noTouche, int typeTouche);
		int getValAxe(bool sys = false);
		void setValAxe(int valeurBrut, bool ignoreInversion = false);

		// Config
		void setInversion(bool inversion);
		void setValeurs(int min, int max);
		void setValeursBrut(int min, int max);
		bool getInversion();
		int getValeurMin();
		int getValeurMax();
		int getNoManette();
		int getNoTouche();
		int getType();
		int setNoManette(int noManette);
		int setNoTouche(int noTouche);
		int setType(int typeTouche);
		void activer();
		void desactiver();
		bool actif();
		bool appeller(bool val=false);

		bool configure = false; // Indique si le bouton est configuré (calibré, etc...)
		std::string nom; // Nom de la touche d'après SDL

	private:
		std::mutex lock; // Mutex de gestion d'accès à l'objet
		int valeur = 0; // Valeur de min à max
		int minBrut = -32768; // valeur min rélle
		int maxBrut = 32768; // valeur max rélle
		int min = -PRECISION_VALEUR_AXE_TOUCHE; // Valeur min virtuelle
		int max = PRECISION_VALEUR_AXE_TOUCHE; // Valeur max virtuelle
		bool inversion = false; // Inverser le sens de l'axe (de max à min)
		int noManette = 0; // Numéro de la manette à laquelle appartient la touche
		int noTouche = 0; // Numéro de la touche sur la manette
		int typeTouche = 0; // Type d'input (bouton/axe/molette, croix directionnelle)
		bool desactive = false; // Indique si l'action a été interceptée et est inhibée
		bool appel = false; // Indique au contrôleur s'il doit emmettre un évènement sur la touche, même s'il n'y en a pas
};

#endif
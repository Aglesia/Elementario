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
		int getValAxe();
		int getValBrutAxe();
		bool isPressed(int direction = 0);
		void setValAxe(int valeurBrut, bool ignoreInversion = false);
		void setPressed(bool pressed);

		// Config
		void setInversion(bool clic, bool axe);
		void setValeurs(int min, int max);
		void setValeursClic(int clic, int unclic);
		bool getInversionClic();
		bool getInversionAxe();
		int getValeurMin();
		int getValeurMax();
		int getValeurClic();
		int getNoManette();
		int getNoTouche();
		int getType();
		int setNoManette(int noManette);
		int setNoTouche(int noTouche);
		int setType(int typeTouche);

		bool configure = false; // Indique si le bouton est configuré (calibré, etc...)
		std::string nom; // Nom de la touche d'après SDL

	protected:

	private:
		std::mutex lock; // Mutex de gestion d'accès à l'objet
		int valeur = 0; // Valeur de -PRECISION_VALEUR_AXE_TOUCHE à +PRECISION_VALEUR_AXE_TOUCHE
		int valeurBrut = 0; // Valeur de min à max
		int min = -32768; // Valeur min du capteur
		int max = 32767; // Valeur max du capteur
		bool inverserClic = false; // Inverser le sens de détection du clic
		bool inverserAxe = false; // Inverser le sens de l'axe (de max à min)
		int noManette = 0; // Numéro de la manette à laquelle appartient la touche
		int noTouche = 0; // Numéro de la touche sur la manette
		int typeTouche = 0; // Type d'input (bouton/axe/molette/directionnelle)
		int valClic = 255; // Valeur lorsque cliqué (et valeur au dessus/dessous de laquelle c'est considéré comme appuyé)
		int valUnclic = 0; // Valeur lorsque relâché
};

#endif
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Manette.h>
#include <ToucheJeu.h>
#include <defs.h>

class Controller // TODO : Surcharger le copy-constructeur
{
	public:
		// Crée un objet de gestion des évènements (clavier/souris/manette)
		Controller(ToucheJeu** touchesJeu);
		// Supprime l'objet de gestion des évènements
		virtual ~Controller();
		// Met à jour les actions effectuées
		void update();
		// Indique le nombre de controller détectés
		int nbController();
		Manette* getController(unsigned int noC);
		// Indique si l'utilisateur a demandé à quitter
		bool quitter();
		Touche* getTouche(int noJoystic, int noBouton, int typeBouton);
		std::vector<Touche*> getTouches();
		void mapManette(Manette* m);
		void tailleFenetre(int x, int y);
		/**
		 * Lance l'écoute sur un contrôlleur particulier :
		 * @param t Touche à remplir, comporte déjà l'ID de controlleur
		 * Note : L'écoute est arrêtée dès le premier évènement qui match
		 */
		void listen(Touche* t);
		static std::string intToString(int i);

	protected:

	private:
		/**
		 * Envoie un évènement à toutes les ToucheJeu pour dire qu'une touche a changé d'état
		 */
		void event(Touche* t);

		Manette* manetteSouris; // Souris
		Manette* manetteClavier; // Clavier
		std::vector<Manette*> manettes; // Ensemble des manettes
		ToucheJeu** touchesJeu = nullptr; // Ensemble des touches du jeu
		std::vector<Touche*> touchesInput; // Ensemble des touches des input
		int sourisX = 0; // Position de la souris, en X (0 = à gauche)
		int sourisY = 0; // position de la souris, en Y (0 = en haut)
		bool doitQuitter = false; // Indique si l'utilisateur a demandé à quitter
		std::mutex lock; // Mutex de gestion d'accès à l'objet
		Touche* toucheEcoutee = nullptr; // Touche à l'écoute : est remplie dès qu'il y'a un évènement sur le bon controller
};

#endif // CONTROLLER_H

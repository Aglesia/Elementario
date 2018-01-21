#ifndef CONFIG_H
#define CONFIG_H

#include <defs.h>

class Config
{
public:
	Config();
	~Config();

	bool getAfficherPointeur();
	void setAfficherPointeur(bool param);

private:
	std::mutex lock; // Mutex d'accès aux objets
	bool afficherPointeur = false; // Affiche un pointeur à l'emplacement du pointeur/joystic
	
};

#endif



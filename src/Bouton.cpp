#include <Bouton.h>

Bouton::Bouton(SDL_Surface* boutonSurface, SDL_Renderer* renderer, int taille)
{
	this->boutonSurface = boutonSurface;
	this->renderer = renderer;
	this->taille = taille;
}

Bouton::~Bouton()
{
	// On vire le texte
	if(this->texteTexture)
		SDL_DestroyTexture(this->texteTexture);
	if(this->boutonTexture)
		SDL_DestroyTexture(this->boutonTexture);
	if(this->boutonSurface)
		SDL_FreeSurface(this->boutonSurface);
}

/**
 * Initialise la texture via la surface donnée
 * Doit être lancé dans le thread principale
 */
void Bouton::init()
{
	if(this->boutonSurface)
		this->boutonTexture = SDL_CreateTextureFromSurface(this->renderer, this->boutonSurface);
}

/**
 * Affiche le bouton sur la texture, en plaçant le centre à l'emplacement indiqué
 * @param pos           Position du centre du bouton
 * @param nbTicks       Nombre de ticks actuel
 * @param etatSelection Etat actuel du bouton (sélectionné ou non)
 */
void Bouton::afficher(SDL_Rect* p, unsigned int nbTicks, bool etatSelection, int opacite)
{
	// On vérifie que la texture est initialisée
	if(this->boutonTexture == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible d'afficher le bouton : il n'est pas initialisé\n");
		return;
	}

	SDL_Rect pos;
	// On calcul la taille
	switch(this->etatAnimation)
	{
		case 0: // non sélectionné
			pos.h = taille;
			pos.w = taille;
			if(etatSelection)
			{
				this->etatAnimation = 1;
				this->nbTicksAnimation = nbTicks;
			}
			break;

		case 1: // En cours de sélection
			pos.h = taille + ((taille*NIVEAU_ZOOM_BOUTON_ACTIF*(nbTicks - this->nbTicksAnimation))/NB_TICKS_ANIMATION_BOUTON);
			pos.w = pos.h;
			if((nbTicks - this->nbTicksAnimation) >= NB_TICKS_ANIMATION_BOUTON)
				this->etatAnimation = 2;
			if(!etatSelection)
			{
				this->etatAnimation = 3;
				// On inverse le temps
				this->nbTicksAnimation = nbTicks*2 - NB_TICKS_ANIMATION_BOUTON - this->nbTicksAnimation;
			}
			break;

		case 2: // Sélectionné
			pos.h = taille*(NIVEAU_ZOOM_BOUTON_ACTIF+1);
			pos.w = pos.h;
			if(!etatSelection)
			{
				this->etatAnimation = 3;
				this->nbTicksAnimation = nbTicks;
			}
			break;

		default: // En cours de déselection
			pos.h = taille + (taille*NIVEAU_ZOOM_BOUTON_ACTIF) - ((taille*NIVEAU_ZOOM_BOUTON_ACTIF*(nbTicks - this->nbTicksAnimation))/NB_TICKS_ANIMATION_BOUTON);
			pos.w = pos.h;
			if((nbTicks - this->nbTicksAnimation) >= NB_TICKS_ANIMATION_BOUTON)
				this->etatAnimation = 0;
			if(etatSelection)
			{
				this->etatAnimation = 1;
				// On inverse le temps
				this->nbTicksAnimation = nbTicks*2 - NB_TICKS_ANIMATION_BOUTON - this->nbTicksAnimation;
			}
			break;
	}

	// On calcul la position
	pos.x = p->x - (pos.w/2);
	pos.y = p->y - (pos.h/2);
	SDL_SetTextureAlphaMod(this->boutonTexture, opacite);
	SDL_RenderCopy(this->renderer, this->boutonTexture, nullptr, &pos);
}

/**
 * Définit le texte à afficher sous le bouton
 * @param texte texte
 */
void Bouton::setTitre(std::string texte) // TODO
{
	// On vide la texture
	if(this->texteTexture)
		SDL_DestroyTexture(this->texteTexture);

	// On crée le nouveau texte
}

/**
 * Définit le texte à afficher sous le bouton
 * @param texte texte
 */
void Bouton::setTaille(int taille)
{
	this->taille = taille;
}

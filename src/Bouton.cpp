#include <Bouton.h>

Bouton::Bouton(SDL_Surface* boutonSurface, SDL_Renderer* renderer, int taille, std::string nom, std::string description)
{
	this->boutonSurface = boutonSurface;
	this->renderer = renderer;
	this->taille = taille;
	this->nom = nom;
	this->description = description;
}

Bouton::Bouton(SDL_Texture* boutonTexture, SDL_Renderer* renderer, int taille, std::string nom, std::string description)
{
	this->boutonTexture = boutonTexture;
	this->renderer = renderer;
	this->taille = taille;
	this->nom = nom;
	this->description = description;
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
	// TODO : On crée le texte selon le nom du bouton
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
				this->etatAnimation = (nbTicks>0)?1:2;
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
				this->etatAnimation = (nbTicks>0)?3:0;
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
 * Affiche le bouton sur la texture, en plaçant le centre à l'emplacement indiqué
 * @param pos           Position du centre du bouton
 * @param etatSelection Etat actuel du bouton (sélectionné ou non)
 */
void Bouton::afficher(SDL_Rect* p, bool etatSelection, int opacite, SDL_Rect* p2)
{
	// On vérifie que la texture est initialisée
	if(this->boutonTexture == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,  "Impossible d'afficher le bouton : il n'est pas initialisé");
		return;
	}

	if(p->h == 0 || p->w == 0)
		return;

	// On calcule la position
	SDL_Rect pos;
	SDL_Rect pos2;
	pos.w = p2->w;
	pos.h = p2->h;
	pos.x = p->x - (p->w/2) + p2->x;
	pos.y = p->y - (p->h/2) + p2->y;
	if(etatSelection)
	{
		pos.x -= (p->w * NIVEAU_ZOOM_BOUTON_ACTIF)/2;
		pos.y -= (p->h * NIVEAU_ZOOM_BOUTON_ACTIF)/2;
		pos.w *= 1+NIVEAU_ZOOM_BOUTON_ACTIF;
		pos.h *= 1+NIVEAU_ZOOM_BOUTON_ACTIF;
	}
	SDL_QueryTexture(this->boutonTexture, NULL, NULL, &pos2.w, &pos2.h);
	pos2.x=(pos2.w*p2->x)/p->w;
	pos2.y=(pos2.h*p2->y)/p->h;
	pos2.w=(pos2.w*p2->w)/p->w;
	pos2.h=(pos2.h*p2->h)/p->h;
	SDL_SetTextureAlphaMod(this->boutonTexture, opacite);
	SDL_RenderCopy(this->renderer, this->boutonTexture, &pos2, &pos);
}

/**
 * Indique si le pointeur touche le bouton, en tenant compte de la transparence
 * @param  tailleX TailleX du bouton à l'écran
 * @param  tailleY TailleY du bouton à l'écran
 * @param  posX    Position relative du pointeur par rapport au centre du bouton
 * @param  posY    Position relative du pointeur par rapport au centre du bouton
 * @return         true si le pointeur touche le bouton
 */
bool Bouton::estPointe(int tailleX, int tailleY, int posX, int posY)
{
	bool ret = false;
	posX+=(tailleX/2);
	posY+=(tailleY/2);

	// Si la position est négative ou sort de la taille du bouton, on ignore
	if(posX>=0 && posX<tailleX && posY>=0 && posY<tailleY)
	{
		// On calcul la position du pixel réel
		int pX = posX*this->boutonSurface->w/tailleX;
		int pY = posY*this->boutonSurface->h/tailleY;

		// On regarde si le pixel est au moins opaque à 50
		Uint8 r, g, b, a;
		SDL_GetRGBA(this->getpixel(this->boutonSurface, pX, pY), this->boutonSurface->format, &r, &g, &b, &a);
		ret = (a>=50);
	}

	return ret;
}

Uint32 Bouton::getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

std::string Bouton::getNom()
{
	return this->nom;
}

std::string Bouton::getDescription()
{
	return this->description;
}

void Bouton::setNom(std::string nom)
{
	this->nom = nom;
}

void Bouton::setDescription(std::string description)
{
	this->description = description;
}

/**
 * Modifie la taille de référence du bouton (zoom)
 * @param taille Taille
 */
void Bouton::setTaille(int taille)
{
	this->taille = taille;
}

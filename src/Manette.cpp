#include "Manette.h"

Manette::Manette(){
	this->touches.clear();
}

Manette::~Manette()
{
	SDL_JoystickClose(this->joy);
	for(int i=0; i<this->touches.size(); i++)
		delete this->touches[i];
}

void Manette::ajoutTouche(Touche* touche)
{
	this->touches.push_back(touche);
}

Touche* Manette::getTouche(int noBouton, int typeBouton)
{
	Touche* ret = nullptr;
	for(int i=0; i<this->touches.size(); i++)
		if(this->touches[i]->getNoTouche() == noBouton && this->touches[i]->getType() == typeBouton)
		{
			ret = this->touches[i];
			break;
		}
	return ret;
}

std::vector<Touche*> Manette::getTouches()
{
	return this->touches;
}
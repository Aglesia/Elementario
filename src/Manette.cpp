#include "Manette.h"

Manette::Manette(){}

Manette::~Manette()
{
	SDL_JoystickClose(this->joy);
}
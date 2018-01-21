#include <Config.h>

Config::Config()
{

}


Config::~Config()
{

}

bool Config::getAfficherPointeur()
{
	bool ret;
	this->lock.lock();
	ret = this->afficherPointeur;
	this->lock.unlock();
	return ret;
}

void Config::setAfficherPointeur(bool param)
{
	this->lock.lock();
	this->afficherPointeur = param;
	this->lock.unlock();
}
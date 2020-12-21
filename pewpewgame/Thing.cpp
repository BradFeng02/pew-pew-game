#include "Thing.h"
#include "Solid.h"
#include "Player.h"
#include <iostream>

void Thing::act()
{
}

void Thing::collideWith(Solid* t)
{
	std::cout << "base" << std::endl;
	return;
}

void Thing::collideWith(Player* t)
{
	std::cout << "base" << std::endl;
	return;
}
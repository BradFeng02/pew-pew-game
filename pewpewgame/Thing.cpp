#include "Thing.h"
#include "Solid.h"
#include "Player.h"
#include <iostream>

void Thing::step()
{
}

void Thing::collideWith(Solid* t)
{
	std::cout << "thing C/W solid" << std::endl;
	return;
}

void Thing::collideWith(Player* t)
{
	std::cout << "thing C/W player" << std::endl;
	return;
}
#pragma once
#include <Box2D\Dynamics\b2Body.h>
#include "Thing.h"
class Solid :public Thing
{
protected:
	b2Body* body;
public:
	Solid(float height, float width, float x0, float y0,float angle);
	virtual void render();

	virtual void collideWith(Solid* t);
	virtual void collideWith(Player* t);
};


#pragma once
#include <Box2D/Box2D.h>
#include "Thing.h"
//#include "Thing.h"
class Player :public Thing
{
protected:
	float hgt, wid, boxhalfhgt;// , x, y;
	b2Body* body;
public:
	Player(float height,float width,float x0,float y0);
	virtual void render();
	b2Body* getBody();

	virtual void collideWith(Solid* t);
	virtual void collideWith(Player* t);

};


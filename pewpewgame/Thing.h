#pragma once

class Solid;
class Player;

enum thingtype {
	SOLI,
	PLAY,
	PROJ
};
struct ThingRef
{
	thingtype id;
	void* t;
};

class Thing
{
protected:
	ThingRef ref;
public:
	virtual void act();

	virtual void collideWith(Solid* t);
	virtual void collideWith(Player* t);
private:
};

#pragma once
#include "Player.h"
class ControllablePlayer :public Player
{
protected:
	float maxspeed;
public:
	ControllablePlayer(float height, float width, float x0, float y0);
	void render();
	void act();

	void right_pressed();
	void left_pressed();
	void jump_pressed();
	void right_released();
	void left_released();
	void jump_released();

	virtual void collideWith(Solid* t);
private:
	void stopmoving();
	bool movingleft;
	bool movingright;
	int tright;
	int tleft;

	//temp
	b2Vec2 ray1;
	b2Vec2 ray2;

};
#pragma once
#include "Player.h"
class ControllablePlayer :public Player
{
protected:
	float maxspeed;
public:
	ControllablePlayer(float height, float width, float x0, float y0);
	void render();
	void drawRays();
	void step();

	void right_pressed();
	void left_pressed();
	void jump_pressed();
	void right_released();
	void left_released();
	void jump_released();

	virtual void collideWith(Solid* t);
private:
	void stopmoving();
	bool movingleft, movingright;
	int tright;
	int tleft;

	int raycnt = 2; // number of rays to check terrain angle per side (ex: 2 for 5 total  //|\\ )
	float raylen = 0.1f;
	b2Vec2* rays;

	float dirx, diry;

};
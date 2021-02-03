#pragma once
#include "Player.h"
#include "controllableplayerraycallback.h"

class ControllablePlayer :public Player
{
protected:
	float maxspeed = 5.0f;
	float maxaccel=70.0f;
	float turnrate = 10.0f;
	int startrate = 7;
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

	bool isGrounded();

	virtual void collideWith(Solid* t);
private:
	void stopmoving();
	bool movingleft, movingright, jumping, grounded;
	int tright, tleft, tjump;

	int raycnt = 2; // number of rays to check terrain angle per side (ex: 2 for 5 total  //|\\ )
	float raylen = 0.1f;
	b2Vec2* rays;
	
	float dirx, diry;
	void getDirLeft(raycallback* cb);
	void getDirRight(raycallback* cb);
	void checkGrounded(raycallback* cb);
};
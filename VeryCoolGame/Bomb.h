#pragma once
#include <tuple>
#include <list>
class Bomb
{
public:
	Bomb(float initialX, float initialY, float Vx_or_Angle,float Vy_or_Vi, float BULLET_GRAVITY, float initialLife,float power,bool usingAngle);
	~Bomb();
	void act(std::pair<char, int>* world, int worldh, int worldw);
	void recycle(float initialX, float initialY, float Vx_or_Angle, float Vy_or_Vi, float BULLET_GRAVITY, float initialLife, float power, bool usingAngle);
	float x;
	float y;
	float vx;
	float vy;
	float g;
	float p;
	float life;
	bool primed;
	std::list<int> explodeX;
	std::list<int> explodeY;
private:
	bool collide(std::pair<char, int>* world, int worldh, int worldw, int curx, int cury);
};


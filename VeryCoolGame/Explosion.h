#pragma once
#include <vector>
class Explosion
{
public:
	//int r;
	//int f;
	//int s;
	//bool decay;
	//double scale;
	//int mx;
	//int my;
	//Explosion(int x, int y, int radius, int speed);
	Explosion(float x, float y, float radius, std::pair<char, int>* world, int worldh, int worldw);
	~Explosion();
	static unsigned long fastrand(void);
private:
	std::vector<float> leftbound;
	std::vector<float> rightbound;
	std::vector<float> upbound;
	std::vector<float> downbound;
	//void act(std::pair<char, int>* world, int worldh, int worldw);
};


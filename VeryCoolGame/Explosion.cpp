#include "Explosion.h"
#include <iostream>
#include <cmath>
#include <vector>
#include "SimplexNoise.h"
#include <algorithm>

static unsigned long x = 123456789, y = 362436069, z = 521288629;
unsigned long Explosion::fastrand(void) {          //period 2^96-1
	unsigned long t;
	x ^= x << 16;
	x ^= x >> 5;
	x ^= x << 1;

	t = x;
	x = y;
	y = z;
	z = t ^ x ^ y;

	return z;
}

//Explosion::Explosion(float mx, float my, float radius, std::pair<char, int>* world, int worldh, int worldw) {
//	int lseed = fastrand() % 100000;
//	int rseed = fastrand() % 100000;
//	int useed = fastrand() % 100000;
//	int dseed = fastrand() % 100000;
//	for (int i = 1; i <= radius * 2+1; ++i) {
//		double hchord = radius * sin(acos(abs(radius + 1 - i) / double(radius)));
//		leftbound.emplace_back(std::min(-hchord + SimplexNoise::noise(lseed+i/.354) * (radius / 20.0), -1.0));
//		rightbound.emplace_back(std::max(hchord - SimplexNoise::noise(rseed+i/.354) * (radius / 20.0), 1.0));
//		upbound.emplace_back(std::min(-hchord + SimplexNoise::noise(useed+i/.354) * (radius / 20.0), -1.0));
//		downbound.emplace_back(std::max(hchord - SimplexNoise::noise(dseed+i/.354) * (radius / 20.0), 1.0));
//	}
 //
//	for (int y = -radius - 1; y <= radius; ++y) {
//		int hchord = radius * sin(acos(abs(y) / double(radius)));
//		for (int x = -hchord - 1; x <= hchord; ++x) {
//			int rx = mx + x;
//			int ry = my + y;
//			if (rx >= 0 && rx < worldw && ry >= 0 && ry < worldh) {
//				if ((x >= leftbound[y + radius] && x <= rightbound[y + radius]) && (y >= upbound[x + radius] && y <= downbound[x + radius])) {
//					int life = 20 + fastrand() % 50 + std::pow((fastrand() % 300 / 100.0), 3.7);
//					if (life > 120)life += Explosion::fastrand() % 70;
//					world[rx + ry * worldw].first = 'f';
//					world[rx + ry * worldw].second = life;
//					//world[rx + ry * worldw] = std::pair<char, int>('f', life);
//				}
//			}
//		}
//	}
//}

Explosion::Explosion(float mx, float my, float radius, std::pair<char, int>* world, int worldh, int worldw) {
	int lseed = fastrand() % 100000;
	int rseed = fastrand() % 100000;
	int useed = fastrand() % 100000;
	int dseed = fastrand() % 100000;
	for (int i = 1; i <= radius * 2 + 1; ++i) {
		double hchord = radius * sin(acos(abs(radius + 1 - i) / double(radius)));
		leftbound.emplace_back(std::min(-hchord + SimplexNoise::noise(lseed + i / .354) * (radius / 20.0), -1.0));
		rightbound.emplace_back(std::max(hchord - SimplexNoise::noise(rseed + i / .354) * (radius / 20.0), 1.0));
		upbound.emplace_back(std::min(-hchord + SimplexNoise::noise(useed + i / .354) * (radius / 20.0), -1.0));
		downbound.emplace_back(std::max(hchord - SimplexNoise::noise(dseed + i / .354) * (radius / 20.0), 1.0));
	}

	for (int y = -radius - 1; y <= radius; ++y) {
		int hchord = radius * sin(acos(abs(y) / double(radius)));
		for (int x = -hchord - 1; x <= hchord; ++x) {
			int rx = mx + x;
			int ry = my + y;
			if (rx >= 0 && rx < worldw && ry >= 0 && ry < worldh) {
				if ((x >= leftbound[y + radius] && x <= rightbound[y + radius]) && (y >= upbound[x + radius] && y <= downbound[x + radius])) {
					int life = 20 + Explosion::fastrand() % 50 + std::pow((Explosion::fastrand() % 300 / 100.0), 3.7);
					if (life > 120)life += Explosion::fastrand() % 70;
					world[rx + ry * worldw].first = 'f';
					world[rx + ry * worldw].second = life;
				}
			}
		}
	}
}

Explosion::~Explosion()
{
	//std::cout << "*BOOM*" << std::endl;
}



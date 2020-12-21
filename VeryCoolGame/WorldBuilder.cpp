#include "WorldBuilder.h"
#include <cmath>
#include <iostream>
#include "SimplexNoise.h"
#include <time.h>

const int dirtLife = 10;
const int stoneLife = 50;
const int obsidianLife = 100;

void WorldBuilder::fill_terrain(int seed,int worldw,int worldh, std::pair<char, int>* world) {
	std::cout << "building terrain... (seed = " << seed<<")"<<std::endl;
	//std::srand(seed);
	for (int x = 0; x < worldw; ++x) {
		float rand = SimplexNoise::noise((x + seed) / 500.0) * 50.0;
		float bigrand = SimplexNoise::noise((x - seed) / 8000.0) * 600.0;
		float smallrand = SimplexNoise::noise((x - seed) / 300.0) * 3.0;
		float sharprand = std::pow(SimplexNoise::noise((x - seed) / 100.0) * 2, 3);
		float h = worldh / 2 + rand + smallrand + sharprand + bigrand;
		//float h = worldh / 2 + bigrand;
		for (int y = 0; y < worldh; ++y) {
			if (y < h) world[x + y * worldw] = std::pair<char,int>('a',0);
			else {
				float stonenoise = (SimplexNoise::noise((x - seed) / 222.0, (y + seed) / 111.0)+1)* 100000000 + SimplexNoise::noise((x - seed) / 100.0, (y + seed) / 100.0) * 5000000 +SimplexNoise::noise((x - seed) / 35.0, (y + seed) / 35.0)*4000000;
				float obsidiannoise = (SimplexNoise::noise((x - seed) / 342.0, (y + seed) / 119.0) + 1) * 5000000000 + SimplexNoise::noise((x - seed) / 100.0, (y + seed) / 80.0) * 5000000;// +SimplexNoise::noise((x - seed) / 35.0, (y + seed) / 20.0) * 4000000;
				if ((y - h) * (y - h) * (y - h) > obsidiannoise)
				world[x + y * worldw] = std::pair<char, int>('o', obsidianLife);
				else if((y-h)*(y-h) * (y - h) >stonenoise)
				world[x + y * worldw] = std::pair<char, int>('s', stoneLife);
				else
				world[x + y * worldw] = std::pair<char, int>('d', dirtLife);
			}
		}
	}
	//std::srand(time(NULL));
	std::cout << "done!" << std::endl;
}
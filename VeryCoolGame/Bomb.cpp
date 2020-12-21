#include "Bomb.h"
#include <iostream>
#include "Explosion.h"
#include <cmath>

Bomb::Bomb(float initialX, float initialY, float Vx_or_Angle, float Vy_or_Vi, float BULLET_GRAVITY, float initialLife, float power, bool usingAngle)
{
	x = initialX;
	y = initialY;
	if (usingAngle) {
		vx = std::cos(Vx_or_Angle) * Vy_or_Vi;
		vy = std::sin(Vx_or_Angle) * Vy_or_Vi;
	}
	else {
		vx = Vx_or_Angle;
		vy = Vy_or_Vi;
	}
	g = BULLET_GRAVITY;
	life = initialLife;
	p = power;
	primed = false;
}

Bomb::~Bomb()
{
	//std::cout << "boom";
}

//void Bomb::act(std::pair<char, int>* world, int worldh, int worldw)
//{
//	int x0 = x;
//	int y0 = y;
//	--life;
//	y += vy;
//	vy += g;//-g*vy/50;
//	x += vx;
//	int x1 = x;
//	int y1 = y;
//	//simple estimated collision detect
//
//	//if (curx >= 0 && curx < worldw && cury >= 0 && cury < worldh && world[curx + cury * worldw].first != 'a' && world[curx + cury * worldw].first != 'f') {
//	//	life = 0;
//	//	x = curx;
//	//	y = cury;
//	//	return;
//	//}
//
//	int curx, cury, dx, dy, dx1, dy1, px, py, xe, ye, i;
//
//	// Calculate line deltas
//	dx = x1 - x0;
//	dy = y1 - y0;
//
//	// Create a positive copy of deltas (makes iterating easier)
//	dx1 = abs(dx);
//	dy1 = abs(dy);
//
//	// Calculate error intervals for both axis
//	px = 2 * dy1 - dx1;
//	py = 2 * dx1 - dy1;
//
//	// The line is X-axis dominant
//	if (dy1 <= dx1) {
//
//		// Line is drawn left to right
//
//		curx = x0; cury = y0; xe = x1;
//
//		if (world[curx + cury * worldw].first == 'a' && Explosion::fastrand() % 100 < 7) {
//			world[curx + cury * worldw].first = 'f';
//			world[curx + cury * worldw].second = Explosion::fastrand() % 33 + 15;
//		}
//		if (curx >= 0 && curx < worldw && cury >= 0 && cury < worldh && world[curx + cury * worldw].first != 'a' && world[curx + cury * worldw].first != 'f') {
//			life = 0;
//			x = curx;
//			y = cury;
//			{Explosion* e = new Explosion(x, y, p, world, worldh, worldw); }
//			return;
//		}
//
//		// Rasterize the line
//		if (dx >= 0) {
//			for (i = 0; curx < xe; i++) {
//				++curx;
//
//				// Deal with octants...
//				if (px < 0) {
//					px = px + 2 * dy1;
//				}
//				else {
//					if (dy > 0) {
//						++cury;
//					}
//					else {
//						--cury;
//					}
//					px = px + 2 * (dy1 - dx1);
//				}
//
//				// Draw pixel from line span at
//				// currently rasterized position
//				if (world[curx + cury * worldw].first == 'a' && Explosion::fastrand() % 100 < 7) {
//					world[curx + cury * worldw].first = 'f';
//					world[curx + cury * worldw].second = Explosion::fastrand() % 33 + 15;
//				}
//				if (curx >= 0 && curx < worldw && cury >= 0 && cury < worldh && world[curx + cury * worldw].first != 'a' && world[curx + cury * worldw].first != 'f') {
//					life = 0;
//					x = curx;
//					y = cury;
//					{Explosion* e = new Explosion(x, y, p, world, worldh, worldw); }
//					return;
//				}
//			}
//		}
//		else {
//			for (i = 0; curx > xe; i++) {
//				--curx;
//
//				// Deal with octants...
//				if (px < 0) {
//					px = px + 2 * dy1;
//				}
//				else {
//					if (dy > 0) {
//						++cury;
//					}
//					else {
//						--cury;
//					}
//					px = px + 2 * (dy1 - dx1);
//				}
//
//				// Draw pixel from line span at
//				// currently rasterized position
//				if (world[curx + cury * worldw].first == 'a' && Explosion::fastrand() % 100 < 7) {
//					world[curx + cury * worldw].first = 'f';
//					world[curx + cury * worldw].second = Explosion::fastrand() % 33 + 15;
//				}
//				if (curx >= 0 && curx < worldw && cury >= 0 && cury < worldh && world[curx + cury * worldw].first != 'a' && world[curx + cury * worldw].first != 'f') {
//					life = 0;
//					x = curx;
//					y = cury;
//					{Explosion* e = new Explosion(x, y, p, world, worldh, worldw); }
//					return;
//				}
//			}
//		}
//
//	}
//	else { // The line is Y-axis dominant
//
//	 // Line is drawn bottom to top
//
//		curx = x0; cury = y0; ye = y1;
//
//		if (world[curx + cury * worldw].first == 'a' && Explosion::fastrand() % 100 < 7) {
//			world[curx + cury * worldw].first = 'f';
//			world[curx + cury * worldw].second = Explosion::fastrand() % 33 + 15;
//		}
//		if (curx >= 0 && curx < worldw && cury >= 0 && cury < worldh && world[curx + cury * worldw].first != 'a' && world[curx + cury * worldw].first != 'f') {
//			life = 0;
//			x = curx;
//			y = cury;
//			{Explosion* e = new Explosion(x, y, p, world, worldh, worldw); }
//			return;
//		}
//
//		// Rasterize the line
//		if (dy >= 0) {
//			for (i = 0; cury < ye; i++) {
//				++cury;
//
//				// Deal with octants...
//				if (py <= 0) {
//					py = py + 2 * dx1;
//				}
//				else {
//					if (dx > 0) {
//						++curx;
//					}
//					else {
//						--curx;
//					}
//					py = py + 2 * (dx1 - dy1);
//				}
//
//				// Draw pixel from line span at
//				// currently rasterized position
//				if (world[curx + cury * worldw].first == 'a' && Explosion::fastrand() % 100 < 7) {
//					world[curx + cury * worldw].first = 'f';
//					world[curx + cury * worldw].second = Explosion::fastrand() % 33 + 15;
//				}
//				if (curx >= 0 && curx < worldw && cury >= 0 && cury < worldh && world[curx + cury * worldw].first != 'a' && world[curx + cury * worldw].first != 'f') {
//					life = 0;
//					x = curx;
//					y = cury;
//					{Explosion* e = new Explosion(x, y, p, world, worldh, worldw); }
//					return;
//				}
//			}
//		}
//		else {
//			for (i = 0; cury > ye; i++) {
//				--cury;
//
//				// Deal with octants...
//				if (py <= 0) {
//					py = py + 2 * dx1;
//				}
//				else {
//					if (dx > 0) {
//						++curx;
//					}
//					else {
//						--curx;
//					}
//					py = py + 2 * (dx1 - dy1);
//				}
//
//				// Draw pixel from line span at
//				// currently rasterized position
//				if (world[curx + cury * worldw].first == 'a' && Explosion::fastrand() % 100 < 7) {
//					world[curx + cury * worldw].first = 'f';
//					world[curx + cury * worldw].second = Explosion::fastrand() % 33 + 15;
//				}
//				if (curx >= 0 && curx < worldw && cury >= 0 && cury < worldh && world[curx + cury * worldw].first != 'a' && world[curx + cury * worldw].first != 'f') {
//					life = 0;
//					x = curx;
//					y = cury;
//					{Explosion* e = new Explosion(x, y, p, world, worldh, worldw); }
//					return;
//				}
//			}
//		}
//	}
//
//
//}

bool Bomb::collide(std::pair<char, int>* world, int worldh, int worldw, int curx, int cury)
{
	if (curx >= 0 && curx < worldw && cury >= 0 && cury < worldh) { //fire trail
		if (world[curx + cury * worldw].first == 'a' && Explosion::fastrand() % 100 < 7) {
			world[curx + cury * worldw].first = 'f';
			world[curx + cury * worldw].second = Explosion::fastrand() % 33 + 15;
		}
		if (world[curx + cury * worldw].first != 'a' && world[curx + cury * worldw].first != 'f') {
			life -= world[curx + cury * worldw].second;
			vx += vx < 0 ? .1 : -.1;
			vy += vy < 0 ? .1 : -.1;
			explodeX.emplace_back(curx);
			explodeY.emplace_back(cury);
			//{Explosion* e = new Explosion(curx, cury, p, world, worldh, worldw); }
			if (life < 0) {
				explodeX.emplace_back(curx);
				explodeY.emplace_back(cury);
				primed = false;
				int i = 0;
				for (int k : explodeX) {
					{Explosion* e = new Explosion(k, explodeY.front(), p + ((life < 0) ? life / 40.0 : 0 - world[k + explodeY.front() * worldw].second / 10.0), world, worldh, worldw); }
					explodeY.pop_front();
				}
				//x = curx;
				//y = cury;
				return true;
			}
		}
	}
	return false;
}

void Bomb::act(std::pair<char, int>* world, int worldh, int worldw)
{
	if (primed) {
		explodeX.clear();
		//explodeY.clear();
		int x0 = x;
		int y0 = y;
		life -= 1;
		y += vy;
		vy += g;//-g*vy/50;
		x += vx;
		int x1 = x;
		int y1 = y;
		//simple estimated collision detect

		//if (curx >= 0 && curx < worldw && cury >= 0 && cury < worldh && world[curx + cury * worldw].first != 'a' && world[curx + cury * worldw].first != 'f') {
		//	life = 0;
		//	x = curx;
		//	y = cury;
		//	return;
		//}

		int curx, cury, dx, dy, dx1, dy1, px, py, xe, ye, i;
		// Calculate line deltas
		dx = x1 - x0;
		dy = y1 - y0;
		// Create a positive copy of deltas (makes iterating easier)
		dx1 = abs(dx);
		dy1 = abs(dy);
		// Calculate error intervals for both axis
		px = 2 * dy1 - dx1;
		py = 2 * dx1 - dy1;
		// The line is X-axis dominant
		if (dy1 <= dx1) {
			curx = x0; cury = y0; xe = x1;

			if (collide(world, worldh, worldw, curx, cury))return;

			// Rasterize the line
			if (dx >= 0) {
				for (i = 0; curx < xe; i++) {
					++curx;

					// Deal with octants...
					if (px < 0) {
						px = px + 2 * dy1;
					}
					else {
						if (dy > 0) {
							++cury;
						}
						else {
							--cury;
						}
						px = px + 2 * (dy1 - dx1);
					}
					if (collide(world, worldh, worldw, curx, cury))return;
				}
			}
			else {
				for (i = 0; curx > xe; i++) {
					--curx;

					// Deal with octants...
					if (px < 0) {
						px = px + 2 * dy1;
					}
					else {
						if (dy > 0) {
							++cury;
						}
						else {
							--cury;
						}
						px = px + 2 * (dy1 - dx1);
					}
					if (collide(world, worldh, worldw, curx, cury))return;
				}
			}

		}
		else { // The line is Y-axis dominant
			curx = x0; cury = y0; ye = y1;

			if (collide(world, worldh, worldw, curx, cury))return;

			// Rasterize the line
			if (dy >= 0) {
				for (i = 0; cury < ye; i++) {
					++cury;

					// Deal with octants...
					if (py <= 0) {
						py = py + 2 * dx1;
					}
					else {
						if (dx > 0) {
							++curx;
						}
						else {
							--curx;
						}
						py = py + 2 * (dx1 - dy1);
					}
					if (collide(world, worldh, worldw, curx, cury))return;
				}
			}
			else {
				for (i = 0; cury > ye; i++) {
					--cury;

					// Deal with octants...
					if (py <= 0) {
						py = py + 2 * dx1;
					}
					else {
						if (dx > 0) {
							++curx;
						}
						else {
							--curx;
						}
						py = py + 2 * (dx1 - dy1);
					}
					if (collide(world, worldh, worldw, curx, cury))return;
				}
			}
		}
	
		for (int k : explodeX) {
			{Explosion* e = new Explosion(k, explodeY.front(), p + ((life < 0) ? life / 40.0 : 0- world[k + explodeY.front() * worldw].second/10.0), world, worldh, worldw); }
			explodeY.pop_front();
		}
	}
}

void Bomb::recycle(float initialX, float initialY, float Vx_or_Angle, float Vy_or_Vi, float BULLET_GRAVITY, float initialLife, float power, bool usingAngle)
{
	x = initialX;
	y = initialY;
	if (usingAngle) {
		vx = std::cos(Vx_or_Angle) * Vy_or_Vi;
		vy = std::sin(Vx_or_Angle) * Vy_or_Vi;
	}
	else {
		vx = Vx_or_Angle;
		vy = Vy_or_Vi;
	}
	g = BULLET_GRAVITY;
	life = initialLife;
	p = power;
	primed = true;
}

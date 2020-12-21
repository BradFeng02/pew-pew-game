#include "SDL.h"
#undef main
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include "WorldBuilder.h"
#include <vector>
#include "Explosion.h"
#include "SimplexNoise.h"
#include <cmath>
#include <tuple>
#include "Bomb.h"
#include <cmath>
#include <random>

int viewportw = 900;
int viewporth = 550;
int updatew = viewportw / 2 + 200;
int updateh = viewporth / 2 + 250;
int worldw = 7000;
int worldh = 4000;

float playerx = 0;
float playery = 0;
float pvx = 0;
float pvy = 0;

const int maxBombs = 1234;
const float BULLET_GRAVITY = .2;
const float PLAYER_GRAVITY = .2;
int drawx = 0;
int drawy = worldh / 2 - viewporth / 2;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* texture;
std::pair<char, int>* world;
std::atomic_flag running = ATOMIC_FLAG_INIT;
Bomb* bombs[maxBombs];
bool three = false;//USED FOR trimming dirt, dw about it

bool goinleft = false;
bool goinright = false;
bool goinup = false;
bool goindown = false;
bool lmousedown = false;
bool rmousedown = false;
bool shot = false;
int mousex = 0;
int mousey = 0;
//int dragstartmx = 0;
//int dragstartmy = 0;
//int dragstarttx = 0;
//int dragstartty = 0;
int cooldown = 0;
std::atomic_flag dragging = ATOMIC_FLAG_INIT;

void create_world(int height, int viewportwth, int seed) {
	world = new std::pair<char, int>[height * viewportwth];
	WorldBuilder::fill_terrain(seed, worldw, worldh, world);
}
void init_bombs() {
	for (int i = 0; i < maxBombs; ++i) {
		//Bomb bomb = Bomb(0, 0, 0, 0, 0, 0, 0, true);
		bombs[i] = new Bomb(-10, -10, -10, -10, -10, -10, -10, true);
	}
	//std::cout << "bombs created" << std::endl;
}
void spawn_player() {
	playerx = worldw / 2;
	for (int y = 0; y < worldh; ++y) {
		if (world[(int)playerx + y * worldw].first != 'a') {
			playery = y-50;
			return;
		}
	}
}

void update_buffer(Uint32* buffer, int ix, int iy) {
	for (int y = 0; y < viewporth; ++y) {
		for (int x = 0; x < viewportw; ++x) {
			int cx = ix + x;
			int cy = iy + y;
			if (cx >= worldw || cy >= worldh || cx < 0 || cy < 0) {//outside world
				buffer[x + y * viewportw] = 0x000000FF;
			}
			else {
				switch (world[cx + cy * worldw].first)
				{
				case 'a':
					buffer[x + y * viewportw] = 0xADECFFFF;//air
					break;
				case 'd':
				{
					//int t = cy - 1;
					//int b = cy + 1;
					//int l = cx - 1;
					//int r = cx + 1;
					//bool tb = t >= 0;
					//bool bb = b < worldh;
					//bool lb = l >= 0;
					//bool rb = r < worldw;
					if (cy - 1 >= 0 && world[cx + (cy - 1) * worldw].first == 'a'){
						buffer[x + y * viewportw] = 0x009900FF;//grass
					}
					else if ((cy + 1 < worldh && world[cx + (cy + 1) * worldw].first == 'a')||(cx+1<worldw&& world[cx + 1 + cy * worldw].first == 'a')||(cx - 1 >= 0&& world[cx - 1 + cy * worldw].first == 'a')) {
						buffer[x + y * viewportw] = 0x60402fFF;//border
					}
					else
						//buffer[x + y * viewportw] = dirt[((cx % 5 + cy % 5) / 5) % 2];//dirt
						buffer[x + y * viewportw] = 0x8B4513FF;
				}
				break;
				case 'f':
					//buffer[x + y * viewportw] = fire[((cx % 5 + cy % 5 + Explosion::fastrand() % 5) / 5) % 3];//fire
					if (world[cx + cy * worldw].second < 10) {
						buffer[x + y * viewportw] = 0xA5A5A5FF;//smoke
					}
					else {
						buffer[x + y * viewportw] = 0xFF5A00FF;//fire
					}
					break;
				case 's':
					buffer[x + y * viewportw] = 0x808080FF;
					break;
				case 'o':
					buffer[x + y * viewportw] = 0x1a0033FF;
					break;
				default:
					buffer[x + y * viewportw] = 0xFF00FFFF;
					break;
				}
			}
		}
	}
	int px = playerx - ix;
	int py = playery - iy;
	for (int x = px-4; x <= px + 4; ++x) {
		for (int y = py - 4; y <= py + 4; ++y) {
			if (x>=0&&x<viewportw&&y>=0&&y<viewporth&&(px-x) * (px-x) + (py-y) * (py-y) <= 16) {
				buffer[x + y * viewportw] = 0xFF00FFFF;
			}
		}
	}
	for (Bomb* b : bombs) {
		int bx = (int)b->x - ix;
		int by = (int)b->y - iy;
		if (b->primed && bx >= 0 && bx < viewportw-1 && by >= 0 && by < viewporth-1) {
			buffer[bx + by * viewportw] = 0x000000ff;
			buffer[bx+1 + by * viewportw] = 0x000000ff;
			buffer[bx + (by+1) * viewportw] = 0x000000ff;
			buffer[bx+1 + (by + 1) * viewportw] = 0x000000ff;
		}
	}
}
void render() {
	std::cout << "render starting..." << std::endl;
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, viewportw, viewporth);
	void* pixels;
	int pitch;
	int vpwdiv2 = viewportw / 2;
	int vphdiv2 = viewporth / 2;
	Uint32* buffer = new Uint32[viewportw * viewporth];
	while (running.test_and_set()) {
		auto start = std::chrono::steady_clock::now();
		auto startTime = std::chrono::time_point_cast<std::chrono::milliseconds>(start).time_since_epoch();
		///////

		//drawx = playerx - vpwdiv2;
		//drawy = playery - vphdiv2; //currently in process keys method for testing

		//if (!dragging.test_and_set()) {
		//	dragging.clear();
		//	drawx = (5 * drawx + playerx - viewportw / 2) / 6;
		//	drawy = (5 * drawy + playery - viewporth / 2) / 6;
		//}

		update_buffer(buffer, drawx, drawy);
		//SDL_RenderClear(renderer);
		SDL_LockTexture(texture, NULL, &pixels, &pitch);
		memcpy(pixels, buffer, pitch * viewporth);
		SDL_UnlockTexture(texture);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		///////
		auto end = std::chrono::steady_clock::now();
		auto endTime = std::chrono::time_point_cast<std::chrono::milliseconds>(end).time_since_epoch();
		std::chrono::milliseconds dt = std::chrono::milliseconds(16) - endTime + startTime;
		if (dt.count() <= 0)std::cout << dt.count() << " (R)" << std::endl;
		std::this_thread::sleep_for(dt);
	}
	running.clear();
	delete[] buffer;
	std::cout << "render exiting..."<<std::endl;
}

void scan_world() {
	for (int y = playery - updateh; y < playery + updateh; ++y) { //loop world
		for (int x = playerx - updatew; x < playerx + updatew; ++x) {
			if (y >= 0 && y < worldh && x >= 0 && x < worldw) {

				//set air
				if (world[x + y * worldw].second < 0) { //if health<0 set to air
						world[x + y * worldw].first = 'a';
						world[x + y * worldw].second = 0;
				}

				//fire sim
				else if (world[x + y * worldw].first == 'f') {//fire sim + decrease fire health
					--world[x + y * worldw].second;
					int blocked = (y > 0 && world[x + (y - 1) * worldw].first != 'a' && world[x + (y - 1) * worldw].first != 'f') ? 10000 : 0;
					int r = Explosion::fastrand() % 500 - world[x + y * worldw].second + blocked;
					if (r > 376) {
						int d = Explosion::fastrand() % 2;
						if (d == 0 && x > 0 && y > 0 && world[x - 1 + (y - 1) * worldw].first == 'a') {
							world[x - 1 + (y - 1) * worldw] = world[x + y * worldw];
							world[x + y * worldw].first = 'a';
							world[x + y * worldw].second = 0;
						}
						else if (d == 0 && x > 0 && world[x - 1 + y * worldw].first == 'a') {
							world[x - 1 + y * worldw] = world[x + y * worldw];
							world[x + y * worldw].first = 'a';
							world[x + y * worldw].second = 0;
						}
						else if (d == 1 && x < worldw - 1 && y > 0 && world[x + 1 + (y - 1) * worldw].first == 'a') {
							world[x + 1 + (y - 1) * worldw] = world[x + y * worldw];
							world[x + y * worldw].first = 'a';
							world[x + y * worldw].second = 0;
						}
						else if (d == 1 && x < worldw - 1 && world[x + 1 + y * worldw].first == 'a') {
							world[x + 1 + y * worldw] = world[x + y * worldw];
							world[x + y * worldw].first = 'a';
							world[x + y * worldw].second = 0;
						}
					}
					else if (r > 170 && y > 0 && world[x + (y - 1) * worldw].first == 'a') {
						world[x + (y - 1) * worldw] = world[x + y * worldw];
						world[x + y * worldw].first = 'a';
						world[x + y * worldw].second = 0;
					}

				}

				//trim dirt
				else if (world[x + y * worldw].first == 'd') { //remove dirt surround w/ 3 air
					if (((x > 0 && world[x - 1 + y * worldw].first == 'a')||(x > 0 && world[x - 1 + y * worldw].first == 'f'))?(((x < worldw - 1 && world[x + 1 + y * worldw].first == 'a')||(x < worldw - 1 && world[x + 1 + y * worldw].first == 'f')) ? (((y > 0 && world[x + (y - 1) * worldw].first == 'a')||(y > 0 && world[x + (y - 1) * worldw].first == 'f')) || ((y < worldh - 1 && world[x + (y + 1) * worldw].first == 'a')||(y < worldh - 1 && world[x + (y + 1) * worldw].first == 'f'))) : (((y > 0 && world[x + (y - 1) * worldw].first == 'a')||(y > 0 && world[x + (y - 1) * worldw].first == 'f')) && ((y < worldh - 1 && world[x + (y + 1) * worldw].first == 'a')||(y < worldh - 1 && world[x + (y + 1) * worldw].first == 'f')))):(((x < worldw - 1 && world[x + 1 + y * worldw].first == 'a')||(x < worldw - 1 && world[x + 1 + y * worldw].first == 'f'))&&((y > 0 && world[x + (y - 1) * worldw].first == 'a')||(y > 0 && world[x + (y - 1) * worldw].first == 'f'))&&((y < worldh - 1 && world[x + (y + 1) * worldw].first == 'a')||(y < worldh - 1 && world[x + (y + 1) * worldw].first == 'f')))) {
						world[x + y * worldw].first = 'a';
						world[x + y * worldw].second = 0;
					}
				}
			}
		}
	}
}
void update_world() {
	std::cout << "world starting..." << std::endl;
	while (running.test_and_set()) {
		auto start = std::chrono::steady_clock::now();
		auto startTime = std::chrono::time_point_cast<std::chrono::milliseconds>(start).time_since_epoch();
		///////
		scan_world();
		///////
		auto end = std::chrono::steady_clock::now();
		auto endTime = std::chrono::time_point_cast<std::chrono::milliseconds>(end).time_since_epoch();
		std::chrono::milliseconds dt = std::chrono::milliseconds(16) - endTime + startTime;
		if (dt.count() <= 0)std::cout << dt.count() << " (W)" << std::endl;
		std::this_thread::sleep_for(dt);
	}
	running.clear();
	std::cout << "world exiting..."<<std::endl;
}

void spawn_explosion(float x, float y, float radius) {
	{
		Explosion* e = new Explosion(drawx + x, drawy + y, radius, world, worldh, worldw);
	}
}
void spawn_bomb(float x, float y, float Vx_or_Angle, float Vy_or_Vi, float grav, int life, float power, bool usingAngle) {
	Bomb* oldest=bombs[0];
	for (Bomb* b : bombs) {
		if (b->life < oldest->life)oldest = b;
		if (!b->primed) {
			b->recycle(x, y, Vx_or_Angle, Vy_or_Vi, grav, life, power, usingAngle);
			return;
		}
	}
	oldest->recycle(x, y, Vx_or_Angle, Vy_or_Vi, grav, life, power, usingAngle);
}

void process_keys(const Uint8* keys) {
	if (keys[SDL_SCANCODE_W])drawy -= 5; //move
	if (keys[SDL_SCANCODE_S])drawy += 5;
	if (keys[SDL_SCANCODE_A])playerx -= 5;
	if (keys[SDL_SCANCODE_D])playerx += 5;
	if (keys[SDL_SCANCODE_C]) {
		drawx = playerx - viewportw / 2;
		drawy = playery - viewporth / 2;
	}
	if (keys[SDL_SCANCODE_SPACE] && shot == false) {
		spawn_explosion(mousex, mousey, 50);
		//for (double i = -3.14159; i < 3.141; i+=0.0055)
		//	spawn_bomb(mousex+drawx, mousey+drawy, atan2(mousey + drawy - playery, mousex + drawx - playerx) + i, 15, 0, 1, 5, true);
		shot = true;
	}
	else if (!keys[SDL_SCANCODE_SPACE]) {
		shot = false;
	}
}
void process_mousebuttons() {
	Uint32 state = SDL_GetMouseState(&mousex, &mousey);
	if (state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
		if (cooldown <= 0) {
			for (int i=-5;i<5;++i)
			spawn_bomb(playerx, playery, atan2(mousey + drawy - playery, mousex + drawx - playerx) +i/25.0+Explosion::fastrand()%1000/25000.0-1/50.0, 11+Explosion::fastrand() % 4/5.0, BULLET_GRAVITY, 20+Explosion::fastrand()%10, 3, true);
			//int temp = std::sqrt((mousey + drawy - playery) * (mousey + drawy - playery) + (mousex + drawx - playerx) * (mousex + drawx - playerx))/10;
			//spawn_bomb(playerx, playery, atan2(mousey + drawy - playery, mousex + drawx - playerx), temp, gravity, temp*10, 3, true);
			cooldown = 10;
		}
		lmousedown = true;
	}
	else lmousedown = false;
	--cooldown;

	if (state & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
		if (!rmousedown) {
			playery = mousey + drawy;
			playerx = mousex + drawx;
		}
		rmousedown = true;
	}
	else {
		rmousedown = false;
	}
	//if (state & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
	//	if (!rmousedown) {
	//		dragstartmx = mousex;
	//		dragstartmy = mousey;
	//		dragstarttx = drawx;
	//		dragstartty = drawy;
	//		dragging.test_and_set();
	//	}
	//	drawx = dragstarttx - mousex + dragstartmx;
	//	drawy = dragstartty - mousey + dragstartmy;
	//	rmousedown = true;
	//}
	//else {
	//	rmousedown = false;
	//	dragging.clear();
	//}
}
void process_bombs() {
	for (Bomb* b : bombs) {
		b->act(world, worldh, worldw);
		//if (b->life == 0)
		//{
		//	{Explosion* e = new Explosion(b->x, b->y, b->p, world, worldh, worldw); }
		//}
		//--b->life;
	}
}

bool collide(int px,int py) {
	for (int x = px - 4; x <= px + 4; ++x) {
		for (int y = py - 4; y <= py + 4; ++y) {
			if (x >= 0 && x < worldw && y >= 0 && y < worldh && (px - x) * (px - x) + (py - y) * (py - y) <= 16) {
				char test = world[x + y * worldw].first;
				if (test != 'a' && test != 'f')return true;
			}
		}
	}
	return false;
}
bool check_traj(float x0,float y0, float x1, float y1) {
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

		if (collide(curx,cury)) {
			return true;
		}

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
				if (collide(curx,cury)) {
					playerx = curx;
					playery = cury;
					return true;
				}
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
				if (collide(curx,cury)) {
					playerx = curx;
					playery = cury;
					return true;
				}
			}
		}

	}
	else { // The line is Y-axis dominant
		curx = x0; cury = y0; ye = y1;

		if (collide(curx,cury)) {
			return true;
		}

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
				if (collide(curx,cury)) {
					playerx = curx;
					playery = cury;
					return true;
				}
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
				if (collide(curx,cury)) {
					playerx = curx;
					playery = cury;
					return true;
				}
			}
		}
	}
	playerx += pvx;
	playery += pvy;
	return false;
}
void play_physics() {
	bool col=check_traj(playerx, playery, playerx + pvx, playery + pvy);//true if collision
	
	pvy += PLAYER_GRAVITY;
	if (col)pvy = 0;
}

int main() {
	std::mt19937 mtrand(time(0));
	init_bombs();
	running.test_and_set(); //tell threads program is running
	SDL_Init(SDL_INIT_EVERYTHING);
	window = NULL;
	while (window == NULL) {
		window = SDL_CreateWindow("Very Cool Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, viewportw, viewporth, SDL_WINDOW_SHOWN);
	}
	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	create_world(worldw, worldh, mtrand()%1000000-500000);
	spawn_player();
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	std::thread render(render); //create render thread
	std::thread update_world(update_world); //create world process thread
	bool isRunning = true;
	while (isRunning) {
		auto start = std::chrono::steady_clock::now();
		auto startTime = std::chrono::time_point_cast<std::chrono::milliseconds>(start).time_since_epoch();
		///////

		process_keys(keys);
		process_mousebuttons();
		process_bombs();
		//process_world();
			
		play_physics();//test for now, maybe own thread

		///////
		SDL_Event event;
		SDL_PollEvent(&event);
		switch (event.type) {
		case SDL_QUIT:
			isRunning = false;
			break;
		default:
			break;
		}
		//
		auto end = std::chrono::steady_clock::now();
		auto endTime = std::chrono::time_point_cast<std::chrono::milliseconds>(end).time_since_epoch();
		std::chrono::milliseconds dt = std::chrono::milliseconds(16) - endTime + startTime;
		if(dt.count()<=0)std::cout << dt.count()<<" (L)"<<std::endl;
		std::this_thread::sleep_for(dt);
	}
	std::cout << "shutting down..." << std::endl;
	running.clear();//tell threads program stopped
	render.join();
	update_world.join();
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(texture);
	delete[] world;
	SDL_Quit();
	std::cout<< "bye" << std::endl;
	return 0;
}
#pragma once
//#include <vector>

const int winHgt = 720;
const int winWid = 1280;

const double step = 1.0f / 60.0f;

static const double DEGTORAD = 3.14159265358979323846 / 180.0;
static const double RADTODEG = 180.0 / 3.14159265358979323846;

extern unsigned int solidboxshader;
extern unsigned int gradientshader;
extern unsigned int solidboxvao;
extern unsigned int solidboxvbo;
extern unsigned int solidboxebo;
extern unsigned int textshader;
extern unsigned int textvao;
extern unsigned int textvbo;
extern unsigned int partvao;
extern unsigned int partvbo;
const unsigned int boxindices[] = {
0, 1, 3,   // first triangle
1, 2, 3    // second triangle
};

const float scale = 70.0f; //each box2d meter is [scale] pixels
extern std::unique_ptr<b2World> phys_world;
extern b2ParticleSystem* part_sys;
//extern std::vector<Solid*> terrain;

extern float phys2pxlX(float physxcoord);
extern float phys2pxlY(float physycoord);
extern float phys2glX(float physxcoord);
extern float phys2glY(float physycoord);
extern float pxl2physX(float pxlxcoord);
extern float pxl2physY(float pxlycoord);
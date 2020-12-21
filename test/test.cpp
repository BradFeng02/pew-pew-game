#include <windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
//#include <cstddef>
using namespace std;

GLFWwindow* window;
const int winWid = 900;
const int winHgt = 600;
const int worldHgt = 600;
const int worldWid = 900;
int viewx = 0;
int viewy = 0;

struct particle
{
	char type;
	int life;
	bool fixed;
	bool flag1;
	bool flag2;
};

uint32_t* buffer;
particle* world;
float particle_colors[8] = {0,0,0,1,1,.5,.2,1}; //r,g,b,a

static unsigned long x = 123456789, y = 362436069, z = 521288629;
unsigned long xorshf96() {          //period 2^96-1
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

bool set_up() {
	/////glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_SCALE_TO_MONITOR, 1);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(winWid, winHgt, "test", NULL, NULL);
	if (window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	/////glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	/////success
	return true;
}

void shader_setup() {
	const char* fragshadersrc = "#version 460 core\nin vec4 gl_FragCoord;\nout vec4 FragColor;\nlayout(location = 3) uniform int wid;\nlayout(location = 4) uniform vec4 colors[2];\nlayout (std430, binding=2) buffer world_data\n{\nuint world[];\n};\nvoid main(){\nFragColor=colors[world[int(gl_FragCoord.x)+int(gl_FragCoord.y)*wid]];\n}";
	const char* vertshadersrc = "#version 460 core\nconst vec2 triVertices[3] = { vec2(-1.0, -1.0), vec2(3.0, -1.0), vec2(-1.0, 3.0) };\nvoid main(){\ngl_Position = vec4(triVertices[gl_VertexID], 0.0, 1.0);\n}";

	unsigned int fragshader;
	fragshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragshader, 1, &fragshadersrc, NULL);
	glCompileShader(fragshader);
	int  success;
	char infoLog[512];
	glGetShaderiv(fragshader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragshader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int vertshader;
	vertshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertshader, 1, &vertshadersrc, NULL);
	glCompileShader(vertshader);
	success;
	infoLog[512];
	glGetShaderiv(vertshader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertshader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertshader);
	glAttachShader(shaderProgram, fragshader);
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);
	glDeleteShader(vertshader);
	glDeleteShader(fragshader);

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint ssbo;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 4 * winWid * winHgt, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo);
}

double mx, my;
int state;
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	
	glfwGetCursorPos(window, &mx, &my);
	state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS)
	{
		for (int x = -5; x < 6; ++x) {
			for (int y = -5; y < 6; ++y) {
				if (x * x + y * y < 25 && x + (int)mx + viewx>=0 && x + (int)mx + viewx<worldWid && y + winHgt - (int)my + viewy>=0 && y + winHgt - (int)my + viewy<worldHgt) {
					world[x+(int)mx + viewx + (y+winHgt - (int)my + viewy) * worldWid].type = 1;
					world[x+(int)mx + viewx + (y+winHgt - (int)my + viewy) * worldWid].fixed = 0;
				}
			}
		}
	}
	state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	if (state == GLFW_PRESS)
	{
		for (int x = -5; x < 6; ++x) {
			for (int y = -5; y < 6; ++y) {
				if (x * x + y * y < 25 && x + (int)mx + viewx >= 0 && x + (int)mx + viewx < worldWid && y + winHgt - (int)my + viewy >= 0 && y + winHgt - (int)my + viewy < worldHgt) {
					world[x + (int)mx + viewx + (y + winHgt - (int)my + viewy) * worldWid].type = 0;
				}
			}
		}	
	}
}

void render() {
	for (int x = 0; x < winWid; ++x) {
		for (int y = 0; y < winHgt; ++y) {
			int offx = x + viewx;
			int offy = y + viewy;
			if (offx < 0 || offx >= worldWid || offy < 0 || offy >= worldHgt)buffer[x + y * winWid] = 0;
			else {
				buffer[x + y * winWid] = (int)world[offx + offy * worldWid].type;
			}
		}
	}
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 4 * winWid * winHgt, buffer);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glfwSwapBuffers(window);
}
// if (y > 0 && ((x > 0 && world[x - 1 + (y - 1) * worldWid].type == 0) || (x < worldWid - 1 && world[x + 1 + (y - 1) * worldWid].type == 0))) 
bool* finished=new bool[worldWid * worldHgt];
bool alt = 0;
//void swapParticles(particle &a,particle &b) {
//	//particle temp = a;
//	//a = b;
//	//b = temp;
//	//if (alt) {
//	//	a.flag1 = 1;
//	//	b.flag1 = 1;
//	//}
//	//else {
//	//	a.flag2 = 1;
//	//	b.flag2 = 1;
//	//}
//	particle temp = a;
//	a = b;
//	b = temp;
//}
void swapParticles(int x1,int y1,int x2,int y2) {
	//particle temp = a;
	//a = b;
	//b = temp;
	//if (alt) {
	//	a.flag1 = 1;
	//	b.flag1 = 1;
	//}
	//else {
	//	a.flag2 = 1;
	//	b.flag2 = 1;
	//}
	particle temp = world[x1 + y1 * worldWid];
	world[x1 + y1 * worldWid] = world[x2 + y2 * worldWid];
	world[x2 + y2 * worldWid] = temp;
	finished[x1 + y1 * worldWid] = 1;
	finished[x2 + y2 * worldWid] = 1;
}
bool unupdated(particle p) {
	return 0 == (alt ? p.flag1 : p.flag2);
}
bool isType(int x, int y, char type) {
	return y >= 0 && y < worldHgt && x >= 0 && x < worldWid && world[x + y * worldWid].type == type;
}
bool done(int x, int y) {
	return y >= 0 && y < worldHgt && x >= 0 && x < worldWid && finished[x + y * worldWid];
}
void updater(int x,int y) {
	/*
	if (alt)world[x + y * worldWid].flag2 = 0;
	else world[x + y * worldWid].flag1 = 0;
	if (!world[x + y * worldWid].fixed && unupdated(world[x + y * worldWid])) {
		if (y > 0 && world[x + (y - 1) * worldWid].type == 0 &&unupdated(world[x + (y - 1) * worldWid])) { //can move down to air
			//world[x + (y - 1) * worldWid] = world[x + y * worldWid];
			//if (alt)world[x + (y - 1) * worldWid].flag1 = 1;
			//else world[x + (y - 1) * worldWid].flag2 = 1;
			//world[x + y * worldWid] = { 0,0,1 };
			swapParticles(world[x + y * worldWid], world[x + (y - 1) * worldWid]);
		}
		else if(y > 0){
			if (x > 0 && unupdated(world[x - 1 + (y - 1) * worldWid])&&world[x - 1 + (y - 1) * worldWid].type == 0 && (xorshf96()%2 == 0 || x >= worldWid - 1 || world[x + 1 + (y - 1) * worldWid].type != 0)) {
				swapParticles(world[x + y * worldWid], world[x - 1 + (y - 1) * worldWid]);
			}
			else if (x < worldWid - 1 && unupdated(world[x + 1 + (y - 1) * worldWid])&&world[x + 1 + (y - 1) * worldWid].type == 0) {
				swapParticles(world[x + y * worldWid], world[x + 1 + (y - 1) * worldWid]);
			}
		}
	}
	*/

	if (!done(x,y)&& isType(x,y,1)) {
		if (isType(x,y-1,0)&&!done(x,y-1) ) { //can move down to air
			swapParticles(x,y, x,y-1);
		}
		else{
			int dir = xorshf96() % 2==0?1:-1;
			if (!isType(x + dir, y - 1, 0)||done(x+dir,y-1)) dir *= -1;
			if (isType(x + dir, y - 1, 0)&&!done(x+dir,y-1)) swapParticles(x, y, x + dir, y - 1);
			else finished[x + y * worldWid] = 1;
		}
	}
}
int direction = 0;
void update_world() {
	memset(finished, 0, worldWid*worldHgt);
	switch (direction) {
	case 0:
		for (int x = 0; x < worldWid; ++x) {
			for (int y = 0; y <worldHgt; ++y) {
				updater(x, y);
			}
		}
		break;
	case 1:
		for (int x = 0; x < worldWid; ++x) {
			for (int y = worldHgt - 1; y >= 0; --y) {
				updater(x, y);
			}
		}
		break;
	case 2:
		for (int x = worldWid-1; x >=0; --x) {
			for (int y = 0; y < worldHgt; ++y) {
				updater(x, y);
			}
		}
		break;
	case 3:
		for (int x = worldWid - 1; x >= 0; --x) {
			for (int y = worldHgt - 1; y >= 0; --y) {
				updater(x, y);
			}
		}
		break;
	}
	//direction = direction < 3 ? direction + 1 : 0;
	direction = xorshf96()%4;
	alt = !alt;
}

int main() {
	if (!set_up()) return -1;
	glViewport(0, 0, winWid, winHgt);
	shader_setup();
	glUniform1i(3, winWid);
	glUniform4fv(4, 4,particle_colors);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
	buffer = new uint32_t[winWid * winHgt];
	world = new particle[worldHgt * worldWid];
	for (int x = 0; x < worldWid; ++x) {
		for (int y = 0; y < worldHgt; ++y) {
			world[x + y * worldWid].type = xorshf96() % 2;
			world[x + y * worldWid].fixed = 0;
			world[x + y * worldWid].flag1 = 0;
			world[x + y * worldWid].flag2 = 0;
		}
	}

	double starttime;
	while (!glfwWindowShouldClose(window)) //main loop!!!!!!!!!!!!!!!!!
	{
		starttime = glfwGetTime();
		processInput(window);
		update_world();
		render();
		glfwPollEvents();
		cout << (glfwGetTime() - starttime) * 1000 << endl;
		while (glfwGetTime() - starttime < 0.01666666667);
		//while (glfwGetTime() - starttime < 0.1);
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glfwTerminate();
	return 0;
}
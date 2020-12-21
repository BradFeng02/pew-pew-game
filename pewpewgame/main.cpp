#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Box2D/Box2D.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "physcontactlistener.h"
#include "shadersrcs.h"
#include "common.h"
#include <iostream>
#include <vector>
#include <atomic>
#include <map>
#include <string>
#include "ControllablePlayer.h"
#include "Solid.h"
using namespace std;

double frametime=1;
GLFWwindow* window;

unsigned int solidboxshader;
unsigned int gradientshader;
unsigned int solidboxvao;
unsigned int solidboxvbo;
unsigned int solidboxebo;
unsigned int textshader;
unsigned int textvao;
unsigned int textvbo;
unsigned int partvao;
unsigned int partvbo;
std::unique_ptr<b2World> phys_world;
b2ParticleSystem* part_sys;
vector<Solid*> terrain;



FT_Library ftlibrary;
FT_Face ftface;
float fpsx = 0.0f;
float fpsy = winHgt - 20.0f;
float fpsscale = 0.5f;
struct Character {
	GLuint     TextureID;  // ID handle of the glyph texture
	int sizex;
	int sizey;// Size of glyph
	int bearingx;
	int bearingy;// Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};
std::map<GLchar, Character> Characters;

const b2ParticleSystemDef particleSystemDef;
contactlistener physcontactlistener;
vector<b2Body*> testboxes;
b2Body* groundBody1;
b2Body* groundBody2;
b2Body* groundBody3;
b2Body* testdummy;
float partrad = 0.05f;

int testscore = 0;
double mousex;
double mousey;
atomic_bool sucking = false;
atomic_bool flowing = false;
int bulletID = 2;
int dummyID = 1;
ControllablePlayer* player;

float phys2pxlX(float physxcoord) {
	return physxcoord * scale - winWid / 2.0f;
}
float phys2pxlY(float physycoord) {
	return physycoord * scale - winHgt / 2.0f;
}
float phys2glX(float physxcoord) {
	return physxcoord * scale / winWid * 2;
}
float phys2glY(float physycoord) {
	return physycoord * scale / winHgt * 2;
}
float pxl2physX(float pxlxcoord) {
	return (pxlxcoord - winWid / 2.0f) / scale;
}
float pxl2physY(float pxlycoord) {
	return (-pxlycoord + winHgt / 2.0f) / scale;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
bool init_opengl() {
	//glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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
	//glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return false;
	}
	//setup
	glViewport(0, 0, winWid, winHgt);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	//
	return true;
}
void init_GLobjs() {
	//VAO
	glGenVertexArrays(1, &solidboxvao);
	glBindVertexArray(solidboxvao);///!
	//VBO
	glGenBuffers(1, &solidboxvbo);
	glBindBuffer(GL_ARRAY_BUFFER, solidboxvbo);
	//setup vertex attrib pointer
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
	//EBO
	glGenBuffers(1, &solidboxebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, solidboxebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boxindices), boxindices, GL_STATIC_DRAW);
	//cleanup
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//font objs
	glGenVertexArrays(1, &textvao);
	glBindVertexArray(textvao);
	glGenBuffers(1, &textvbo);
	glBindBuffer(GL_ARRAY_BUFFER, textvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	//particles
	glGenVertexArrays(1, &partvao);
	glBindVertexArray(partvao);
	glGenBuffers(1, &partvbo);
	glBindBuffer(GL_ARRAY_BUFFER, partvbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void create_shader_program(const char* vertShaderSrc, const char* fragShaderSrc, unsigned int* shaderPrgm) {
	int success;
	char infoLog[512];
	////vert shader
	unsigned int vertShader;
	vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
	glCompileShader(vertShader);
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	////frag shader
	unsigned int fragShader;
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
	glCompileShader(fragShader);
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	////shader program
	*shaderPrgm = glCreateProgram();
	glAttachShader(*shaderPrgm, vertShader);
	glAttachShader(*shaderPrgm, fragShader);
	glLinkProgram(*shaderPrgm);
	glUseProgram(*shaderPrgm);
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
}
void init_shaders() {
	//solid box
	create_shader_program(solidboxvert, solidboxfrag, &solidboxshader);
	//gradient rainbow thing
	create_shader_program(gradientvert, gradientfrag, &gradientshader);
	//text
	create_shader_program(textvert, textfrag, &textshader);
	glm::mat4 projection = glm::ortho(0.0f, (float)winWid, 0.0f, (float)winHgt);
	glUniformMatrix4fv(glGetUniformLocation(textshader, "projection"),1,GL_FALSE,glm::value_ptr(projection));
}
void init_fonts() {
	int error = FT_Init_FreeType(&ftlibrary);
	if (error) cout << "uh oh true type broken!!11!"<<endl;
	error = FT_New_Face(ftlibrary, "C:\\Users\\brad\\Desktop\\pew pew game\\pewpewgame\\font\\OpenSans-Regular.ttf", 0, &ftface);
	if (error == FT_Err_Unknown_File_Format) cout << "uh oh font format not supported" << endl;
	else if (error) cout << "uh oh font loading broken!!!!!!1" << endl;
	//error = FT_Set_Char_Size(ftface, 0, 16 * 64, 300, 300);
	FT_Set_Pixel_Sizes(ftface, 0, 50);
	//preload char bitmaps (first 128 ascii)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(ftface, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			ftface->glyph->bitmap.width,
			ftface->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			ftface->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			ftface->glyph->bitmap.width, ftface->glyph->bitmap.rows,
			ftface->glyph->bitmap_left, ftface->glyph->bitmap_top,
			ftface->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	FT_Done_Face(ftface);
	FT_Done_FreeType(ftlibrary);
}
//density is ((real density)^(1/3))^2 because its 2d not 3d
void makeTESTbodies() {
	//make ground
	
	//make box
	//b2BodyDef boxbd;
	//boxbd.type = b2_dynamicBody;
	//boxbd.position.Set(-0.5f, 0.0f);
	////boxbd.userData = (void*)bulletID;
	//b2Body* boxbody = phys_world->CreateBody(&boxbd);
	//b2PolygonShape boxShape;
	//boxShape.SetAsBox(0.15f, 0.15f);
	//b2FixtureDef boxfd;
	//boxfd.shape = &boxShape;
	//boxfd.density = 50.0f;
	//boxfd.friction = 0.3f;
	//boxfd.restitution = 0.5f;
	//b2Fixture* boxfixture = boxbody->CreateFixture(&boxfd);
	//testboxes.emplace_back(boxbody);
}
void makeTESTdummy() {
	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.bullet = true;
	bd.position.Set(0.0f, 0.0f);
	bd.fixedRotation = true;
	bd.userData = (void*)dummyID;
	testdummy = phys_world->CreateBody(&bd);
	b2PolygonShape box;
	box.SetAsBox(0.2f, 0.95f);
	b2FixtureDef fd;
	fd.shape = &box;
	fd.density = 1000.0f;
	fd.friction = 0.3f;
	testdummy->CreateFixture(&fd);
}
void init_phys() {
	b2Vec2 gravity(0.0f, -9.807f);
	phys_world = make_unique<b2World>(gravity);
	part_sys = phys_world->CreateParticleSystem(&particleSystemDef);
	part_sys->SetDestructionByAge(true);
	part_sys->SetRadius(partrad);
	part_sys->SetDensity(1000.0f);//1000
	phys_world->SetContactListener(&physcontactlistener);
	//
	makeTESTbodies();
	//makeTESTdummy();
}

void RenderText(string text, GLfloat x, GLfloat y, GLfloat ftscale)//, glm::vec3 color)
{
	// Activate corresponding render state	
	glUseProgram(textshader);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textvao);
	//glUniform3f(glGetUniformLocation(s.Program, "textColor"), color.x, color.y, color.z);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];
		GLfloat posx = x + ch.bearingx * ftscale;
		GLfloat posy = y - (ch.sizey - ch.bearingy) * ftscale;

		GLfloat w = ch.sizex * ftscale;
		GLfloat h = ch.sizey * ftscale;
		// Update VBO for each character
		GLfloat ftvertices[6][4] = {
			{ posx,     posy + h,   0.0, 0.0 },
			{ posx,     posy,       0.0, 1.0 },
			{ posx + w, posy,       1.0, 1.0 },

			{ posx,     posy + h,   0.0, 0.0 },
			{ posx + w, posy,       1.0, 1.0 },
			{ posx + w, posy + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, textvbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ftvertices), ftvertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * ftscale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void RenderBoxBody(b2Body* boxbd) {
	glBindVertexArray(solidboxvao);
	glBindBuffer(GL_ARRAY_BUFFER, solidboxvbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, solidboxebo);
	//
	b2PolygonShape* s = (b2PolygonShape*)boxbd->GetFixtureList()->GetShape();
	float vertices[] = {
		 boxbd->GetWorldPoint(s->GetVertex(0)).x* scale / winWid * 2, boxbd->GetWorldPoint(s->GetVertex(0)).y* scale / winHgt * 2,
		 boxbd->GetWorldPoint(s->GetVertex(1)).x* scale / winWid * 2, boxbd->GetWorldPoint(s->GetVertex(1)).y* scale / winHgt * 2,
		 boxbd->GetWorldPoint(s->GetVertex(2)).x* scale / winWid * 2, boxbd->GetWorldPoint(s->GetVertex(2)).y* scale / winHgt * 2,
		 boxbd->GetWorldPoint(s->GetVertex(3)).x* scale / winWid * 2, boxbd->GetWorldPoint(s->GetVertex(3)).y* scale / winHgt * 2
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
void RenderParticles() {
	auto particles = part_sys->GetPositionBuffer();
	for (int i = 0; i < part_sys->GetParticleCount(); ++i) {
		float posx = particles->x;
		float posy = particles->y;
		//cout << posx << ", " << posy << endl;
		float s = partrad;// -0.02f;
		float vertices[] = {
		 (posx - s) * scale  / winWid * 2, (posy + s) * scale / winHgt * 2,
		 (posx + s) * scale  / winWid * 2, (posy + s) * scale / winHgt * 2,
		 (posx + s) * scale  / winWid * 2, (posy - s) * scale / winHgt * 2,
		 (posx - s) * scale  / winWid * 2, (posy - s) * scale / winHgt * 2
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		++particles;
	}
}
void drawtestlines() {
	glUniform3f(glGetUniformLocation(solidboxshader, "color"), 0.65f, 0.65f, 0.65f);
	glBindVertexArray(solidboxvao);
	glBindBuffer(GL_ARRAY_BUFFER, solidboxvbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, solidboxebo);
	//
	for (int x = -winWid / scale / 2; x < winWid / scale / 2; ++x) {
		float vertices[] = {
		 x * scale / winWid * 2 - .002f, 1,
		 x * scale / winWid * 2, 1,
		 x * scale / winWid * 2, -1,
		 x * scale / winWid * 2 - .002f, -1
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	for (int y = -winHgt / scale / 2; y < winHgt / scale / 2; ++y) {
		float vertices[] = {
		 -1,y * scale / winHgt * 2 + .002f,
		 1,y * scale / winHgt * 2 + .002f,
		 1,y * scale / winHgt * 2,
		 -1,y * scale / winHgt * 2
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	glUniform3f(glGetUniformLocation(solidboxshader, "color"), 0.0f, 0.0f, 0.0f);
	float x[] = {
		 -.002f, 1,
		 0, 1,
		 0, -1,
		 - .002f, -1
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(x), x, GL_DYNAMIC_DRAW);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	float y[] = {
		 -1, .002f,
		 1,.002f,
		 1,0,
		 -1,0
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(y), y, GL_DYNAMIC_DRAW);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
void render() {
	glClear(GL_COLOR_BUFFER_BIT);
	//
	//glUseProgram(solidboxshader);
	//glUniform3f(glGetUniformLocation(solidboxshader, "color"), 0.0, 0.0, 0.0);
	for (Solid* s : terrain) s->render();
	//glUniform3f(glGetUniformLocation(solidboxshader, "color"), 1.0, 0.0, 0.0);
	player->render();
	//
	//glUseProgram(gradientshader);
	//for (auto b : testboxes) RenderBoxBody(b);
	glUseProgram(solidboxshader);
	glUniform3f(glGetUniformLocation(solidboxshader, "color"), 0.0f, 0.1f, 1.0f);
	RenderParticles();
	drawtestlines();
	//fps
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	RenderText(to_string(round(1 / frametime * 100.0) / 100.0).substr(0,4), fpsx, fpsy, fpsscale);
	//
	b2Vec2 dummytextpos(0, 0);
	RenderText(to_string(testscore), dummytextpos.x * scale + winWid / 2.0f-10, dummytextpos.y* scale + winHgt / 2.0f+100, 0.5f);
	glDisable(GL_BLEND);
	//
	glfwSwapBuffers(window);
}

bool keystates[350] = { 0 };
void processInput()
{
	//if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_D) != keystates[GLFW_KEY_D]) {
		if (glfwGetKey(window, GLFW_KEY_D)) player->right_pressed();
		else player->right_released();
		keystates[GLFW_KEY_D] = glfwGetKey(window, GLFW_KEY_D);
	}
	if (glfwGetKey(window, GLFW_KEY_A) != keystates[GLFW_KEY_A]) {
		if (glfwGetKey(window, GLFW_KEY_A)) player->left_pressed();
		else player->left_released();
		keystates[GLFW_KEY_A] = glfwGetKey(window, GLFW_KEY_A);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) != keystates[GLFW_KEY_SPACE]) {
		if (glfwGetKey(window, GLFW_KEY_SPACE)) player->jump_pressed();
		else player->jump_released();
		keystates[GLFW_KEY_SPACE] = glfwGetKey(window, GLFW_KEY_SPACE);
	}
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		sucking = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		sucking = false;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		flowing = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		flowing = false;
	}
	/*else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		glfwGetCursorPos(window, &mousex, &mousey);
		b2BodyDef boxbd;
		boxbd.type = b2_dynamicBody;
		boxbd.position.Set((mousex - winWid / 2.0) / scale, (-mousey + winHgt / 2.0) / scale);
		boxbd.userData = (void*)bulletID;
		b2PolygonShape boxShape;
		boxShape.SetAsBox(0.15f, 0.15f);
		b2FixtureDef boxfd;
		boxfd.shape = &boxShape;
		boxfd.density = 50.0f;
		boxfd.friction = 0.3f;
		b2ParticleDef pd;
		pd.flags = b2_powderParticle;
		pd.color.Set(0, 0, 255, 255);
		pd.position.Set((mousex - winWid / 2.0) / scale, (-mousey + winHgt / 2.0) / scale);
		pd.userData = (void*)bulletID;
		while (phys_world->IsLocked());
		b2Body* boxbody = phys_world->CreateBody(&boxbd);
		b2Fixture* boxfixture = boxbody->CreateFixture(&boxfd);
		testboxes.emplace_back(boxbody);
		int tempIndex = part_sys->CreateParticle(pd);
		part_sys->SetParticleLifetime(tempIndex, 5.0f);
	}*/
}

int main() {
	cout << "hi" << endl;
	if (!init_opengl()) return -1;
	init_GLobjs();
	init_shaders();
	init_fonts();
	init_phys();
	//ControllablePlayer p(1.83f,0.52f,-0.0f,0.0f);
	//player = &p;
	player = new ControllablePlayer(1.2f, 0.52f, -0.0f, 0.0f);
	//Solid s1(.2f, 15.0f, 0.0f, -2.0f,0);
	//terrain.emplace_back(&s1);
	auto s1 = new Solid(.2f, 15.0f, 0.0f, -2.0f, 0);
	terrain.emplace_back(s1);
	//Solid s2(3.0f, .2f, -5.0f, -.5f,0);
	//terrain.emplace_back(&s2);
	auto s2 = new Solid(3.0f, .2f, -5.0f, -.5f, 0);
	terrain.emplace_back(s2);
	//Solid s3(2.0f, .2f, 1.0f, -1.0f,0);
	//terrain.emplace_back(&s3);
	auto s3 = new Solid(2.0f, .2f, 7.0f, -1.0f, 0);
	terrain.emplace_back(s3);
	//Solid s4(2.75f, .2f, -4.05f, -1.05f,45);
	//terrain.emplace_back(&s4);
	auto s4 = new Solid(2.75f, .2f, -4.05f, -1.05f, 45);
	terrain.emplace_back(s4);

	double starttime;
	while (!glfwWindowShouldClose(window))
	{
		starttime = glfwGetTime();
		///
		processInput();
		player->act();
		if (sucking) {
			glfwGetCursorPos(window, &mousex, &mousey);
			//for (auto b : testboxes) {
			//	float dx = mousex - b->GetWorldCenter().x * scale - winWid / 2.0f;
			//	float dy = -mousey + winHgt - b->GetWorldCenter().y * scale - winHgt / 2.0f;
			//	b->ApplyForceToCenter(b2Vec2(dx, dy), true);
			//}
			//for (int i = 0; i < part_sys->GetParticleCount(); ++i) {
			//	float dx = mousex - (part_sys->GetPositionBuffer()+i)->x * scale - winWid / 2.0f;
			//	float dy = -mousey + winHgt - (part_sys->GetPositionBuffer() + i)->y * scale - winHgt / 2.0f;
			//	part_sys->ParticleApplyForce(i, b2Vec2(dx, dy));
			//}
			float dx = mousex - player->getBody()->GetWorldCenter().x * scale - winWid / 2.0f;
			float dy = -mousey + winHgt - player->getBody()->GetWorldCenter().y * scale - winHgt / 2.0f;
			player->getBody()->SetLinearVelocity(b2Vec2(dx,dy));
		}
		if (flowing) {
			glfwGetCursorPos(window, &mousex, &mousey);
			for (int x = -1; x < 1; ++x) {
				for (int y = -1; y < 1; ++y) {
					b2ParticleDef pd;
					pd.flags = b2_elasticParticle;
					pd.color.Set(0, 0, 255, 255);
					pd.position.Set(((mousex-x) - winWid / 2.0f) / scale, (-(mousey-y) + winHgt / 2.0f) / scale);
					while (phys_world->IsLocked());
					int tempIndex = part_sys->CreateParticle(pd);
					part_sys->SetParticleLifetime(tempIndex, 25.0f);
				}
			}
		}
		///
		phys_world->Step(step, 6, 2); //BTW: dont use particle collisions for on hit effects. only use them for force and vfx
		//for (Thing* b : physcontactlistener.zombies) {
			//++testscore;
			//testboxes.erase(remove(testboxes.begin(), testboxes.end(), b), testboxes.end());
			//phys_world->DestroyBody(b);
			//if (b->getID() == 2) {
			//	//b=static_cast<
			//}
			//phys_world->DestroyBody(b.body)
		//}
		//physcontactlistener.zombies.clear();
		///
		render();
		///
		//cout << (glfwGetTime() - starttime) * 1000 << endl;
		while (glfwGetTime() - starttime < step);
		frametime = glfwGetTime() - starttime;
		glfwPollEvents();
	}

	glfwTerminate();
	cout << "bye" << endl;
	return 0;
}
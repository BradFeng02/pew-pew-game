#include "Solid.h"
#include <Box2D/Box2D.h>
#include "common.h"
#include <glad\glad.h>
#include <iostream>

Solid::Solid(float height, float width, float x0, float y0,float angle) {
	b2BodyDef bd;
	ref = { SOLI, this };
	bd.userData = &ref;
	bd.type = b2_staticBody;
	bd.position.Set(x0, y0);
	bd.angle=float(angle*DEGTORAD);
	Solid ground1();
	body = phys_world->CreateBody(&bd);
	//terrain.emplace_back(this);
	b2PolygonShape box;
	box.SetAsBox(width/2.0f, height/2.0f);
	body->CreateFixture(&box, 0.0f);
}

void Solid::render()
{
	glUseProgram(solidboxshader);
	glUniform3f(glGetUniformLocation(solidboxshader, "color"), 0.0f, 0.8f, 0.0f);
	glBindVertexArray(solidboxvao);
	glBindBuffer(GL_ARRAY_BUFFER, solidboxvbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, solidboxebo);
	//
	b2PolygonShape* s = (b2PolygonShape*)body->GetFixtureList()->GetShape();
	float vertices[] = {
		 body->GetWorldPoint(s->GetVertex(0)).x * scale / winWid * 2, body->GetWorldPoint(s->GetVertex(0)).y * scale / winHgt * 2,
		 body->GetWorldPoint(s->GetVertex(1)).x * scale / winWid * 2, body->GetWorldPoint(s->GetVertex(1)).y * scale / winHgt * 2,
		 body->GetWorldPoint(s->GetVertex(2)).x * scale / winWid * 2, body->GetWorldPoint(s->GetVertex(2)).y * scale / winHgt * 2,
		 body->GetWorldPoint(s->GetVertex(3)).x * scale / winWid * 2, body->GetWorldPoint(s->GetVertex(3)).y * scale / winHgt * 2
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}



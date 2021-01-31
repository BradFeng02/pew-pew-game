#include "ControllablePlayer.h"
#include <Box2D/Box2D.h>
#include <glad/glad.h>
#include "common.h"
#include <iostream>
#include "physraycallback.h"
using namespace std;

ControllablePlayer::ControllablePlayer(float height, float width, float x0, float y0)
	:Player(height,width,x0,y0)
{
	maxspeed = 5.0f;
	body->GetFixtureList()->SetFriction(1.0f);
}

void ControllablePlayer::render()
{
	glUseProgram(solidboxshader);
	glUniform3f(glGetUniformLocation(solidboxshader, "color"), 0.8f, 0.8f, 0.0f);
	glBindVertexArray(capsulevao);
	glBindBuffer(GL_ARRAY_BUFFER, capsulevbo);

	GLfloat* coords = new GLfloat[4 * CAPSULE_PREC+4];
	int i = 0;

	b2PolygonShape* s = (b2PolygonShape*)body->GetFixtureList()->GetNext()->GetNext()->GetShape();
	//cout << body->GetWorldPoint(s->GetVertex(2)).y << endl;
	//coords where cy and cy start at 0 rads
	float cx = s->GetVertex(2).x;
	float cy = 0;
	cout << cx << endl;
	//diff btwn circle coords and world coords
	float wdx = body->GetWorldPoint(s->GetVertex(2)).x-cx;
	float wdy = body->GetWorldPoint(s->GetVertex(2)).y-cy;
	float ogx = cx;
	float ogy = cy;

	coords[i++] = phys2glX(cx+wdx);
	coords[i++] = phys2glY(cy+wdy);

	coords[i++] = phys2glX(cx+wdx);
	coords[i++] = phys2glY(cy+wdy-2*boxhalfhgt);

	for (int a = 1; a < CAPSULE_PREC; ++a) {
		float newcx = cosInc * cx - sinInc * cy;
		cy = sinInc * cx + cosInc * cy;
		cx = newcx;

		coords[i++] = phys2glX(cx+wdx);
		coords[i++] = phys2glY(cy+wdy);

		coords[i++] = phys2glX(cx+wdx);
		coords[i++] = phys2glY(-cy+wdy-2*boxhalfhgt);
	}

	coords[i++] = phys2glX(ogx+wdx-wid);
	coords[i++] = phys2glY(ogy+wdy);

	coords[i++] = phys2glX(ogx + wdx - wid);
	coords[i++] = phys2glY(ogy + wdy - 2*boxhalfhgt);

	glBufferData(GL_ARRAY_BUFFER, (4*CAPSULE_PREC+4)*sizeof(GLfloat),coords, GL_DYNAMIC_DRAW);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * CAPSULE_PREC);

	delete[] coords;

	//glUseProgram(solidboxshader);
	//glUniform3f(glGetUniformLocation(solidboxshader, "color"), 0.8f, 0.8f, 0.0f);
	//glBindVertexArray(solidboxvao);
	//glBindBuffer(GL_ARRAY_BUFFER, solidboxvbo);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, solidboxebo);
	////
	//b2PolygonShape* s = (b2PolygonShape*)body->GetFixtureList()->GetShape();
	//float vertices[] = {
	//	 phys2glX(body->GetWorldPoint(s->GetVertex(0)).x), phys2glY(body->GetWorldPoint(s->GetVertex(0)).y),
	//	 phys2glX(body->GetWorldPoint(s->GetVertex(1)).x), phys2glY(body->GetWorldPoint(s->GetVertex(1)).y),
	//	 phys2glX(body->GetWorldPoint(s->GetVertex(2)).x), phys2glY(body->GetWorldPoint(s->GetVertex(2)).y),
	//	 phys2glX(body->GetWorldPoint(s->GetVertex(3)).x), phys2glY(body->GetWorldPoint(s->GetVertex(3)).y)
	//};
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	/////////
	/////////
	glBindVertexArray(solidboxvao);
	glBindBuffer(GL_ARRAY_BUFFER, solidboxvbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, solidboxebo);
	glUniform3f(glGetUniformLocation(solidboxshader, "color"), 1.0f, 0.0f, 1.0f);
	float v[] = {
		phys2glX(ray2.x) + .001f, phys2glY(ray2.y),
		phys2glX(ray2.x) - .001f, phys2glY(ray2.y),
		phys2glX(ray1.x) - .001f, phys2glY(ray1.y),
		phys2glX(ray1.x) + .001f, phys2glY(ray1.y)
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(v), &v, GL_DYNAMIC_DRAW);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void ControllablePlayer::act()
{
	ray1 = body->GetWorldPoint(b2Vec2(0, -hgt / 2.0f));
	ray2 = body->GetWorldPoint(b2Vec2(0, -hgt / 2.0f - 0.1f));
	//b2RayCastInput rayin;
	//rayin.p1 = body->GetWorldPoint(b2Vec2(0, -hgt / 2.0f));
	//rayin.p2 = body->GetWorldPoint(b2Vec2(0, -hgt / 2.0f)) - 0.1f;
	raycallback cb;
	phys_world->RayCast(&cb, ray1, ray2);
	//cout << body->GetLinearVelocity().x << endl;
	b2PolygonShape* s = (b2PolygonShape*)body->GetFixtureList()->GetShape();
	//cout << body->GetWorldPoint(s->GetVertex(1)).y << endl;

	if (movingright && !movingleft) {
		if (body->GetLinearVelocity().x < 0) stopmoving();
		++tright;
		float fx = min(5.0 * body->GetMass() * (5 + tright), (maxspeed - body->GetLinearVelocity().x) / step * body->GetMass());
		body->ApplyForceToCenter(b2Vec2(fx, 0), true);
	}
	if (movingleft && !movingright) {
		if (body->GetLinearVelocity().x > 0) stopmoving();
		++tleft;
		float fx = max(-5.0 * body->GetMass() * (5 + tleft), (-maxspeed - body->GetLinearVelocity().x) / step * body->GetMass());
		body->ApplyForceToCenter(b2Vec2(fx, 0), true);
	}
	if (!(movingright ^ movingleft)) {
		stopmoving();
	}
}

void ControllablePlayer::right_pressed()
{
	movingright = true;
	tright = 0;
}

void ControllablePlayer::left_pressed()
{
	movingleft = true;
	tleft = 0;
}

void ControllablePlayer::jump_pressed()
{
}

void ControllablePlayer::right_released()
{
	movingright = false;
}

void ControllablePlayer::left_released()
{
	movingleft = false;
}

void ControllablePlayer::jump_released()
{
}

void ControllablePlayer::collideWith(Solid* t)
{
	cout << "controllable player C/W solid" << endl;
}

void ControllablePlayer::stopmoving()
{
	float fx = min(maxspeed * 15.0 * body->GetMass(), (abs(body->GetLinearVelocity().x)) / step * body->GetMass());
	body->ApplyForceToCenter(b2Vec2(body->GetLinearVelocity().x < 0 ? fx : -fx, 0), true);
}

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
	glBindVertexArray(solidboxvao);
	glBindBuffer(GL_ARRAY_BUFFER, solidboxvbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, solidboxebo);
	//
	b2PolygonShape* s = (b2PolygonShape*)body->GetFixtureList()->GetShape();
	float vertices[] = {
		 phys2glX(body->GetWorldPoint(s->GetVertex(0)).x), phys2glY(body->GetWorldPoint(s->GetVertex(0)).y),
		 phys2glX(body->GetWorldPoint(s->GetVertex(1)).x), phys2glY(body->GetWorldPoint(s->GetVertex(1)).y),
		 phys2glX(body->GetWorldPoint(s->GetVertex(2)).x), phys2glY(body->GetWorldPoint(s->GetVertex(2)).y),
		 phys2glX(body->GetWorldPoint(s->GetVertex(3)).x), phys2glY(body->GetWorldPoint(s->GetVertex(3)).y)
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	/////////
	/////////
	glUniform3f(glGetUniformLocation(solidboxshader, "color"), 1.0f, 0.0f, 1.0f);
	float v[] = {
		phys2glX(ray2.x) + .001f, phys2glY(ray2.y),
		phys2glX(ray2.x) - .001f, phys2glY(ray2.y),
		phys2glX(ray1.x) - .001f, phys2glY(ray1.y),
		phys2glX(ray1.x) + .001f, phys2glY(ray1.y)
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_DYNAMIC_DRAW);
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
	cout << "CONTRollaable player class" << endl;
}

void ControllablePlayer::stopmoving()
{
	float fx = min(maxspeed * 15.0 * body->GetMass(), (abs(body->GetLinearVelocity().x)) / step * body->GetMass());
	body->ApplyForceToCenter(b2Vec2(body->GetLinearVelocity().x < 0 ? fx : -fx, 0), true);
}

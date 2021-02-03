#include "Player.h"
#include <Box2D/Box2D.h>
#include "common.h"
#include <iostream>

Player::Player(float height, float width,float x0, float y0)
{
	//id = PLAY;
	hgt = height;
	wid = width;
	boxhalfhgt = (height - width) / 2.0f;
	//x = x0;
	//y = y0;

	b2BodyDef bd;
	ref = { PLAY, this };
	bd.userData = &ref;
	bd.type = b2_dynamicBody;
	bd.bullet = true;
	bd.position.Set(x0, y0);
	bd.fixedRotation = true;
	body = phys_world->CreateBody(&bd);

	//capsule
	float fric = 1.0f; //should depend on terrain value??
	//box
	b2PolygonShape box;
	box.SetAsBox(width / 2.0f, boxhalfhgt);
	b2FixtureDef boxfd;
	boxfd.shape = &box;
	boxfd.density = 90.0f;
	boxfd.friction = fric;
	body->CreateFixture(&boxfd);
	//top circle
	b2CircleShape top;
	top.m_radius = width / 2.0f;
	top.m_p.Set(0.0f, boxhalfhgt);
	b2FixtureDef topfd;
	topfd.shape = &top;
	topfd.density = 45.0f;
	topfd.friction = fric;
	body->CreateFixture(&topfd);
	//bot circle
	b2CircleShape bot;
	bot.m_radius = width / 2.0f;
	bot.m_p.Set(0.0f, -boxhalfhgt);
	b2FixtureDef botfd;
	botfd.shape = &bot;
	botfd.density = 45.0f;
	botfd.friction = fric;
	body->CreateFixture(&botfd);
}

void Player::render()
{
}

b2Body* Player::getBody()
{
	return body;
}

void Player::collideWith(Solid* t)
{
	std::cout << "player class" << std::endl;
}

void Player::collideWith(Player* t)
{
}

#include "Player.h"
#include <Box2D/Box2D.h>
#include "common.h"
#include <iostream>

Player::Player(float height, float width,float x0, float y0)
{
	//id = PLAY;
	hgt = height;
	wid = width;
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
	b2PolygonShape box;
	box.SetAsBox(width/2.0f, height/2.0f);
	b2FixtureDef fd;
	fd.shape = &box;
	fd.density = 1000.0;
	fd.friction = 0.3;
	body->CreateFixture(&fd);
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

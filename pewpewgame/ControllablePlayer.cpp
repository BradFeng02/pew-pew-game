#include "ControllablePlayer.h"
#include <Box2D/Box2D.h>
#include <glad/glad.h>
#include "common.h"
#include <iostream>
#include "controllableplayerraycallback.h"
using namespace std;

ControllablePlayer::ControllablePlayer(float height, float width, float x0, float y0)
	:Player(height, width, x0, y0)
{
	movingleft = false;
	movingright = false;

	rays = new b2Vec2[raycnt * 4 + 2];

	dirx = 0;
	diry = 0;

	float r = width / 2.0f;

	//rays ordered from left to right
	for (int i = 0; i < raycnt * 2; i += 2) {
		double a = (1 - (i / 2) / static_cast<double>(raycnt))* PI / 4.0;

		rays[i] = b2Vec2(-sin(a) * r, -boxhalfhgt - cos(a) * r);
		rays[i + 1] = b2Vec2(-sin(a) * (r + raylen), -boxhalfhgt - cos(a) * (r + raylen));

		rays[raycnt * 4 - i] = b2Vec2(sin(a) * r, -boxhalfhgt - cos(a) * r);
		rays[raycnt * 4 - i + 1] = b2Vec2(sin(a) * (r + raylen), -boxhalfhgt - cos(a) * (r + raylen));
	}
	rays[raycnt * 2] = b2Vec2(0, -hgt / 2.0f);
	rays[raycnt * 2 + 1] = b2Vec2(0, -hgt / 2.0f - raylen);
}

void ControllablePlayer::render()
{
	glUseProgram(solidboxshader);
	glUniform3f(glGetUniformLocation(solidboxshader, "color"), 0.8f, 0.8f, 0.0f);
	glBindVertexArray(capsulevao);
	glBindBuffer(GL_ARRAY_BUFFER, capsulevbo);

	GLfloat* coords = new GLfloat[4 * CAPSULE_PREC + 4];
	int i = 0;

	b2PolygonShape* s = (b2PolygonShape*)body->GetFixtureList()->GetNext()->GetNext()->GetShape();
	//cout << body->GetWorldPoint(s->GetVertex(2)).y << endl;
	//coords where cy and cy start at 0 rads
	float cx = s->GetVertex(2).x;
	float cy = 0;
	//diff btwn circle coords and world coords
	float wdx = body->GetWorldPoint(s->GetVertex(2)).x - cx;
	float wdy = body->GetWorldPoint(s->GetVertex(2)).y - cy;
	float ogx = cx;
	float ogy = cy;

	coords[i++] = phys2glX(cx + wdx);
	coords[i++] = phys2glY(cy + wdy);

	coords[i++] = phys2glX(cx + wdx);
	coords[i++] = phys2glY(cy + wdy - 2 * boxhalfhgt);

	for (int a = 1; a < CAPSULE_PREC; ++a) {
		float newcx = cosInc * cx - sinInc * cy;
		cy = sinInc * cx + cosInc * cy;
		cx = newcx;

		coords[i++] = phys2glX(cx + wdx);
		coords[i++] = phys2glY(cy + wdy);

		coords[i++] = phys2glX(cx + wdx);
		coords[i++] = phys2glY(-cy + wdy - 2 * boxhalfhgt);
	}

	coords[i++] = phys2glX(ogx + wdx - wid);
	coords[i++] = phys2glY(ogy + wdy);

	coords[i++] = phys2glX(ogx + wdx - wid);
	coords[i++] = phys2glY(ogy + wdy - 2 * boxhalfhgt);

	glBufferData(GL_ARRAY_BUFFER, (4 * CAPSULE_PREC + 4) * sizeof(GLfloat), coords, GL_DYNAMIC_DRAW);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * CAPSULE_PREC);

	delete[] coords;

	/////////

	drawRays();

}

void ControllablePlayer::drawRays() {
	glUseProgram(solidboxshader);
	glBindVertexArray(solidboxvao);
	glBindBuffer(GL_ARRAY_BUFFER, solidboxvbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, solidboxebo);
	glUniform3f(glGetUniformLocation(solidboxshader, "color"), 1.0f, 0.0f, 1.0f);
	//rays
	for (int i = 0; i <= raycnt * 4; i += 2) {
		b2Vec2 ray1 = body->GetWorldPoint(rays[i]);
		b2Vec2 ray2 = body->GetWorldPoint(rays[i + 1]);

		float v[] = {
		phys2glX(ray2.x) + .001f, phys2glY(ray2.y),
		phys2glX(ray2.x) - .001f, phys2glY(ray2.y),
		phys2glX(ray1.x) - .001f, phys2glY(ray1.y),
		phys2glX(ray1.x) + .001f, phys2glY(ray1.y)
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(v), &v, GL_DYNAMIC_DRAW);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	//direction
	b2Vec2 dirvec1 = body->GetWorldPoint(b2Vec2(0, 0));
	b2Vec2 dirvec2 = body->GetWorldPoint(b2Vec2(dirx, diry));
	float v[] = {
		phys2glX(dirvec2.x) , phys2glY(dirvec2.y) + .002f,
		phys2glX(dirvec2.x) , phys2glY(dirvec2.y) - .002f,
		phys2glX(dirvec1.x) , phys2glY(dirvec1.y) - .002f,
		phys2glX(dirvec1.x) , phys2glY(dirvec1.y) + .002f
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(v), &v, GL_DYNAMIC_DRAW);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

//checks grounded as well
void ControllablePlayer::getDirLeft(raycallback* cb) {
	float minfrac = 2;
	b2Vec2 minnorm;
	//forward rays
	for (int i = 0; i < 2 * raycnt; i += 2) {
		cb->clear();
		phys_world->RayCast(cb, body->GetWorldPoint(rays[i]), body->GetWorldPoint(rays[i + 1]));
		float newfrac = cb->fraction();
		if (newfrac < minfrac) {
			minfrac = newfrac;
			minnorm = cb->normal();
		}
	}
	//if nothing, other rays
	if (minfrac == 2) {
		for (int i = 0; i <= 2 * raycnt; i += 2) {
			cb->clear();
			phys_world->RayCast(cb, body->GetWorldPoint(rays[4 * raycnt - i]), body->GetWorldPoint(rays[4 * raycnt - i + 1]));
			float newfrac = cb->fraction();
			if (newfrac < minfrac) {
				minfrac = newfrac;
				minnorm = cb->normal();
			}
		}
	}

	if (minfrac < 2) {
		//force direction multiplier
		dirx = -minnorm.y;
		diry = minnorm.x;
		grounded = true;
	}
	else {
		//if still nothing, in air;
		dirx = -1;
		diry = 0;
		grounded = false;
	}
}

//checks grounded as well
void ControllablePlayer::getDirRight(raycallback* cb) {
	float minfrac = 2;
	b2Vec2 minnorm;
	//forward rays
	for (int i = 0; i < 2 * raycnt; i += 2) {
		cb->clear();
		phys_world->RayCast(cb, body->GetWorldPoint(rays[4 * raycnt - i]), body->GetWorldPoint(rays[4 * raycnt - i + 1]));
		float newfrac = cb->fraction();
		if (newfrac < minfrac) {
			minfrac = newfrac;
			minnorm = cb->normal();
		}
	}
	//if nothing, other rays
	if (minfrac == 2) {
		for (int i = 0; i <= 2 * raycnt; i += 2) {
			cb->clear();
			phys_world->RayCast(cb, body->GetWorldPoint(rays[i]), body->GetWorldPoint(rays[i + 1]));
			float newfrac = cb->fraction();
			if (newfrac < minfrac) {
				minfrac = newfrac;
				minnorm = cb->normal();
			}
		}
	}

	if (minfrac < 2) {
		//force direction multiplier
		dirx = minnorm.y;
		diry = -minnorm.x;
		grounded = true;
	}
	else {
		//if still nothing, in air;
		dirx = 1;
		diry = 0;
		grounded = false;
	}
}

void ControllablePlayer::checkGrounded(raycallback* cb) {
	grounded = false;
	for (int i = 0; i <= 4 * raycnt; i += 2) {
		cb->clear();
		phys_world->RayCast(cb, body->GetWorldPoint(rays[i]), body->GetWorldPoint(rays[i + 1]));
		if (cb->fraction() < 2) {
			grounded = true;
			break;
		}
	}
}

void ControllablePlayer::step()
{
	///PROCESS: (TODO)
	// 1) check raycnt rays in forward direction. eg if move left, check left rays not incl center
	// 2) if found, choose closest to get move angle, skip (3)
	// 3) if not found, get angle from closest of center + other side
	// 4) if found, apply impulse once then force after in angle of terrain.
	// 5) else, in air

	//float force = 50 * body->GetMass();
	b2Vec2 vnormed = body->GetLinearVelocity();
	float speed = vnormed.Normalize();
	float mass = body->GetMass();
	raycallback cb;

	//if both or none pressed
	if (!(movingleft ^ movingright)) {
		checkGrounded(&cb);
		//maxaccel = grounded ? accelhi : accello;
		tleft = 0;
		tright = 0;

		//stop moving
		if (grounded) {
			//jump up
			if (jumping) {
				if (tjump == 0) {
					body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x, jumppwr));
					++tjump;
				}
			}
			//stop moving
			else {
				float d = pow(speed, 3) / pow(maxspeed, 2);
				float force = clamp(min(maxspeed + d + turnrate, maxspeed * speed) * startrate, 0.0f, maxaccel) * mass;
				body->ApplyForceToCenter(b2Vec2(force * -vnormed.x, force * -vnormed.y), true);
			}
		}
		else {
			float d = pow(speed, 3) / pow(maxspeed, 2);
			float force = clamp(min(maxspeed + d + turnrate, maxspeed * speed) * startrate, 0.0f, maxaccel) * mass;
			body->ApplyForceToCenter(b2Vec2(force * -vnormed.x, 0), true);
		}

		if (jumping&&tjump < 15) {
			if (tjump>3) body->ApplyForceToCenter(b2Vec2(0, floatpwr * mass), true);
			++tjump;
		}
	}
	else if (movingleft) {
		getDirLeft(&cb);
		//maxaccel = grounded ? accelhi : accello;
		if (tleft < 60)++tleft;

		float d = pow(speed, 3) / pow(maxspeed, 2);
		if (vnormed.x > 0)d = -d - turnrate;
		float force = clamp((maxspeed - d) * (startrate + tleft / accelrate), 0.0f, maxaccel) * mass * (grounded ? 1 : airmult);
		body->ApplyForceToCenter(b2Vec2(force * dirx, force * diry), true);

		if (jumping) {
			if (tjump == 0) {
				body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x, jumppwr));
				++tjump;
			}else if (tjump < 15) {
				if (tjump > 3) body->ApplyForceToCenter(b2Vec2(0, floatpwr * mass), true);
				++tjump;
			}
		}
	}
	else if (movingright) {
		getDirRight(&cb);
		//maxaccel = grounded ? accelhi : accello;
		if (tright < 60)++tright;

		float d = pow(speed, 3) / pow(maxspeed, 2);
		if (vnormed.x < 0)d = -d - turnrate;
		float force = clamp((maxspeed - d) * (startrate + tright / accelrate), 0.0f, maxaccel) * mass * (grounded ? 1 : airmult);
		body->ApplyForceToCenter(b2Vec2(force * dirx, force * diry), true);

		if (jumping) {
			if (tjump == 0) {
				body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x, jumppwr));
				++tjump;
			}
			else if (tjump < 15) {
				if (tjump > 3) body->ApplyForceToCenter(b2Vec2(0, floatpwr * mass), true);
				++tjump;
			}
		}
	}

	//b2Vec2 vel = body->GetLinearVelocity();
	//float slowamt = 10.0f * body->GetMass();
	//if (vel.Normalize() > maxspeed) {
	//	body->ApplyForceToCenter(b2Vec2(-slowamt * vel.x, -slowamt * vel.y), true);
	//}


	/*
	b2PolygonShape* s = (b2PolygonShape*)body->GetFixtureList()->GetShape();
	//cout << body->GetWorldPoint(s->GetVertex(1)).y << endl;

	if (movingright && !movingleft) {
		if (body->GetLinearVelocity().x < 0) stopmoving();
		++tright;
		float fx = min(5.0 * body->GetMass() * (5 + tright), (maxspeed - body->GetLinearVelocity().x) / P_STEP * body->GetMass());
		body->ApplyForceToCenter(b2Vec2(fx, 0), true);
	}
	if (movingleft && !movingright) {
		if (body->GetLinearVelocity().x > 0) stopmoving();
		++tleft;
		float fx = max(-5.0 * body->GetMass() * (5 + tleft), (-maxspeed - body->GetLinearVelocity().x) / P_STEP * body->GetMass());
		body->ApplyForceToCenter(b2Vec2(fx, 0), true);
	}
	if (!(movingright ^ movingleft)) {
		stopmoving();
	}
	*/


}

bool ControllablePlayer::isGrounded() {
	return grounded;
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
	jumping = true;
	tjump = 0;
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
	jumping = false;
}

void ControllablePlayer::collideWith(Solid* t)
{
	cout << "controllable player C/W solid" << endl;
}

void ControllablePlayer::stopmoving()
{
	float fx = min(maxspeed * 15.0 * body->GetMass(), (abs(body->GetLinearVelocity().x)) / P_STEP * body->GetMass());
	body->ApplyForceToCenter(b2Vec2(body->GetLinearVelocity().x < 0 ? fx : -fx, 0), true);
}

#pragma once
#include <Box2D/Box2D.h>
#include <iostream>
using namespace std;

class raycallback : public b2RayCastCallback
{
private:
	float32 latestFraction;
	b2Vec2 latestNormal;
public:
	float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction) {
		//cout << "rayed ("<<fraction<<") ("<<normal.x<<", " <<normal.y<<")" << endl;
		latestFraction = fraction;
		latestNormal = normal;
		return fraction;
	}

	float32 fraction() {
		return latestFraction;
	}

	b2Vec2 normal() {
		return latestNormal;
	}

	void clear() {
		latestFraction = 2;
	}
};
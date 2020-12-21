#pragma once
#include <Box2D/Box2D.h>
#include <iostream>
using namespace std;

class raycallback : public b2RayCastCallback
{
public:
	float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction) {
		cout << "rayed" << endl;
		return 0;
	}
};
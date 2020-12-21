#pragma once
#include <Box2D/Box2D.h>
#include <iostream>
#include <vector>
#include "Thing.h"
#include "Solid.h"
#include "Player.h"
using namespace std;

class contactlistener : public b2ContactListener
{
public:
   //vector<Thing*> zombies;
    //vector<unsigned int> particlestoprocess;

    void BeginContact(b2Contact* contact) {
        auto a = static_cast<ThingRef*>(contact->GetFixtureA()->GetBody()->GetUserData());
        auto b = static_cast<ThingRef*>(contact->GetFixtureB()->GetBody()->GetUserData());

        switch (a->id) {
        case SOLI:
            switch (b->id) {
            case SOLI:
                //cout << "soli x soli" << endl;
                static_cast<Solid*>(a->t)->collideWith(static_cast<Solid*>(b->t));
                return;
            case PLAY:
                //cout << "soli x play" << endl;
                static_cast<Solid*>(a->t)->collideWith(static_cast<Player*>(b->t));
                return;
            }
            return;
        case PLAY:
            switch (b->id) {
            case SOLI:
                //cout << "play x soli" << endl;
                static_cast<Player*>(a->t)->collideWith(static_cast<Solid*>(b->t));
                return;
            case PLAY:
                //cout << "play x play" << endl;
                static_cast<Player*>(a->t)->collideWith(static_cast<Player*>(b->t));
                return;
            }
            return;
        }
    }

    void EndContact(b2Contact* contact) {

        ////check if fixture A was a ball
        //void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
        //if (bodyUserData)
        //    static_cast<Ball*>(bodyUserData)->endContact();

        ////check if fixture B was a ball
        //bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
        //if (bodyUserData)
        //    static_cast<Ball*>(bodyUserData)->endContact();

    }

    /*
    void BeginContact(b2ParticleSystem* particleSystem, b2ParticleBodyContact* particleBodyContact) {
        //B2_NOT_USED(particleSystem);
        //B2_NOT_USED(particleBodyContact);
        b2Body* b=particleBodyContact->body;
        unsigned int p = particleBodyContact->index;
        cout << (int)b->GetUserData() << endl;
        if ((int)b->GetUserData() == 1 ){//&& (int)(particleSystem->GetUserDataBuffer()+p)==2) {
            particleSystem->SetParticleFlags(p,particleSystem->GetParticleFlags(p)| b2_zombieParticle);
            particlestoprocess.emplace_back(p);
        }
    }
    */ 
    //DOES NOT WORK. USE BODIES FOR SHOTS WITH EFFECT. PARTICLE FOR FORCE ONLY

private:

};
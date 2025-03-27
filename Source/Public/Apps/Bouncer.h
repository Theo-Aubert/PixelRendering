#pragma once
#include "../AppManager.h"

class Bouncer : public PixelApp
{
public:

    struct BounceOject
    {
        olc::vf2d position;
        olc::vf2d dPosition;
        olc::Pixel color;

        int nbBounces = 10;
        float size = 15.f;
        float bounciness = 0.7f; //[0,1]
		
    };

    Bouncer()
    {
        // Name your application
        sAppName = "Bouncer";
    }

public:
    bool OnUserCreate() override
    {
        // Called once at the start, so create things here
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        Clear(olc::BLACK);
        if (GetMouse(olc::Mouse::LEFT).bHeld)
        {
            bouncers.push_back(BounceOject(GetMousePos(), olc::vf2d(rand() % 300 - 150.f, 0.f), olc::Pixel(rand() % 256, rand() % 256, rand() % 256)));
        }

        for (auto it = bouncers.begin(); it < bouncers.end(); it++)
        {
            if ((*it).nbBounces < 0)
            {
                continue;
            }
            DrawCircle(olc::vi2d((*it).position), int32_t((*it).size), (*it).color);

            UpdatePhysics((*it), fElapsedTime);
			
        }

        return true;
    }

    void UpdatePhysics(BounceOject& bouncer, float fElapsedTime)
    {
        bouncer.dPosition.y += gravity;
        bouncer.position += bouncer.dPosition * fElapsedTime;

        if (bouncer.position.y + bouncer.size > ScreenHeight())
        {
            bouncer.dPosition.y *= -bouncer.bounciness;
            bouncer.position.y = ScreenHeight() - bouncer.size;
            bouncer.nbBounces--;
        }

        if (bouncer.position.y - bouncer.size < 0)
        {
            bouncer.dPosition.y *= -bouncer.bounciness;
            bouncer.position.y = bouncer.size;
            bouncer.nbBounces--;
        }

        if (bouncer.position.x - bouncer.size < 0)
        {
            bouncer.dPosition.x *= -bouncer.bounciness;
            bouncer.position.x = bouncer.size;
            bouncer.nbBounces--;
        }

        if (bouncer.position.x + bouncer.size > ScreenWidth())
        {
            bouncer.dPosition.x *= -bouncer.bounciness;
            bouncer.position.x = ScreenWidth() - bouncer.size;
            bouncer.nbBounces--;
        }
    }

    float gravity = 0.2f;
    std::vector<BounceOject> bouncers;
};
REGISTER_CLASS(Bouncer)

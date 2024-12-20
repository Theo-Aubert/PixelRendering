#pragma once

#include "../Externals/olcPixelGameEngine.h"
#include "Constants.h"

class RayTracer : public olc::PixelGameEngine
{
public:
    RayTracer()
    {
        // Name your application
        sAppName = "RayTracer";
    }

public:
    bool OnUserCreate() override;

    bool OnUserUpdate(float fElapsedTime) override;

private:

    olc::Sprite* pDrawTarget = nullptr;

    int iCurrentRow = 0;
    int iRowNum = 0;
    
};

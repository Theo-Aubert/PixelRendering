#pragma once

#include "PixelApp.h"
#include "AppManager.h"
#include "Constants.h"

class RayTracer : public PixelApp
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
REGISTER_CLASS(RayTracer)

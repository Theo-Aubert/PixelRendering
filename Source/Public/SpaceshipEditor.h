#pragma once
#include "PixelApp.h"
#include "AppManager.h"
#include "Constants.h"

class SpaceshipEditor :  public PixelApp
{
public:
    SpaceshipEditor()
    {
        // Name your application
        sAppName = "SpaceshipEditor";
    }

public:
    bool OnUserCreate() override;

    bool OnUserUpdate(float fElapsedTime) override;

private:
    
    
};
REGISTER_CLASS(SpaceshipEditor)

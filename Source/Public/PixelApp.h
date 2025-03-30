#pragma once
#include "../Externals/olcPixelGameEngine.h"

class PixelApp : public olc::PixelGameEngine
{
public:
    
    virtual bool OnUserDestroy() override;
    
    void NotifyExit() const ;
    
};
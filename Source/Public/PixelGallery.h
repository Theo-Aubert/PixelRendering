#pragma once
#include "AppManager.h"

class PixelGallery : public PixelApp
{
public:

    PixelGallery()
    {
        sAppName = "Pixel Gallery";
    };
    
    virtual bool OnUserCreate() override;
    virtual bool OnUserUpdate(float fElapsedTime) override;
    

    //virtual std::string GetName() const override { return "PixelGallery";};
};
REGISTER_CLASS(PixelGallery)



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

class DummyA : public PixelApp
{
public:
    virtual bool OnUserCreate() override {return true;}
    virtual bool OnUserUpdate(float fElapsedTime) override;
    
    //virtual std::string GetName() const override { return "DummyA";};
};

REGISTER_CLASS(DummyA)

class DummyB : public PixelApp
{
public:

    virtual bool OnUserCreate() override {return true;}
    virtual bool OnUserUpdate(float fElapsedTime) override;
    //virtual std::string GetName() const override { return "DummyB";};
    
};

REGISTER_CLASS(DummyB)



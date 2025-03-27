#include "../Public/PixelGallery.h"

bool PixelGallery::OnUserCreate()
{
    return true;
}

bool PixelGallery::OnUserUpdate(float fElapsedTime)
{
    //Quit app
    if (GetKey(olc::ESCAPE).bReleased) return false;

    if(GetKey(olc::LEFT).bReleased)
    {
        AppManager::SignalNextApp("DummyA");
        return false;
    }
    if(GetKey(olc::RIGHT).bReleased)
    {
        AppManager::SignalNextApp("DummyB");
        return false;
    }
    
    Clear(olc::BLACK);

    int idx = 0;
    
    for(auto const& app : AppManager::GetRegisteredApps())
    {
        DrawRect(olc::vi2d(25,25 + idx * 90), olc::vi2d(150, 75));
        DrawString(olc::vi2d(45,60 + idx * 90), app);
        idx ++;
    }

    return true;
}

bool DummyA::OnUserUpdate(float fElapsedTime)
{
    if (GetKey(olc::ESCAPE).bReleased) return false;
    
    DrawRect(75,75, 150, 150, olc::GREEN);
    return true;
}

bool DummyB::OnUserUpdate(float fElapsedTime)
{
    if (GetKey(olc::ESCAPE).bReleased) return false;
    
    DrawRect(75,75, 150, 150, olc::RED);
    return true;
}
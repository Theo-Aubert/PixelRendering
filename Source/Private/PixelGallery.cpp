#include "../Public/PixelGallery.h"

bool PixelGallery::OnUserCreate()
{
    return true;
}

bool PixelGallery::OnUserUpdate(float fElapsedTime)
{
    //Quit app
    if (GetKey(olc::ESCAPE).bReleased) return false;

    olc::vi2d mousePos = GetMousePos();
    
    Clear(olc::BLACK);

    int idx = 0;

   size_t iNUmApps = AppManager::GetRegisteredApps().size();

    int iNumCols = ScreenWidth() / 200;
    int iNumRows = ScreenHeight() / 100;

    int mouseX = mousePos.x / 200;
    int mouseY = mousePos.y / 100;

    int xidx = 0;
    int yidx = 0;

    std::string sSelectedApp;
    
    for(auto const& app : AppManager::GetRegisteredApps())
    {
        if(app == DefaultAppName)
            continue;
        
        if(xidx == mouseX && yidx == mouseY)
        {
            FillRect(xidx * 200 + 25, yidx * 100 + 15, 150, 70);
            DrawString(xidx * 200 +35,yidx * 100 + 45, app, olc::BLACK);

            if(GetMouse(0).bReleased) sSelectedApp = app;
        }
        else
        {
            DrawRect(xidx * 200 + 25, yidx * 100 + 15, 150, 70);
            DrawString(xidx * 200 +35,yidx * 100 + 45, app);
        }

        yidx = ++yidx % iNumRows;
        if(yidx == 0)
            xidx++;
           
    }

    if(!sSelectedApp.empty())
    {
        AppManager::SignalNextApp(sSelectedApp);
        return false;
    }
        

    return true;
}
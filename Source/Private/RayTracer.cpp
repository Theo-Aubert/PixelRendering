#include "../Public/RayTracer.h"

#include <strstream>

bool RayTracer::OnUserCreate()
{
    pDrawTarget = new olc::Sprite(ScreenWidth(), ScreenHeight());

    SetDrawTarget(pDrawTarget);
    Clear(olc::BLACK);

    for (int j = 0; j < pDrawTarget->height; j++) {
        
    }
    SetDrawTarget(nullptr);

    iRowNum = pDrawTarget->height;
    
    return true;
}

bool RayTracer::OnUserUpdate(float fElapsedTime)
{
    //Quit app
    if (GetKey(olc::ESCAPE).bReleased) return false;

    if(iCurrentRow < iRowNum)
    {
        
        for (int i = 0; i < pDrawTarget->width; i++) {
            auto r = double(i) / (pDrawTarget->width-1);
            auto g = double(iCurrentRow) / (pDrawTarget->width-1);
            auto b = 0.0;

            int ir = int(255.999 * r);
            int ig = int(255.999 * g);
            int ib = int(255.999 * b);
                
            pDrawTarget->SetPixel(i, iCurrentRow, olc::Pixel(ir, ig, ib));
        }
    }
    
    DrawSprite(0, 0, pDrawTarget);

    if(iCurrentRow < iRowNum)
    {
        std::ostringstream stream;
        stream << "Row : " << iCurrentRow <<" / "<<iRowNum;
        std::cout << stream.str() <<'\n';
        DrawString(olc::vi2d(35,25), stream.str(),olc::WHITE, 4);
        DrawString(olc::vi2d(35,35), stream.str(),olc::BLACK, 3);
        

        iCurrentRow ++;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    
    
    return true;
}

#include "../Public/MidPoint.h"

bool MidPoint::OnUserCreate()
{

    vGridSize = olc::vi2d(ScreenWidth() / iTileSize, ScreenHeight() / iTileSize);

    vCircleCenter = olc::vi2d(ScreenWidth() / 2, ScreenHeight() / 2);

    return true;
}

bool MidPoint::OnUserUpdate(float fElapsedTime)
{
    if (GetKey(olc::ESCAPE).bReleased)
    {
        return false;
    }

    if (GetMouse(olc::Mouse::LEFT).bHeld)
    {
        vCircleCenter = GetMousePos();
    }

    if (GetKey(olc::UP).bHeld)
    {
        radius += 1;
    }
    if (GetKey(olc::DOWN).bHeld)
    {
        radius -= 1;
    }
    radius = std::clamp(radius, 0, ScreenHeight() * 2);

    Clear(olc::BLACK);

    std::vector<olc::vi2d> arrTiles = MidPointAlgo(vCircleCenter, radius);
    DrawHovered(arrTiles);
    

    DrawGrid();

    DrawCircle(vCircleCenter, radius, olc::RED);
    
    

    return true;
}

void MidPoint::DrawGrid()
{

    for (int x = 0; x < vGridSize.x; x++)
    {
        DrawLine(olc::vi2d(x * iTileSize, 0), olc::vi2d(x * iTileSize, vGridSize.y * iTileSize));
    }
    DrawLine(olc::vi2d(vGridSize.x * iTileSize, 0), olc::vi2d(vGridSize.x * iTileSize, vGridSize.y * iTileSize));

    for (int y = 0; y < vGridSize.y; y++)
    {
        DrawLine(olc::vi2d(0, y *iTileSize), olc::vi2d(vGridSize.x * iTileSize, y * iTileSize));
    }
    DrawLine(olc::vi2d(0, vGridSize.y * iTileSize), olc::vi2d(vGridSize.x * iTileSize, vGridSize.y * iTileSize));
}

void MidPoint::DrawHovered(const std::vector<olc::vi2d>& arrTiles)
{
    for(int k = 0; k < arrTiles.size(); k++)
    {
        FillRect(olc::vi2d(arrTiles[k].x * iTileSize, arrTiles[k].y * iTileSize), olc::vi2d(iTileSize, iTileSize), olc::CYAN);
    }
}

std::vector<olc::vi2d> MidPoint::MidPointAlgo(olc::vi2d vCenter, int radius)
{
	std::vector<olc::vi2d> arrResult;


	int x = floor(vCenter.x / iTileSize);
	int y = floor(vCenter.y / iTileSize);

	if (radius <= iTileSize /2)
	{
		arrResult.push_back(olc::vi2d(x, y));
	}
    else
    {
        int x0 = 0;
        int y0 = -floor(radius / iTileSize);
        int p = 3 * 2 * y0;

        while (x0 <= -y0)
        {
            double yMid = y0 + 0.5;

            if (x0 * x0 * iTileSize *iTileSize + yMid *iTileSize * yMid *iTileSize > radius * radius)
                y0++;
            /*if (p <= 0)
            {
                p += 2 * x0 + 1;
            }
            else
            {
                y0++;
                p += 2 * (x0 + y0) + 1;
            }*/

            arrResult.push_back(olc::vi2d(x + x0, y + y0));
            arrResult.push_back(olc::vi2d(x - x0, y + y0));
            arrResult.push_back(olc::vi2d(x + x0, y - y0));
            arrResult.push_back(olc::vi2d(x - x0, y - y0));
            arrResult.push_back(olc::vi2d(x + y0, y + x0));
            arrResult.push_back(olc::vi2d(x - y0, y + x0));
            arrResult.push_back(olc::vi2d(x + y0, y - x0));
            arrResult.push_back(olc::vi2d(x - y0, y - x0));
            x0++;
        }
    }

    return arrResult;
}

std::vector<olc::vi2d> MidPoint::MidPointAlgoInt(olc::vi2d vCenter, int radius)
{
    std::vector<olc::vi2d> arrResult;


    return std::vector<olc::vi2d>();
}

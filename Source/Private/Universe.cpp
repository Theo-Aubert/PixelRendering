#include "../Public/Universe.h"
#include <random>

bool Universe::OnUserCreate()
{
    return true;
}

bool Universe::OnUserUpdate(float fElapsedTime)
{
    bool bState = false;

    switch (currentLevel)
    {
    case LevelID::Menu:
        bState = UpdateMenu(fElapsedTime);
        break;
    case LevelID::Galaxy :
        bState = UpdateGalaxy(fElapsedTime);
        break;
    default:
        break;
    }

    return bState;
  
}

bool Universe::UpdateMenu(float fElapsedTime)
{
    //Commands
    //Enter main level 
    if (GetKey(olc::ENTER).bReleased)
    {
        currentLevel = LevelID::Galaxy;
        return true;
    }

    //Quit
    if (GetKey(olc::ESCAPE).bReleased)
        return false;

    //Display
    Clear(olc::BLACK);
    DrawString(olc::vi2d(50, 50),  std::string("Ocean of Stars"),               olc::YELLOW,15U);
    DrawString(olc::vi2d(50, 150), std::string("A simulation by TAU"),          olc::RED,   5U);
    DrawString(olc::vi2d(50, 200), std::string("Press [ENTER] to continue"),    olc::WHITE, 10U);


    return true;
}

bool Universe::UpdateGalaxy(float fElapsedTime)
{

    //Return to menu
    if (GetKey(olc::ESCAPE).bReleased)
    {
        currentLevel = LevelID::Menu;
        return true;
    }

    if (GetKey(olc::R).bReleased)
    {
        srand(time(NULL));
        seed = rand();
    }
    if (GetKey(olc::Z).bHeld) universeOffset.y -= 50.0f * fElapsedTime;
    if (GetKey(olc::S).bHeld) universeOffset.y += 50.0f * fElapsedTime;
    if (GetKey(olc::Q).bHeld) universeOffset.x -= 50.0f * fElapsedTime;
    if (GetKey(olc::D).bHeld) universeOffset.x += 50.0f * fElapsedTime;


    //if (GetMouseWheel() > 0) sectorSize = std::max(1, sectorSize / 2);
    //if (GetMouseWheel() < 0) sectorSize = std::min(128, sectorSize * 2);

    Clear(olc::BLACK);

    olc::vi2d mouse = { GetMouseX() / sectorSize, GetMouseY() / sectorSize };
    olc::vi2d universeMouse = mouse + universeOffset;

    uint8_t halfSectorSize = 8; // std::max((uint8_t)1, (uint8_t)(sectorSize / 2));

    int nSectorsX = ScreenWidth() / sectorSize;
    int nSectorsY = ScreenHeight() / sectorSize;

    olc::vi2d screenSector = { 0,0 };

    for (screenSector.x = 0; screenSector.x < nSectorsX; screenSector.x++)
    {
        for (screenSector.y = 0; screenSector.y < nSectorsY; screenSector.y++)
        {
            StarSystem star(screenSector.x + (uint32_t)universeOffset.x, screenSector.y + (uint32_t)universeOffset.y);

            if (star.bStarExists)
            {
                FillCircle(screenSector.x * sectorSize + halfSectorSize, screenSector.y * sectorSize + halfSectorSize, (int)(star.starDiameter / halfSectorSize), star.starColor);

                if (mouse.x == screenSector.x && mouse.y == screenSector.y)
                {
                    DrawCircle(screenSector.x * sectorSize + halfSectorSize, screenSector.y * sectorSize + halfSectorSize, (int)(star.starDiameter / halfSectorSize) + 2, olc::YELLOW);
                }
            }
        }
    }

    if (GetMouse(olc::Mouse::LEFT).bPressed)
    {
        StarSystem star(universeMouse.x, universeMouse.y);

        if (star.bStarExists)
        {
            bStarSelected = true;
            selectedStar = universeMouse;
        }
        else
        {
            bStarSelected = false;
        }
    }

    if (bStarSelected)
    {
        StarSystem star(selectedStar.x, selectedStar.y, true);

        //Draw Windom
        FillRect(8, 240, 496, 232, olc::DARK_BLUE);
        DrawRect(8, 240, 496, 232, olc::WHITE);

        olc::vi2d vBody = { 14, 356 };
        vBody.x += star.starDiameter * 1.375;
        FillCircle(vBody, (int)(star.starDiameter * 1.375), star.starColor);
        vBody.x += (star.starDiameter * 1.375) + halfSectorSize;

        //Draw Planets
        for (sPlanet& planet : star.planets)
        {
            if (vBody.x + planet.diameter >= 496) break;

            vBody.x += planet.diameter;
            FillCircle(vBody, (int)(planet.diameter * 1.0), olc::PixelF(planet.minerals, planet.foliage, planet.water));

            olc::vi2d vMoon = vBody;
            for (sMoon& moon : planet.Moons)
            {
                vMoon.y += planet.diameter + moon.distance;
                FillCircle(vMoon, (int)(moon.diameter * 1.0), olc::GREY);
                vMoon.y += 10;
            }
            vBody.x += planet.diameter + 8;

        }


    }

    return true;
}

uint32_t StarSystem::Lehmer32()
{
    //I have no idea what's going on here 
    nLehmer += 0xe120fc15;
    uint64_t tmp;
    tmp = (uint64_t)nLehmer * 0x4a39b70d;
    uint32_t m1 = (tmp >> 32) ^ tmp;
    tmp = (uint64_t)m1 * 0x12fad5c9;
    uint32_t m2 = (tmp >> 32) ^ tmp;
    return m2;
}

int StarSystem::rndInt(int min, int max)
{
     return (Lehmer32() % (max - min)) + min;
}

double StarSystem::rndDouble(double min, double max)
{
    return ((double)Lehmer32() / double(0x7FFFFFFF)) * (max - min) + min;
}

StarSystem::StarSystem(uint32_t x, uint32_t y, bool bGenerateFullSystem)
{
    nLehmer = (x & 0xFFFF) << 16 | (y & 0xFFFF);
    nLehmer ^= seed;

    bStarExists = (rndInt(0, 20) == 1);

    if (!bStarExists) return;

    starDiameter = rndDouble(10.0, 40.0);
    starColor.n = g_starColors[rndInt(0, 8)];

    if (!bGenerateFullSystem) return;

    double dDistanceFromStar = rndDouble(60.0, 200.0);
    int nPlanets = rndInt(0, 10);
    for (int i = 0; i < nPlanets; i++)
    {
        sPlanet p;
        p.distance = dDistanceFromStar;
        dDistanceFromStar += rndDouble(20.0, 200.0);
        p.diameter = rndDouble(4.0, 20.0);
        p.temperature = (1700 - p.distance * 0.25 ) + rndDouble(-20.0, 30.0);

        //TO DO :Make environnemental variables function of the distance and temperature
        p.foliage = rndDouble(0.0, 1.0);
        p.minerals = rndDouble(0.0, 1.0);
        p.water = rndDouble(0.0, 1.0);
        p.gases = rndDouble(0.0, 1.0);

        if ((p.foliage + p.minerals + p.gases + p.water) > 0)
        {
            double dSum = 1.0 / (p.foliage + p.minerals + p.gases + p.water);
            p.foliage *= dSum;
            p.minerals *= dSum;
            p.water *= dSum;
            p.gases *= dSum;
        }

        p.population = std::max(rndInt(-5000000, 20000000), 0);

        int nMoons = std::max(rndInt(-15, 25), 0);
        for (int j = 0; j < nMoons; j++)
        {
            sMoon moon;
            moon.diameter = rndDouble(0.05, 0.35) * p.diameter;
            moon.distance = rndDouble(5., 20.0);

            p.Moons.push_back(moon);
        }


        planets.push_back(p);
    }
}

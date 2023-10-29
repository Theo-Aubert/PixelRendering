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

void Universe::GoBackToMainMenu()
{
    currentLevel = LevelID::Menu;
    bHasMenuBeenDrawn = false;
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
    if (!bHasMenuBeenDrawn)
    {

        Clear(olc::BLACK);

        std::string strMainTittle   = "Ocean of Stars";
        std::string strAuthor       = "A simulation by TAU";
        std::string strContinue     = "Press [ENTER] to continue";

        DrawNormalizedString(strMainTittle, MAIN_TITTLE_WIDTH, MAIN_TITTLE_SPACING_LEFT, MAIN_TITTLE_SPACING_TOP, true, olc::YELLOW);
        DrawNormalizedString(strAuthor,AUTHOR_WIDTH, AUTHOR_SPACING_LEFT, AUTHOR_SPACING_TOP, true, olc::RED);
        DrawNormalizedString(strContinue, CONTINUE_WIDTH, CONTINUE_SPACING_LEFT, CONTINUE_SPACING_TOP, true);

        bHasMenuBeenDrawn = true;
    }

    return true;
}

bool Universe::UpdateGalaxy(float fElapsedTime)
{

    //Return to menu
    if (GetKey(olc::ESCAPE).bReleased)
    {
        GoBackToMainMenu();
        return true;
    }

    if (GetKey(olc::R).bReleased)
    {
        srand((unsigned int)time(NULL));
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

        //bHasSolarSystemPanelBeenDraw = false;
    }

    if (bStarSelected /* && !bHasSolarSystemPanelBeenDraw*/)
    {
        const StarSystem star(selectedStar.x, selectedStar.y, true);

        switch (currentStarVisu)
        {
        case GalaxyStarVisualization::Accurate:
            DrawAccurateStarSystemVisualization(star);
            break;
        case GalaxyStarVisualization::Simplified:
            DrawSimplifiedStarSystemVisualization(star);
            break;
        default:
            break;
        }

    }

    return true;
}

void Universe::DrawAccurateStarSystemVisualization(const StarSystem& star)
{
    //Draw Window

    olc::vi2d rectSize = olc::vi2d(SOLAR_SYSTEM_BOX_WIDTH * ScreenWidth(), SOLAR_SYSTEM_BOX_HEIGHT * ScreenHeight());
    olc::vi2d rectPos = olc::vi2d(SOLAR_SYSTEM_SPACING_LEFT * ScreenWidth(), SOLAR_SYSTEM_SPACING_TOP * ScreenHeight());

    FillRect(rectPos, rectSize, olc::DARK_BLUE);
    DrawRect(rectPos, rectSize, olc::WHITE);


    //Get the biggest body in our system (should be the star, but we never know :) )
    double widestBodyDiameter = star.starDiameter;
    for (const sPlanet& planet : star.planets)
    {
        widestBodyDiameter = std::max(widestBodyDiameter, planet.diameter);
    }

    //Normalizing system size to fit in the box
    olc::vd2d vSystemSize = star.ComputeSystemSize();
    olc::vd2d vScalingVector = olc::vd2d((rectSize.x - 10.0 /*5 pixels margin left and right*/) / vSystemSize.x, (rectSize.y - 10.0 /*5 pixels margin up and down*/) / vSystemSize.y);
    double dScalingVector = std::min((rectSize.x - 10.0 /*5 pixels margin left and right*/) / vSystemSize.x, (rectSize.y - 10.0 /*5 pixels margin up and down*/) / vSystemSize.y); //px.unit^-1
    

    //int32_t uTopSpacing = rectPos.y + 5 + (uint32_t)(widestBodyDiameter * vScalingVector.x);
    int32_t uTopSpacing = rectPos.y + 5 + (uint32_t)(widestBodyDiameter * dScalingVector / 2.0);

    //DrawRect(rectPos + olc::vi2d(10, 10), star.ComputeSystemSize(), olc::RED);

    olc::vi2d vBody = { rectPos.x + 5, uTopSpacing };
    // vBody.x += (int32_t)(star.starDiameter * vScalingVector.x);
     //FillCircle(vBody, (int)(star.starDiameter * vScalingVector.x), star.starColor);

    vBody.x += (int32_t)(star.starDiameter * dScalingVector / 2.0);
    FillCircle(vBody, (int)(star.starDiameter * dScalingVector / 2.0), star.starColor);
    vBody.x += (int32_t)(star.starDiameter * dScalingVector / 2.0);

    //Draw Planets
    for (const sPlanet& planet : star.planets)
    {
        if (vBody.x + planet.diameter * dScalingVector >= (rectPos.x + rectSize.x) - 5.0) break;

        /*vBody.x += (int32_t)(planet.distance * vScalingVector.x);
        vBody.x += (int32_t)(planet.diameter * vScalingVector.x);
        FillCircle(vBody, (int)(planet.diameter * vScalingVector.x), olc::PixelF((float)planet.minerals, (float)planet.foliage, (float)planet.water));*/

        vBody.x += (int32_t)(planet.distance * dScalingVector);
        vBody.x += (int32_t)(planet.diameter * dScalingVector / 2.0);
        FillCircle(vBody, (int)(planet.diameter * dScalingVector / 2.0), olc::PixelF((float)planet.minerals, (float)planet.foliage, (float)planet.water));
        

        olc::vi2d vMoon = vBody;
        for (const sMoon& moon : planet.Moons)
        {
            if (vMoon.y + (moon.diameter + planet.diameter/ 2.0 + moon.distance) * dScalingVector >= (rectPos.y + rectSize.y)) break;

            vMoon.y += (int32_t)(planet.diameter * dScalingVector / 2.0);
            vMoon.y += (int32_t)(moon.distance * dScalingVector);
            vMoon.y += (int32_t)(moon.diameter * dScalingVector / 2.0);
            FillCircle(vMoon, (int)std::min(1.0,(moon.diameter * dScalingVector / 2.0)), olc::GREY);
            vMoon.y += (int32_t)(moon.diameter * dScalingVector / 2.0);
        }

        vBody.x += (int32_t)(planet.diameter * dScalingVector / 2.0);

    }

    //bHasSolarSystemPanelBeenDraw = true;
}

void Universe::DrawSimplifiedStarSystemVisualization(const StarSystem& star)
{
}

bool Universe::DrawNormalizedString(const std::string& string, float fWidth, float fLeftSpacing, float fTopSpacing, bool bAutoCenter, const olc::Pixel& color)
{
    olc::vi2d vStringSize = GetTextSize(string);

    if (vStringSize.x <= 0)
    {
        return false;
    }

    uint32_t uStringScale = (uint32_t)((ScreenWidth() * fWidth) / (float)vStringSize.x);
    olc::vi2d vStringPos = olc::vi2d((int32_t)(fLeftSpacing * ScreenWidth()), (int32_t)(fTopSpacing * ScreenHeight()));

    /*std::cout << string << ": ";
    std::cout << "Lft spacing = " << (int32_t)(fLeftSpacing * ScreenWidth()) << " | ";
    std::cout << "String size = " << (int32_t)(ScreenWidth() * fWidth) << " | ";
    std::cout << "Rght Spacing = " << ScreenWidth() - ((int32_t)(ScreenWidth() * fWidth) + (int32_t)(fLeftSpacing * ScreenWidth())) << " | ";

    std::cout << "Scale factor = " << (uint32_t)((ScreenWidth() * fWidth) / (float)vStringSize.x) << " | ";
    std::cout << "Screen size = " << uStringScale * vStringSize.x << "\n\n";*/
    if(!bAutoCenter)
        DrawString(vStringPos, string, color, uStringScale);
    else
    {
        uint32_t uRealScreenSize = uStringScale * vStringSize.x;
        vStringPos.x = (int32_t)(0.5f * (ScreenWidth() - uRealScreenSize));
        DrawString(vStringPos, string, color, uStringScale);
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

olc::vd2d StarSystem::ComputeSystemSize() const
{
    olc::vd2d result;

    result.x += starDiameter;
    result.y = starDiameter;
    for (const auto& planet : planets)
    {
        olc::vd2d systemSize = planet.ComputeSystemSize();

        result.x += planet.distance + systemSize.y;
        result.y = std::max(result.y, systemSize.x);
    }
    return result;
}

olc::vd2d sPlanet::ComputeSystemSize() const
{
    olc::vd2d result;

    result.x += diameter;
    result.y = diameter;
    for (const auto& moon : Moons)
    {
        result.x += moon.distance + moon.diameter;
        result.y = std::max(result.y, moon.diameter);
    }
    return result;
}

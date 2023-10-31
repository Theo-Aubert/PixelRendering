#include "../Public/Universe.h"
#include <random>

bool Universe::OnUserCreate()
{
    pAccurateDisplaySprite = new olc::Sprite(SOLAR_SYSTEM_BOX_WIDTH * ScreenWidth(), SOLAR_SYSTEM_BOX_HEIGHT * ScreenHeight());
    pSimplifiedDisplaySprite = new olc::Sprite(SOLAR_SYSTEM_BOX_WIDTH * ScreenWidth(), SOLAR_SYSTEM_BOX_HEIGHT * ScreenHeight());

    vDisplayTopLeftCorner = olc::vi2d(SOLAR_SYSTEM_SPACING_LEFT * ScreenWidth(), SOLAR_SYSTEM_SPACING_TOP * ScreenHeight());

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
    case LevelID::SolarSystem :
        bState = UpdateStarSystem(fElapsedTime);
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
    if (GetKey(olc::Z).bHeld) universeOffset.y -= uNumSectorSizeX * fElapsedTime;
    if (GetKey(olc::S).bHeld) universeOffset.y += uNumSectorSizeX * fElapsedTime;
    if (GetKey(olc::Q).bHeld) universeOffset.x -= uNumSectorSizeX * fElapsedTime;
    if (GetKey(olc::D).bHeld) universeOffset.x += uNumSectorSizeX * fElapsedTime;

    if (GetKey(olc::NP1).bReleased && bStarSelected) currentStarVisu = GalaxyStarVisualization::Accurate;
    if (GetKey(olc::NP2).bReleased && bStarSelected) currentStarVisu = GalaxyStarVisualization::Simplified;
    if (GetKey(olc::NP3).bReleased && bStarSelected) currentLevel = LevelID::SolarSystem;

    if (GetMouseWheel() > 0)
    {
        uNumSectorSizeX = std::max(1, uNumSectorSizeX / 2);
        //uint16_t uSectorSize = ScreenWidth() / uNumSectorSizeX;
        //olc::vi2d mouse = { GetMouseX() / uSectorSize, GetMouseY() / uSectorSize };
        //olc::vf2d universeMouse = mouse + universeOffset;    

        //universeOffset = universeMouse - olc::vf2d(SECTOR_SIZE * (uNumSectorSizeX + 1)/ 2.0, SECTOR_SIZE * std::floor((uNumSectorSizeX * ((double)ScreenHeight() / ScreenWidth()) + 1)/ 2.0));
        //universeOffset.x += SECTOR_SIZE * (uNumSectorSizeX + 1) / 2.0;// , SECTOR_SIZE* std::floor(uNumSectorSizeX * ((double)ScreenHeight() / ScreenWidth()) / 2));
        //universeOffset += mouse * SECTOR_SIZE - olc::vf2d((uNumSectorSizeX / 2) * SECTOR_SIZE, (uNumSectorSizeX / 2) * SECTOR_SIZE);
    }
    if (GetMouseWheel() < 0)
    {
        uNumSectorSizeX = std::min(512, uNumSectorSizeX * 2);

       // uint16_t uSectorSize = ScreenWidth() / uNumSectorSizeX;
        //olc::vi2d mouse = { GetMouseX() / uSectorSize, GetMouseY() / uSectorSize };

        //universeOffset -= olc::vf2d((uNumSectorSizeX / 2) * SECTOR_SIZE, (uNumSectorSizeX / 2) * SECTOR_SIZE);
        //universeOffset -= olc::vf2d(SECTOR_SIZE * uNumSectorSizeX / 4, SECTOR_SIZE * std::floor(uNumSectorSizeX * ((double)ScreenHeight() / ScreenWidth()) / 4));
        
    }

    Clear(olc::BLACK);

    uint16_t numSectorX = uNumSectorSizeX;
    uint16_t numSectorY = (uint16_t)std::floor(uNumSectorSizeX * ((double)ScreenHeight() / ScreenWidth()));

    //std::cout << (int)numSectorY << "\n";
    uint16_t uSectorSize = ScreenWidth() / numSectorX;


    //olc::vi2d mouse = { GetMouseX() / sectorSize, GetMouseY() / sectorSize };
    olc::vi2d mouse = { GetMouseX() / uSectorSize, GetMouseY() / uSectorSize };
    olc::vi2d universeMouse = mouse + universeOffset;

    uint8_t halfSectorSize = 8; // std::max((uint8_t)1, (uint8_t)(sectorSize / 2));
    uint16_t uHalfSectorSize = uSectorSize / 2;

    int nSectorsX = ScreenWidth() / sectorSize;
    int nSectorsY = ScreenHeight() / sectorSize;

    olc::vi2d screenSector = { 0,0 };

    for (screenSector.x = 0; screenSector.x < numSectorX; screenSector.x++)
    {
        for (screenSector.y = 0; screenSector.y < numSectorY; screenSector.y++)
        {
            StarSystem star(screenSector.x + (uint32_t)universeOffset.x, screenSector.y + (uint32_t)universeOffset.y);

            if (star.bStarExists)
            {
                FillCircle(screenSector.x * uSectorSize + uHalfSectorSize, screenSector.y * uSectorSize + uHalfSectorSize, (int)(star.starDiameter / uNumSectorSizeX), star.starColor);

                if (mouse.x == screenSector.x && mouse.y == screenSector.y)
                {
                    DrawCircle(screenSector.x * uSectorSize + uHalfSectorSize, screenSector.y * uSectorSize + uHalfSectorSize, (int)(star.starDiameter / uNumSectorSizeX) + 2, olc::YELLOW);
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

            if (!pGazedStar || universeMouse != selectedStar)
            {
                selectedStar = universeMouse;
                pGazedStar = new StarSystem(universeMouse.x, universeMouse.y, true);

                DrawAccurateStarSystemVisualization(*pGazedStar);
                DrawSimplifiedStarSystemVisualization(*pGazedStar);
            }
            
        }
        else
        {
            bStarSelected = false;
        }

        //bHasSolarSystemPanelBeenDraw = false;
    }

    if (bStarSelected /* && !bHasSolarSystemPanelBeenDraw*/)
    {
       // const StarSystem star(selectedStar.x, selectedStar.y, true);

        switch (currentStarVisu)
        {
        case GalaxyStarVisualization::Accurate:
            DrawSprite(vDisplayTopLeftCorner, pAccurateDisplaySprite);
            //DrawAccurateStarSystemVisualization(*pGazedStar);
            break;
        case GalaxyStarVisualization::Simplified:
            DrawSprite(vDisplayTopLeftCorner, pSimplifiedDisplaySprite);
            //DrawSimplifiedStarSystemVisualization(*pGazedStar);
            break;
        default:
            break;
        }

        //pGazedStar = new StarSystem(selectedStar.x, selectedStar.y, true);

    }

    return true;
}

bool Universe::UpdateStarSystem(float fElapsedTime)
{
    if (GetKey(olc::ESCAPE).bReleased || !pGazedStar) currentLevel = LevelID::Galaxy;

    Clear(olc::BLACK);

    uint16_t numSectorX = uNumSectorSizeX;
    uint16_t numSectorY = (uint16_t)std::floor(uNumSectorSizeX * ((double)ScreenHeight() / ScreenWidth()));

    //std::cout << (int)numSectorY << "\n";
    uint16_t uSectorSize = ScreenWidth() / numSectorX;
    uint16_t uHalfSectorSize = uSectorSize / 2;

    olc::vi2d screenSector = { 0,0 };

    for (screenSector.x = 0; screenSector.x < numSectorX; screenSector.x++)
    {
        for (screenSector.y = 0; screenSector.y < numSectorY; screenSector.y++)
        {
            StarSystem star(screenSector.x + (uint32_t)universeOffset.x, screenSector.y + (uint32_t)universeOffset.y);

            if (star.bStarExists)
            {
                FillCircle(screenSector.x * uSectorSize + uHalfSectorSize, screenSector.y * uSectorSize + uHalfSectorSize, (int)(star.starDiameter / uNumSectorSizeX), star.starColor);
            }
        }
    }

    //StarSystem star(selectedStar.x, selectedStar.y, true);

    if (pGazedStar->bStarExists)
    {
        olc::vi2d vStarPos = olc::vi2d{ ScreenWidth() / 2, ScreenHeight() / 2 };

        olc::vd2d vSystemSize   = pGazedStar->ComputeSystemSize();
        double dSystemLength    = pGazedStar->OuterBound();
        double dScalingFactor   = std::min((ScreenWidth() - 10) / (/*2.0 */ dSystemLength), (ScreenHeight() - 10) / (/*2.0 */ dSystemLength));

        FillCircle(vStarPos, (int32_t)((pGazedStar->starDiameter * dScalingFactor / 2.0) /* 1.375*/), pGazedStar->starColor);

        for (auto& planet : pGazedStar->planets)
        {
            olc::vi2d vPlanetPos = vStarPos;
            
            DrawCircle(vPlanetPos   , (int32_t)(planet.distance * dScalingFactor / 2.0));

            //Planet rotation
            vPlanetPos.x += cos(planet.angle) * planet.distance * dScalingFactor / 2.0;
            vPlanetPos.y += sin(planet.angle) * planet.distance * dScalingFactor / 2.0;
            planet.angle += planet.angularSpeed * fElapsedTime;

            FillCircle(vPlanetPos     , (int32_t)((planet.diameter * dScalingFactor / 2.0) /* 1.375*/)   , olc::PixelF((float)planet.minerals, (float)planet.foliage, (float)planet.water));

            for (auto& moon : planet.Moons)
            {
                olc::vi2d vMoonPos = vPlanetPos;

                DrawCircle(vMoonPos, (int32_t)(moon.distance * dScalingFactor / 2.0));

                vMoonPos.x += cos(moon.angle) * moon.distance * dScalingFactor / 2.0;
                vMoonPos.y += sin(moon.angle) * moon.distance * dScalingFactor / 2.0;
                moon.angle += moon.angularSpeed;

                FillCircle(vMoonPos, (int32_t)((moon.diameter * dScalingFactor / 2.0) /* 1.375*/), olc::GREY);
            }
        }

    }

    
    //DrawCircle(250, 250, 250);
    return true;
}

void Universe::DrawAccurateStarSystemVisualization(const StarSystem& star)
{
    //Draw Window

    SetDrawTarget(pAccurateDisplaySprite);

    olc::vi2d rectSize = { GetDrawTargetWidth(), GetDrawTargetHeight() };
    olc::vi2d rectPos = { 0, 0 };

    Clear(olc::DARK_BLUE);
    DrawRect(rectPos, rectSize - olc::vi2d(1, 1), olc::WHITE);


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
    
    int32_t uTopSpacing = rectPos.y + 5 + (uint32_t)(widestBodyDiameter * dScalingVector / 2.0) + (uint32_t)((rectSize.y - vSystemSize.y * dScalingVector)  / 2.0);

    olc::vi2d vBody = { rectPos.x + 5, uTopSpacing };
    vBody.x += (int32_t)(star.starDiameter * dScalingVector / 2.0);
    FillCircle(vBody, (int)(star.starDiameter * dScalingVector / 2.0), star.starColor);
    vBody.x += (int32_t)(star.starDiameter * dScalingVector / 2.0);

    //Draw Planets
    for (const sPlanet& planet : star.planets)
    {
        if (vBody.x + (planet.distance + planet.diameter) * dScalingVector >= (rectPos.x + rectSize.x) - 5.0) break;
       
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

    SetDrawTarget(nullptr);
}

void Universe::DrawSimplifiedStarSystemVisualization(const StarSystem& star)
{
    //Draw Window

    SetDrawTarget(pSimplifiedDisplaySprite);

    olc::vi2d rectSize = { GetDrawTargetWidth(), GetDrawTargetHeight() };
    olc::vi2d rectPos = { 0, 0 };

    Clear(olc::DARK_BLUE);
    DrawRect(rectPos, rectSize - olc::vi2d(1,1), olc::WHITE);


    //Get the biggest body in our system (should be the star, but we never know :) )
    double widestBodyDiameter = star.starDiameter;
    for (const sPlanet& planet : star.planets)
    {
        widestBodyDiameter = std::max(widestBodyDiameter, planet.diameter);
    }

    //Normalizing system size to fit in the box
    olc::vd2d vSystemSize = star.ComputeSimplifiedSystemSize();
    olc::vd2d vScalingVector = olc::vd2d((rectSize.x - 10.0 /*5 pixels margin left and right*/) / vSystemSize.x, (rectSize.y - 10.0 /*5 pixels margin up and down*/) / vSystemSize.y);
    double dScalingVector = std::min((rectSize.x - 10.0 /*5 pixels margin left and right*/) / vSystemSize.x, (rectSize.y - 10.0 /*5 pixels margin up and down*/) / vSystemSize.y); //px.unit^-1

    int32_t uTopSpacing = rectPos.y + 5 + (uint32_t)(widestBodyDiameter * dScalingVector / 2.0) + (uint32_t)((rectSize.y - vSystemSize.y * dScalingVector) / 2.0);

    olc::vi2d vBody = { rectPos.x + 5, uTopSpacing };
    vBody.x += (int32_t)(star.starDiameter * dScalingVector / 2.0);
    FillCircle(vBody, (int)(star.starDiameter * dScalingVector / 2.0), star.starColor);
    vBody.x += (int32_t)(star.starDiameter * dScalingVector / 2.0);

    //Draw Planets
    for (const sPlanet& planet : star.planets)
    {
        if (vBody.x + planet.diameter * dScalingVector >= (rectPos.x + rectSize.x) - 5.0) break;

        vBody.x += (int32_t)(SOLAR_SYSTEM_PLANET_DISTANCE * dScalingVector);
        vBody.x += (int32_t)(planet.diameter * dScalingVector / 2.0);
        FillCircle(vBody, (int)(planet.diameter * dScalingVector / 2.0), olc::PixelF((float)planet.minerals, (float)planet.foliage, (float)planet.water));

        olc::vi2d vMoon = vBody;
        for (const sMoon& moon : planet.Moons)
        {
            if (vMoon.y + (moon.diameter + planet.diameter / 2.0 + moon.distance) * dScalingVector >= (rectPos.y + rectSize.y)) break;

            vMoon.y += (int32_t)(planet.diameter * dScalingVector / 2.0);
            vMoon.y += (int32_t)(SOLAR_SYSTEM_MOON_DISTANCE * dScalingVector);
            vMoon.y += (int32_t)(moon.diameter * dScalingVector / 2.0);
            FillCircle(vMoon, (int)std::min(1.0, (moon.diameter * dScalingVector / 2.0)), olc::GREY);
            vMoon.y += (int32_t)(moon.diameter * dScalingVector / 2.0);
        }

        vBody.x += (int32_t)(planet.diameter * dScalingVector / 2.0);

    }

    SetDrawTarget(nullptr);
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
        p.angularSpeed = rndDouble(-PI / 8.0, PI / 8.0);
        p.angle = rndDouble(0.0, 2 * PI);

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

            moon.angularSpeed = rndDouble(-PI / (moon.distance *256.0), PI / (moon.distance * 256.0));
            moon.angle = rndDouble(0.0, 2 * PI);

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

olc::vd2d StarSystem::ComputeSimplifiedSystemSize() const
{
    olc::vd2d result;

    result.x += starDiameter;
    result.y = starDiameter;
    for (const auto& planet : planets)
    {
        olc::vd2d systemSize = planet.ComputeSimplifiedSystemSize();

        result.x += SOLAR_SYSTEM_PLANET_DISTANCE + systemSize.y;
        result.y = std::max(result.y, systemSize.x);
    }
    return result;
}

double StarSystem::OuterBound() const
{
    double result = starDiameter;
    if (planets.size())
    {
        sPlanet farMostPlanet = planets.back();
        result = farMostPlanet.distance + (farMostPlanet.diameter / 2.0);
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

olc::vd2d sPlanet::ComputeSimplifiedSystemSize() const
{
    olc::vd2d result;

    result.x += diameter;
    result.y = diameter;
    for (const auto& moon : Moons)
    {
        result.x += SOLAR_SYSTEM_MOON_DISTANCE + moon.diameter;
        result.y = std::max(result.y, moon.diameter);
    }
    return result;
}

double sPlanet::ComputeOuterBound() const
{
    double result = diameter / 2.0;
    if (Moons.size())
    {
        sMoon farMostMoon = Moons.back();
        result = farMostMoon.distance + (farMostMoon.diameter / 2.0);
    }
    return result;
}

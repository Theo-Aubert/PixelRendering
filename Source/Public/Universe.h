#pragma once

#include "../Externals/olcPixelGameEngine.h"
#include "Constants.h"

//UI Absolute defines

//Menu
#define MAIN_TITTLE_WIDTH			0.7f
#define MAIN_TITTLE_SPACING_LEFT	0.15f
#define MAIN_TITTLE_SPACING_TOP		0.4f

#define AUTHOR_WIDTH				0.5f
#define AUTHOR_SPACING_LEFT			0.25f
#define AUTHOR_SPACING_TOP			0.6f
									 
#define CONTINUE_WIDTH				0.9f
#define CONTINUE_SPACING_LEFT		0.05f
#define CONTINUE_SPACING_TOP		0.8f
									 
//Galaxy Exploration			 
#define SOLAR_SYSTEM_BOX_WIDTH		0.9f
#define SOLAR_SYSTEM_BOX_HEIGHT		0.45f
#define SOLAR_SYSTEM_SPACING_LEFT	0.05f
#define SOLAR_SYSTEM_SPACING_TOP	0.50f

#define SECTOR_SIZE						500.
#define SOLAR_SYSTEM_MOON_DISTANCE		5.
#define SOLAR_SYSTEM_PLANET_DISTANCE	15.

struct sMoon
{

	double distance = 0.0;
	double diameter = 0.0;

	double angularSpeed = 0.0;
	double angle = 0.0;
};

struct sPlanet
{
	double distance = 0.0;
	double diameter = 0.0;
	double angularSpeed = 0.0;
	double angle = 0.0;
	double foliage = 0.0;
	double minerals = 0.0;
	double water = 0.0;
	double gases = 0.0;
	double temperature = 0.0;
	double population = 0.0f;
	bool ring = false;
	std::vector<sMoon> Moons;

	//Sum up the Planet diameter with all its moons' diameter and distance on x axis and return the max diameter of the all system on y
	olc::vd2d ComputeSystemSize() const;

	//Same has abose but convert distance to constants
	olc::vd2d ComputeSimplifiedSystemSize() const;

	double ComputeOuterBound() const;
};

constexpr uint32_t g_starColors[8] =
{
	0xFFFFFFFF, 0xFFD9FFFF, 0xFFA3FFFF, 0xFFFFC8C8,
	0xFFFFC89D, 0xFF9F9FFF, 0xFF415EFF, 0xFF28199D
};

inline uint32_t seed = 0;

class StarSystem
{
public:

	StarSystem(uint32_t x, uint32_t y, bool bGenerateFullSystem = false);

	//Sum up the star diameter with all its planets' SystemSize.y and distance on x axis and return the max diameter of the all system on y
	olc::vd2d ComputeSystemSize() const;

	//Same has abose but convert distance to constants
	olc::vd2d ComputeSimplifiedSystemSize() const;

	//Compute the greatest distance from center to star system to its farmost body (moons not included) 
	double OuterBound() const;

public :

	//States Variables
	bool bStarExists = false;
	double starDiameter = 0.0f;
	olc::Pixel starColor = olc::WHITE;

	std::vector<sPlanet> planets;
private:
	uint32_t Lehmer32();

	int rndInt(int min, int max);

	double rndDouble(double min, double max);
	

	uint32_t nLehmer = 0;

	
};

enum class LevelID : uint8_t
{
	Menu,
	Galaxy,
	SolarSystem,
	Planet,
	Moon
};

enum class GalaxyStarVisualization : uint8_t
{
	Accurate,
	Simplified
};

// Override base class with your custom functionality
class Universe : public olc::PixelGameEngine
{
public:
	Universe()
	{
		// Name your application
		sAppName = "Procedural Universe";
	}

public:
	bool OnUserCreate() override;

	bool OnUserUpdate(float fElapsedTime) override;

	

private :

	void GoBackToMainMenu();

	bool UpdateMenu(float fElapsedTime);

	bool UpdateGalaxy(float fElapsedTime);

	bool UpdateStarSystem(float fElapsedTime);

	bool UpdatePlanet(float fElapsedTime);

	bool UpdateMoon(float fElapsedTime);

	void DrawAccurateStarSystemVisualization(const StarSystem& star);

	void DrawSimplifiedStarSystemVisualization(const StarSystem& star);

	// fWidth, fLeftSpacing and fTopSaping must specifies absolute screen coordinates i. e. [0.0, 1.0]. Values out this range may mess things up 
	bool DrawNormalizedString(const std::string& string, float fWidth, float fLeftSpacing, float fTopSpacing, bool bAutoCenter = false, const olc::Pixel& color = olc::WHITE);

private :

	//Keep track of the current level
	LevelID currentLevel = LevelID::Menu;
	GalaxyStarVisualization currentStarVisu = GalaxyStarVisualization::Accurate;
	
	//Galaxy Level variables
	olc::Sprite* pAccurateDisplaySprite = nullptr;
	olc::Sprite* pSimplifiedDisplaySprite = nullptr;

	olc::vi2d vDisplayTopLeftCorner;

	uint8_t sectorSize = 16;
	uint16_t uNumSectorSizeX = 64;

	olc::vf2d universeOffset = { 0,0 };

	bool bStarSelected	 = false;
	bool bPlanetSelected = false;
	bool bMoonSelected	 = false;

	olc::vi2d selectedStar	 = { 0,0 };
	olc::vi2d selectedPlanet = { 0,0 };
	olc::vi2d selectedMoon	 = { 0,0 };

	StarSystem* pGazedStar = nullptr;

	//Display global states
	bool bHasMenuBeenDrawn = false;
	//bool bHasSolarSystemPanelBeenDraw = false;

};




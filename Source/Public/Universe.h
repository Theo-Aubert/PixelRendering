#pragma once

#include "../Externals/olcPixelGameEngine.h"


struct sMoon
{

	double distance = 0.0;
	double diameter = 0.0;
};

struct sPlanet
{
	double distance = 0.0;
	double diameter = 0.0;
	double foliage = 0.0;
	double minerals = 0.0;
	double water = 0.0;
	double gases = 0.0;
	double temperature = 0.0;
	double population = 0.0f;
	bool ring = false;
	std::vector<sMoon> Moons;
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

	bool UpdateMenu(float fElapsedTime);

	bool UpdateGalaxy(float fElapsedTime);

	bool UpdateSolarSystem(float fElapsedTime);

	bool UpdatePlanet(float fElapsedTime);

	bool UpdateMoon(float fElapsedTime);

private :

	LevelID currentLevel = LevelID::Menu;

	uint8_t sectorSize = 16;

	olc::vf2d universeOffset = { 0,0 };

	bool bStarSelected	 = false;
	bool bPlanetSelected = false;
	bool bMoonSelected	 = false;

	olc::vi2d selectedStar	 = { 0,0 };
	olc::vi2d selectedPlanet = { 0,0 };
	olc::vi2d selectedMoon	 = { 0,0 };

};




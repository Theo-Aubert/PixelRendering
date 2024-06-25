

#pragma once

#include "../Externals/olcPixelGameEngine.h"
#include <set>

#define RESSOURCE_PATH_TILES std::string(".../../Ressources/WFCTiles/")

enum  EConnect : uint8_t
{
	North	= 0,
	South	= 1,
	West	= 2,
	East	= 3	
};


enum EMaterial : uint8_t
{
	Grass	= 0,
	Road	= 1 
};

// very simple time 
struct SimpleTile 
{

	olc::Sprite* m_pSprite	= nullptr;
	olc::Decal* m_pDecal	= nullptr;
	//olc::Pixel m_arrPixels[3][3]; //green : grass, brown : road

	std::map<int, int> m_connexMap;

	
};

//Define our modules
static SimpleTile Cross;
static SimpleTile Empty;
static SimpleTile LVertical;
static SimpleTile LHorizontal;
static SimpleTile TUp;
static SimpleTile TDown;
static SimpleTile TRight;
static SimpleTile TLeft;
static SimpleTile CLeftUp;
static SimpleTile CLeftDown;
static SimpleTile CRightUp;
static SimpleTile CRightDown;

class WaveFC : public olc::PixelGameEngine
{

public:
	WaveFC()
	{
		// Name your application
		sAppName = "Wave Function Collapse Example";
	}


public:

	bool OnUserCreate() override;

	bool OnUserUpdate(float fElapsedTime) override;

	int m_iTileSize = 27;				//pixel size of a tile
	olc::vi2d m_vGridSize = { 16, 16 };	//number of tiles per row and per column
	std::vector < std::vector<std::set<SimpleTile*>>> m_vModuleGrid;  //inner representation of our grid

private:

	void InitGrid();

	bool AreCoordInBounds(const olc::vi2d& coord);
	bool AreCoordInBounds(int x, int y);

	size_t GetEntropy(const olc::vi2d& coord);
	size_t GetEntropy(int x, int y);
	
	void Propagate(const olc::vi2d& coord);
	void Propagate(int x, int y);
	
	bool IsTileCollapsed(const olc::vi2d& coord);
	bool IsTileCollapsed(int x, int y);
	
	void DrawEmptyGrid();

	void DrawModules();

	void DrawModuleConnex(SimpleTile& Tile, olc::vi2d& pos);


};

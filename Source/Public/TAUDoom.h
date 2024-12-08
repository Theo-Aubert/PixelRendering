#pragma once

#include "../Externals/olcPixelGameEngine.h"
#include "Constants.h"

//constexpr int screenWidth = 640;
//constexpr int screenHeight = 480;
constexpr int mapWidth = 24;
constexpr int mapHeight = 24;
constexpr float mapSizeX = 24.f;
constexpr float mapSizeY = 24.f;

constexpr int miniMapScale = 15;

constexpr float cellSize = mapSizeX / mapWidth; // world unit size 

struct TAUPlayer
{
	olc::vd2d vPosition;
	olc::vd2d vLookAt = olc::vd2d(cos(PI / 2), sin(PI / 2));
	olc::vd2d vCameraPlane;
	double dLookAtAngle = PI / 2;
	double dFoV = PI / 3;
	double dSpeed = 2.;

	double dRotateSpeed = PI / 4.;

	olc::vd2d GetRightVector();
};

class TAUDoom : public olc::PixelGameEngine
{
public:
	TAUDoom()
	{
		// Name your application
		sAppName = "TAUDoom";
	}

public:
	bool OnUserCreate() override;

	bool OnUserUpdate(float fElapsedTime) override;

private:

	void Setup2DMap();

	void DrawPlayer();

	void Render2DMap();

	void RenderDoomMap();

	int m_iStripes = 64;
	olc::vi2d m_vMapSize	= { mapWidth, mapHeight};
	olc::vd2d m_vMapDim		= { mapSizeX, mapSizeY }; //WorldUnit
	olc::vi2d m_vMapWindow	= { 512		, 512 };

	TAUPlayer player;

	//Sprites 
	olc::Sprite* pPlayerSprite = nullptr;
	olc::Sprite* pWallSprite = nullptr;
	olc::Sprite* pMiniMapSprite = nullptr;

	//Decals
	olc::Decal* pWallDecal = nullptr;
	olc::Decal* pPlayerDecal = nullptr;
	olc::Decal* pMiniMapDecal = nullptr;

	int worldMap[mapWidth][mapHeight] =
	{
	  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
	  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
	  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
	  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	};
};


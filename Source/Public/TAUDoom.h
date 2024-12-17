#pragma once

#include "../Externals/olcPixelGameEngine.h"
#include "Constants.h"
#include <set>

constexpr int mapWidth = 24;
constexpr int mapHeight = 24;

constexpr int miniMapScale = 16;
constexpr double d3DDisplayRatio = 16. / 9.;

constexpr double cellSize = 1.; // world unit size 

constexpr int ShadowMapWidth =480;
constexpr int ShadowMapHeight =480;

struct TAUPlayer
{
	olc::vd2d vPosition = { double(mapWidth) / 2., double(mapHeight) / 2. };
	double dLookAtAngle = PI / 2;
	olc::vd2d vLookAt = olc::vd2d(cos(dLookAtAngle), sin(dLookAtAngle));

	double dFoV = PI / 3;
	double dSpeed = 2.;

	double dRotateSpeed = PI / 2.;

	olc::vd2d GetRightVector();
};

struct DirectionalLight
{
	olc::vd2d pos;
	double height;

	olc::Pixel lightColor = olc::WHITE;
};

struct PointLight
{
	olc::vd2d pos;
	double height;

	olc::Pixel lightColor = olc::WHITE;

	double phi = 2 * PI;
	double theta = 2 * PI;
};

//Structs to determine polygons while performing shadow casting 
struct sEdge
{
	olc::vd2d start;
	olc::vd2d end;
};

struct sCell
{
	int edgeID[4];
	bool bEdgeExist[4];
	bool bExist = false;
};

enum CardinalPoints
{
	NNorth = 0,
	NSouth,
	NEast,
	NWest
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
	void ComputeShadowMap();
	void ConvertTileMapToPolyMap(const olc::vi2d& pos, const olc::vi2d& size, double dBlockWidth, int pitch, std::vector<sEdge>& outEdges);
	void CalculateVisibilityPolygon(olc::vd2d pos, double dRadius, const std::vector<sEdge>& Edges, std::vector<std::tuple<double, double, double>>& outVisibilityPolygonPoints);
	void CalculateDirectionalShadowPolygon(const DirectionalLight& light, const std::vector<sEdge>& Edges, std::vector<olc::vd2d>& outShadowPolygonPoints);

	void DrawPlayer();

	void Render2DMap(const olc::vi2d& vPos, const olc::vi2d& vResolution, bool bShowMapSprite);
	void RenderShadowMap(const olc::vi2d& vPos, const olc::vi2d& vResolution, bool bShowMapSprite);

	void RenderDoomMap(const olc::vi2d& vPos, const olc::vi2d& vResolution);

	int m_iDebugRays = 32;
	std::set<olc::vd2d> sHitPoints;
	std::vector<olc::vd2d> vShadowSamples;

	TAUPlayer player;

	//Sprites 
	olc::Sprite* pMiniMapSprite = nullptr;
	olc::Sprite* pShadowMap = nullptr;

	//Decals
	olc::Decal* pMiniMapDecal = nullptr;

	//   |----> Y
	//   |
	//	 v X

	int worldMap[mapWidth][mapHeight] =
	{
	  {6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6},
	  {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	  {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	  {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	  {6,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,6},
	  {6,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,6},
	  {6,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,6},
	  {6,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,6},
	  {6,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,6},
	  {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	  {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	  {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	  {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	  {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	  {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	  {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	  {6,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	  {6,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	  {6,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	  {6,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	  {6,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	  {6,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	  {6,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	  {6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6}
	};

	sCell* mCellWorld = nullptr;
	olc::vi2d lightPos = olc::vi2d(240., 240.);
	double m_dShadowBias = 0.0;


	std::vector<DirectionalLight*> m_arrDirectionalLights;
	std::vector<PointLight*> m_arrPointLights;

	//display settings
	bool m_bIs3DMainViewport = true; //Toggle between 3D secne or minimap as main display
	bool m_bShowMinimapSprite = true; //Minimap show static tiles or only dynbamic elements

	//Pixel-wise size and place for 2.5D rendering viewport 
	olc::vi2d v3DMainAnchor;
	olc::vi2d v3DMainResolution;
	olc::vi2d v3DMiniAnchor;
	olc::vi2d v3DMiniResolution;

	//Pixel_wise size and place for minimap display
	olc::vi2d v2DMainAnchor;
	olc::vi2d v2DMainResolution;
	olc::vi2d v2DMiniAnchor;
	olc::vi2d v2DMiniResolution;
	int i2DMainScale = 1;

	//textures
	olc::vi2d vTexSize = olc::vi2d(64,64);
	std::vector<olc::Pixel> arrTextures[8];
};


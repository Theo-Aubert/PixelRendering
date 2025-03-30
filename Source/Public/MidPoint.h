#pragma once

#include "../Externals/olcPixelGameEngine.h"


class MidPoint : public olc::PixelGameEngine
{

public:
	MidPoint()
	{
		// Name your application
		sAppName = "MidPoint Algorithm";
	}


public:

	bool OnUserCreate() override;

	bool OnUserUpdate(float fElapsedTime) override;

private :

	void DrawGrid();

	void DrawHovered(const std::vector<olc::vi2d>& arrTiles);

	std::vector<olc::vi2d> MidPointAlgo(olc::vi2d vCenter, int radius);

	std::vector<olc::vi2d> MidPointAlgoInt(olc::vi2d vCenter, int radius);

public:

	int iTileSize = 16; //in px

	olc::vi2d vGridSize; //in tile

	olc::vi2d vCircleCenter;
	int radius = 5;
};


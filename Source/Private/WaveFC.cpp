#include "../Public/WaveFC.h"
#include <chrono>
#include <thread>


bool WaveFC::OnUserCreate()
{
	//Set up Modules (gonna be long....)

	//Cross
	Cross.m_pSprite = new olc::Sprite(RESSOURCE_PATH_TILES + "/Cross.png");
	Cross.m_pDecal = new olc::Decal(Cross.m_pSprite);
	Cross.m_connexMap.emplace(EConnect::North, EMaterial::Road);
	Cross.m_connexMap.emplace(EConnect::South, EMaterial::Road);
	Cross.m_connexMap.emplace(EConnect::East, EMaterial::Road);
	Cross.m_connexMap.emplace(EConnect::West, EMaterial::Road);

	//Empty
	Empty.m_pSprite = new olc::Sprite(RESSOURCE_PATH_TILES + "/Empty.png");
	Empty.m_pDecal = new olc::Decal(Empty.m_pSprite);
	Empty.m_connexMap.emplace(EConnect::North, EMaterial::Grass);
	Empty.m_connexMap.emplace(EConnect::South, EMaterial::Grass);
	Empty.m_connexMap.emplace(EConnect::East, EMaterial::Grass);
	Empty.m_connexMap.emplace(EConnect::West, EMaterial::Grass);

	//Lines
	LHorizontal.m_pSprite = new olc::Sprite(RESSOURCE_PATH_TILES + "/LHorizontal.png");
	LHorizontal.m_pDecal = new olc::Decal(LHorizontal.m_pSprite);
	LHorizontal.m_connexMap.emplace(EConnect::North, EMaterial::Grass);
	LHorizontal.m_connexMap.emplace(EConnect::South, EMaterial::Grass);
	LHorizontal.m_connexMap.emplace(EConnect::East, EMaterial::Road);
	LHorizontal.m_connexMap.emplace(EConnect::West, EMaterial::Road);

	LVertical.m_pSprite = new olc::Sprite(RESSOURCE_PATH_TILES + "/LVertical.png");
	LVertical.m_pDecal = new olc::Decal(LVertical.m_pSprite);
	LVertical.m_connexMap.emplace(EConnect::North, EMaterial::Road);
	LVertical.m_connexMap.emplace(EConnect::South, EMaterial::Road);
	LVertical.m_connexMap.emplace(EConnect::East, EMaterial::Grass);
	LVertical.m_connexMap.emplace(EConnect::West, EMaterial::Grass);

	//T-junctions
	TUp.m_pSprite = new olc::Sprite(RESSOURCE_PATH_TILES + "/TUp.png");
	TUp.m_pDecal = new olc::Decal(TUp.m_pSprite);
	TUp.m_connexMap.emplace(EConnect::North, EMaterial::Road);
	TUp.m_connexMap.emplace(EConnect::South, EMaterial::Grass);
	TUp.m_connexMap.emplace(EConnect::East, EMaterial::Road);
	TUp.m_connexMap.emplace(EConnect::West, EMaterial::Road);

	TDown.m_pSprite = new olc::Sprite(RESSOURCE_PATH_TILES + "/TDown.png");
	TDown.m_pDecal = new olc::Decal(TDown.m_pSprite);
	TDown.m_connexMap.emplace(EConnect::North, EMaterial::Grass);
	TDown.m_connexMap.emplace(EConnect::South, EMaterial::Road);
	TDown.m_connexMap.emplace(EConnect::East, EMaterial::Road);
	TDown.m_connexMap.emplace(EConnect::West, EMaterial::Road);

	TLeft.m_pSprite = new olc::Sprite(RESSOURCE_PATH_TILES + "/TLeft.png");
	TLeft.m_pDecal = new olc::Decal(TLeft.m_pSprite);
	TLeft.m_connexMap.emplace(EConnect::North, EMaterial::Road);
	TLeft.m_connexMap.emplace(EConnect::South, EMaterial::Road);
	TLeft.m_connexMap.emplace(EConnect::East, EMaterial::Grass);
	TLeft.m_connexMap.emplace(EConnect::West, EMaterial::Road);

	TRight.m_pSprite = new olc::Sprite(RESSOURCE_PATH_TILES + "/TRight.png");
	TRight.m_pDecal = new olc::Decal(TRight.m_pSprite);
	TRight.m_connexMap.emplace(EConnect::North, EMaterial::Road);
	TRight.m_connexMap.emplace(EConnect::South, EMaterial::Road);
	TRight.m_connexMap.emplace(EConnect::East, EMaterial::Road);
	TRight.m_connexMap.emplace(EConnect::West, EMaterial::Grass);

	//Corners
	CLeftUp.m_pSprite = new olc::Sprite(RESSOURCE_PATH_TILES + "/CLeftUp.png");
	CLeftUp.m_pDecal = new olc::Decal(CLeftUp.m_pSprite);
	CLeftUp.m_connexMap.emplace(EConnect::North, EMaterial::Road);
	CLeftUp.m_connexMap.emplace(EConnect::South, EMaterial::Grass);
	CLeftUp.m_connexMap.emplace(EConnect::East, EMaterial::Grass);
	CLeftUp.m_connexMap.emplace(EConnect::West, EMaterial::Road);

	CRightUp.m_pSprite = new olc::Sprite(RESSOURCE_PATH_TILES + "/CRightUp.png");
	CRightUp.m_pDecal = new olc::Decal(CRightUp.m_pSprite);
	CRightUp.m_connexMap.emplace(EConnect::North, EMaterial::Road);
	CRightUp.m_connexMap.emplace(EConnect::South, EMaterial::Grass);
	CRightUp.m_connexMap.emplace(EConnect::East, EMaterial::Road);
	CRightUp.m_connexMap.emplace(EConnect::West, EMaterial::Grass);

	CLeftDown.m_pSprite = new olc::Sprite(RESSOURCE_PATH_TILES + "/CLeftDown.png");
	CLeftDown.m_pDecal = new olc::Decal(CLeftDown.m_pSprite);
	CLeftDown.m_connexMap.emplace(EConnect::North, EMaterial::Grass);
	CLeftDown.m_connexMap.emplace(EConnect::South, EMaterial::Road);
	CLeftDown.m_connexMap.emplace(EConnect::East, EMaterial::Grass);
	CLeftDown.m_connexMap.emplace(EConnect::West, EMaterial::Road);

	CRightDown.m_pSprite = new olc::Sprite(RESSOURCE_PATH_TILES + "/CRightDown.png");
	CRightDown.m_pDecal = new olc::Decal(CRightDown.m_pSprite);
	CRightDown.m_connexMap.emplace(EConnect::North, EMaterial::Grass);
	CRightDown.m_connexMap.emplace(EConnect::South, EMaterial::Road);
	CRightDown.m_connexMap.emplace(EConnect::East, EMaterial::Road);
	CRightDown.m_connexMap.emplace(EConnect::West, EMaterial::Grass);

	std::srand(std::time(nullptr));

	InitGrid();

	return true;
}

bool WaveFC::OnUserUpdate(float fElapsedTime)
{

	Clear(olc::BLACK);

	if (GetKey(olc::ESCAPE).bReleased)
	{
		return false;
	}

	if (GetKey(olc::R).bReleased)
	{
		InitGrid();
	}

	if (GetKey(olc::SPACE).bReleased)
	{
		bIsStarted = !bIsStarted;
	}

	DrawEmptyGrid();
	DrawTiles();

	DrawModules();

	if (bIsStarted)
	{
		olc::vi2d vNextTile = GetLowestEntropy();
		if (vNextTile != olc::vi2d(-1, -1))
			CollapseTile(GetLowestEntropy());
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	return true;
}

void WaveFC::InitGrid()
{
	std::set<SimpleTile*> tilesSet
	{
		&Cross,
		&Empty,
		&LHorizontal,
		&LVertical,
		&TUp,
		&TDown,
		&TRight,
		&TLeft,
		&CLeftDown,
		&CLeftUp,
		&CRightDown,
		&CRightUp
	};

	m_vModuleGrid.reserve(m_vGridSize.x);
	for(int i = 0; i < m_vGridSize.x; ++i)
	{
		std::vector<std::set<SimpleTile*>> vLine;
		vLine.reserve((m_vGridSize.y));
		for(int j = 0; j < m_vGridSize.y; ++j)
		{
			 
			vLine.emplace_back(tilesSet);
		}
		m_vModuleGrid.emplace_back(vLine);
	}
}

bool WaveFC::AreCoordInBounds(const olc::vi2d& coord)
{
	return AreCoordInBounds(coord.x, coord.y);
}

bool WaveFC::AreCoordInBounds(int x, int y)
{
	return !(x < 0 || x >= m_vGridSize.x || y < 0 || y >= m_vGridSize.y);
}

size_t WaveFC::GetEntropy(const olc::vi2d& coord)
{
	return GetEntropy(coord.x, coord.y);
}

size_t WaveFC::GetEntropy(int x, int y)
{
	if(!AreCoordInBounds(x,y))
	{
		//std::cout << "OOB\n";
		return 0; //Specific value for OOB request. Minimum value for valid coordinates is 1.
	}

	return m_vModuleGrid[x][y].size();
}

olc::vi2d WaveFC::GetLowestEntropy()
{
	olc::vi2d vResult;
	size_t iMinEntropy = 13; //max entropy value at init state is 12
	std::vector<olc::vi2d> vPossibilities;

	for (int i = 0; i < m_vGridSize.x; ++i)
	{
		for (int j = 0; j < m_vGridSize.y; ++j)
		{
			if (IsTileCollapsed(i, j))
			{
				continue;
			}

			int iCurrentEntropy = GetEntropy(i, j);

			if (iCurrentEntropy <= 1)
			{
				continue;
			}

			if (iCurrentEntropy < iMinEntropy)
			{
				vPossibilities.clear();
				vPossibilities.emplace_back(i, j);
				iMinEntropy = iCurrentEntropy;
			}
			else if(iCurrentEntropy == iMinEntropy)
			{
				vPossibilities.emplace_back(i, j);
			}
		}
	}
	if (vPossibilities.size())
	{
		int randidx = std::rand() % vPossibilities.size();
		return vPossibilities[randidx];

	}
	else
	{
		return olc::vi2d(-1, -1);
	}
}

bool WaveFC::CollapseTile(const olc::vi2d& coord)
{
	return CollapseTile(coord.x, coord.y);
}

bool WaveFC::CollapseTile(int x, int y)
{
	if (!AreCoordInBounds(x, y))
	{
		return false;
	}

	if (!IsTileCollapsed(x, y))
	{
		auto r = std::rand() % m_vModuleGrid[x][y].size();
		SimpleTile* pCollapsedTile = *SelectRandom(m_vModuleGrid[x][y], r);
		m_vModuleGrid[x][y].clear();
		m_vModuleGrid[x][y].insert(pCollapsedTile);
	}

	Propagate(x, y);
	

}

void WaveFC::Propagate(const olc::vi2d& coord)
{
	Propagate(coord.x, coord.y);
}

void WaveFC::Propagate(int x, int y)
{
	//If not collapsed or out of bounds, ignore 
	if(!IsTileCollapsed(x,y))
	{
		return;
	}

	//get the one and only tile available
	SimpleTile* pCollapsedTile = *m_vModuleGrid[x][y].begin();

	if (!pCollapsedTile)
	{
		return; //Should never happen
	}

	//Iterate through North/South/West/East neighbours and reduce configuration numbers
	int n_x = x;
	int n_y = y;

	auto reduce = [](SimpleTile* pTile, EConnect eDir, std::set<SimpleTile*>& setToReduce)
		{
			std::vector<SimpleTile*> arrToRemove;

			for (auto const& tile : setToReduce)
			{
				switch (eDir)
				{
				case North:
					if (pTile->m_connexMap[eDir] != tile->m_connexMap[South])
					{
						arrToRemove.push_back(tile);
					}
					break;
				case South:
					if (pTile->m_connexMap[eDir] != tile->m_connexMap[North])
					{
						arrToRemove.push_back(tile);
					}
					break;
				case West:
					if (pTile->m_connexMap[eDir] != tile->m_connexMap[East])
					{
						arrToRemove.push_back(tile);
					}
					break;
				case East:
					if (pTile->m_connexMap[eDir] != tile->m_connexMap[West])
					{
						arrToRemove.push_back(tile);
					}
					break;
				default:
					break;
				}
			}

			for (auto const& tileToRemove : arrToRemove)
			{
				setToReduce.erase(tileToRemove);
			}
		};

	//Raw iteration throug neighbors here, a smart loop would be accurate for generalization purposes

	//North
	if(AreCoordInBounds(x, y - 1) && !IsTileCollapsed(x,y-1))
		reduce(pCollapsedTile, North, m_vModuleGrid[x][y -1]);

	//South
	if (AreCoordInBounds(x, y + 1) && !IsTileCollapsed(x, y + 1))
		reduce(pCollapsedTile, South, m_vModuleGrid[x][y + 1]);

	//West
	if (AreCoordInBounds(x - 1, y) && !IsTileCollapsed(x - 1, y))
		reduce(pCollapsedTile, West, m_vModuleGrid[x - 1][y]);

	//East
	if (AreCoordInBounds(x + 1, y) && !IsTileCollapsed(x + 1,y))
		reduce(pCollapsedTile, East, m_vModuleGrid[x + 1][y]);
	
}

bool WaveFC::IsTileCollapsed(const olc::vi2d& coord)
{

	return IsTileCollapsed(coord.x, coord.y);
}

bool WaveFC::IsTileCollapsed(int x, int y)
{
	if(x < 0 || x >= m_vGridSize.x || y < 0 || y >= m_vGridSize.y)
	{
		return false; //convenience to return false here so we do not perform OOB test twice
	}
	
	return m_vModuleGrid[x][y].size() == 1;
}

void WaveFC::DrawTiles()
{
	olc::vi2d vTLCorner = { int((ScreenWidth() - m_vGridSize.x * m_iTileSize) / 2.),int((ScreenHeight() - m_vGridSize.y * m_iTileSize) / 2.) };
	olc::vi2d vTileCorner;

	for (int i = 0; i < m_vGridSize.x; ++i)
	{
		for (int j = 0; j < m_vGridSize.y; ++j)
		{
			vTileCorner = vTLCorner + olc::vi2d(i * m_iTileSize, j * m_iTileSize);

			if (!IsTileCollapsed(i, j))
			{
				//DrawString(vTileCorner, "x:" + std::to_string(i) + "| y:" + std::to_string(j));
				DrawString(vTileCorner + olc::vi2d(0.3 * m_iTileSize, 0.3 * m_iTileSize), std::to_string(GetEntropy(i, j)));
				continue;
			}

			vTileCorner = vTLCorner + olc::vi2d(i * m_iTileSize, j * m_iTileSize);

			SimpleTile* pCollapsedTile = *m_vModuleGrid[i][j].begin();
			DrawDecal(vTileCorner, pCollapsedTile->m_pDecal, {3.,3.});
		}
	}
}

void WaveFC::DrawEmptyGrid()
{
	//<<<<<<<<<<<<<<<< Grid Drawing >>>>>>>>>>>>>>>>>>>//
	// 
	//construct bottom left corner where we start drawing
	olc::vi2d vBLCorner = { int((ScreenWidth() - m_vGridSize.x * m_iTileSize) / 2.),int((ScreenHeight() + m_vGridSize.y * m_iTileSize) / 2.) };

	//pattern
	for (int i = 0; i <= m_vGridSize.x; ++i)
	{
		olc::vi2d vStart = vBLCorner;
		olc::vi2d vEnd;

		vStart.x += m_iTileSize * i;
		vEnd = vStart;
		vEnd.y -= m_iTileSize * m_vGridSize.y;

		DrawLine(vStart, vEnd);
	}
	for (int j = 0; j <= m_vGridSize.y; ++j)
	{
		olc::vi2d vStart = vBLCorner;
		olc::vi2d vEnd;

		vStart.y -= m_iTileSize * j;
		vEnd = vStart;

		vEnd.x += m_iTileSize * m_vGridSize.x;

		DrawLine(vStart, vEnd);

	}
}

void WaveFC::DrawModules()
{
	olc::vi2d vBLCorner = { int((ScreenWidth() - m_vGridSize.x * m_iTileSize) / 2.),int((ScreenHeight() + m_vGridSize.y * m_iTileSize) / 2.) };

	vBLCorner.x -= m_iTileSize * 2;

	vBLCorner.y -= m_iTileSize + 5;

	DrawDecal(vBLCorner, Cross.m_pDecal, { 3.,3. });
	DrawModuleConnex(Cross, vBLCorner);

	vBLCorner.y -= m_iTileSize + 5 ;

	DrawDecal(vBLCorner, Empty.m_pDecal, { 3.,3. });
	DrawModuleConnex(Empty, vBLCorner);

	vBLCorner.y -= m_iTileSize + 5;

	DrawDecal(vBLCorner, LHorizontal.m_pDecal, { 3.,3. });
	DrawModuleConnex(LHorizontal, vBLCorner);

	vBLCorner.y -= m_iTileSize + 5;

	DrawDecal(vBLCorner, LVertical.m_pDecal, { 3.,3. });
	DrawModuleConnex(LVertical, vBLCorner);

	vBLCorner.y -= m_iTileSize + 5;

	DrawDecal(vBLCorner, TUp.m_pDecal, { 3.,3. });
	DrawModuleConnex(TUp, vBLCorner);

	vBLCorner.y -= m_iTileSize + 5;

	DrawDecal(vBLCorner, TDown.m_pDecal, { 3.,3. });
	DrawModuleConnex(TDown, vBLCorner);

	vBLCorner.y -= m_iTileSize + 5;

	DrawDecal(vBLCorner, TLeft.m_pDecal, { 3.,3. });
	DrawModuleConnex(TLeft, vBLCorner);

	vBLCorner.y -= m_iTileSize + 5;

	DrawDecal(vBLCorner, TRight.m_pDecal, { 3.,3. });
	DrawModuleConnex(TRight, vBLCorner);

	vBLCorner.y -= m_iTileSize + 5;

	DrawDecal(vBLCorner, CLeftUp.m_pDecal, { 3.,3. });
	DrawModuleConnex(CLeftUp, vBLCorner);

	vBLCorner.y -= m_iTileSize + 5;

	DrawDecal(vBLCorner, CLeftDown.m_pDecal, { 3.,3. });
	DrawModuleConnex(CLeftDown, vBLCorner);

	vBLCorner.y -= m_iTileSize + 5;

	DrawDecal(vBLCorner, CRightUp.m_pDecal, { 3.,3. });
	DrawModuleConnex(CRightUp, vBLCorner);

	vBLCorner.y -= m_iTileSize + 5;

	DrawDecal(vBLCorner, CRightDown.m_pDecal, { 3.,3. });
	DrawModuleConnex(CRightDown, vBLCorner);
}

void WaveFC::DrawModuleConnex(SimpleTile& Tile, olc::vi2d& pos)
{
	if (Tile.m_connexMap[EConnect::North] == EMaterial::Road)
	{
		DrawLine(pos + olc::vi2d(-1, -1), pos + olc::vi2d(m_iTileSize, -1), olc::RED);
	}

	if (Tile.m_connexMap[EConnect::South] == EMaterial::Road)
	{
		DrawLine(pos + olc::vi2d(0, m_iTileSize), pos + olc::vi2d(m_iTileSize,m_iTileSize), olc::BLUE);
	}

	if (Tile.m_connexMap[EConnect::West] == EMaterial::Road)
	{
		DrawLine(pos + olc::vi2d(-1, 0), pos + olc::vi2d(-1, m_iTileSize), olc::YELLOW);
	}

	if (Tile.m_connexMap[EConnect::East] == EMaterial::Road)
	{
		DrawLine(pos + olc::vi2d(m_iTileSize, 0), pos + olc::vi2d(m_iTileSize, m_iTileSize), olc::CYAN);
	}
}



#include "../Public/TAUDoom.h"

bool TAUDoom::OnUserCreate()
{
    olc_UpdateMouseFocus(true);

    mCellWorld = new sCell[mapWidth * mapHeight];
    ComputeShadowMap();
    Setup2DMap();

    v3DMainAnchor = { 0, 0 };
    v3DMainResolution = { ScreenWidth() - 1, ScreenHeight() -1 };
    v3DMiniResolution = { 320, 180 };
    v3DMiniAnchor = v3DMainResolution - (v3DMiniResolution + olc::vi2d(1,1));


    int iSmallestSide = std::min(ScreenWidth(), ScreenHeight());
    i2DMainScale = iSmallestSide / mapHeight;
    v2DMainResolution = { 960 , 960 };
    v2DMainAnchor = {(ScreenWidth() - iSmallestSide) / 2, 0};
    v2DMiniResolution = {480, 480};
    v2DMiniAnchor = v3DMainResolution - (v2DMiniResolution + olc::vi2d(1, 1));

	return true;
}

bool TAUDoom::OnUserUpdate(float fElapsedTime)
{
	//Quit app
	if (GetKey(olc::ESCAPE).bReleased) return false;

    if (GetKey(olc::F1).bReleased) m_bIs3DMainViewport = !m_bIs3DMainViewport;
    if (GetKey(olc::F2).bReleased) m_bShowMinimapSprite = !m_bShowMinimapSprite;

	//Basic commands
	if (GetKey(olc::Z).bHeld) player.vPosition += player.vLookAt * player.dSpeed * fElapsedTime;
	if (GetKey(olc::S).bHeld) player.vPosition -= player.vLookAt * player.dSpeed * fElapsedTime;
    if (GetKey(olc::Q).bHeld) player.vPosition -= player.GetRightVector() * player.dSpeed * fElapsedTime;
	if (GetKey(olc::D).bHeld) player.vPosition += player.GetRightVector() * player.dSpeed * fElapsedTime;


    if (GetKey(olc::RIGHT).bHeld)
    {
        player.dLookAtAngle -= player.dRotateSpeed * fElapsedTime;
        player.vLookAt.x = cos(player.dLookAtAngle);
        player.vLookAt.y = sin(player.dLookAtAngle);
    }
    if (GetKey(olc::LEFT).bHeld)
    {
        player.dLookAtAngle += player.dRotateSpeed * fElapsedTime;
        player.vLookAt.x = cos(player.dLookAtAngle);
        player.vLookAt.y = sin(player.dLookAtAngle);
    }

    if(GetKey(olc::UP).bReleased)
        m_dShadowBias += 0.5;

    if(GetKey(olc::DOWN).bReleased)
        m_dShadowBias -= 0.5;

    m_dShadowBias = std::clamp(m_dShadowBias, 0., 15.);

	Clear(olc::BLACK);

    //DrawSprite(0,0, pShadowMap);
    //return true;

    if (m_bIs3DMainViewport)
    {
        RenderDoomMap(v3DMainAnchor, v3DMainResolution);
        Render2DMap(v2DMiniAnchor, v2DMiniResolution, m_bShowMinimapSprite);
        //RenderShadowMap(v2DMiniAnchor, v2DMiniResolution, m_bShowMinimapSprite);
    }
    else
    {
        RenderDoomMap(v3DMiniAnchor, v3DMiniResolution);
        //RenderShadowMap(v2DMainAnchor, v2DMainResolution, m_bShowMinimapSprite);
        Render2DMap(v2DMainAnchor, v2DMainResolution, m_bShowMinimapSprite);
    }

    
	return true;
}

void TAUDoom::Setup2DMap()
{
    pMiniMapSprite = new olc::Sprite(mapWidth, mapWidth);

    SetDrawTarget(pMiniMapSprite); //draw on sprite
    Clear(olc::BLACK);

    for (int i = 0; i < mapWidth; i++)
    {
        for (int j = 0; j < mapHeight; j++)
        {

            //choose wall color
            olc::Pixel color;
            switch (worldMap[i][j])
            {
            case 0:  color = olc::DARK_GREY;  break; //grey
            case 1:  color = olc::RED;    break; //red
            case 2:  color = olc::GREEN;  break; //green
            case 3:  color = olc::BLUE;   break; //blue
            case 4:  color = olc::WHITE;  break; //white
            default: color = olc::YELLOW; break; //yellow
            }
            
            FillRect(
                olc::vi2d(j, i),
                olc::vi2d(1,1),
                color
             );
        }
    }

    
    SetDrawTarget(nullptr); //release sprite writing 

    pMiniMapDecal = new olc::Decal(pMiniMapSprite);
}

void TAUDoom::ComputeShadowMap()
{
    pShadowMap = new olc::Sprite(ShadowMapWidth, ShadowMapHeight);
    SetDrawTarget(pShadowMap);
    Clear(olc::BLACK);
    
    std::vector<sEdge> vEdges;
    std::vector<std::tuple<double, double, double>> vecVisibilityPolygonPoints;

    ConvertTileMapToPolyMap({ 0,0 }, { mapWidth, mapHeight }, ShadowMapWidth / mapWidth,mapWidth, vEdges);

    // for (auto& e : vEdges)
    // {
    //     DrawLine(e.start, e.end, olc::RED);
    //     FillCircle(e.start, 3, olc::RED);
    //     FillCircle(e.end, 3, olc::RED);
    // }
    
    CalculateVisibilityPolygon(lightPos, 1000., vEdges, vecVisibilityPolygonPoints);

    for (int i = 0; i < vecVisibilityPolygonPoints.size() - 1; i++)
    {
        FillTriangle(lightPos, 
            olc::vi2d(get<1>(vecVisibilityPolygonPoints[i]), get<2>(vecVisibilityPolygonPoints[i])), 
            olc::vi2d(get<1>(vecVisibilityPolygonPoints[i +1]), get<2>(vecVisibilityPolygonPoints[i +1])));
    }

    FillTriangle(lightPos, 
        olc::vi2d(get<1>(vecVisibilityPolygonPoints[vecVisibilityPolygonPoints.size() - 1]), get<2>(vecVisibilityPolygonPoints[vecVisibilityPolygonPoints.size() - 1])), 
        olc::vi2d(get<1>(vecVisibilityPolygonPoints[0]), get<2>(vecVisibilityPolygonPoints[0])));

    //FillCircle(lightPos, 5, olc::DARK_YELLOW);

    //clear front faces to avoid shadow acne
     for(int x = 0; x < mapWidth; x++)
         for(int y = 0; y < mapHeight; y++)
         {
             if(worldMap[x][y] > 0)
             {
                 FillRect(y * ShadowMapHeight / mapHeight,x * ShadowMapWidth / mapWidth,  ShadowMapHeight / mapHeight, ShadowMapWidth / mapWidth);
             }
         }

    SetDrawTarget(nullptr); //release sprite writing 
}

void TAUDoom::ConvertTileMapToPolyMap(const olc::vi2d& pos, const olc::vi2d& size, double dBlockWidth, int pitch, std::vector<sEdge>& outEdges)
{
    outEdges.clear();

    for (int x = 0; x < size.x; x++)
    {
        for (int y = 0; y < size.y; y++)
        {
            mCellWorld[(y + pos.y) * pitch + (x + pos.x)].bExist = worldMap[(y + pos.y)][(x + pos.x)] > 0;

            for (int j = 0; j < 4; j++)
            {
                
                mCellWorld[(y + pos.y) * pitch + (x + pos.x)].bEdgeExist[j] = false;
                mCellWorld[(y + pos.y) * pitch + (x + pos.x)].edgeID[j] = 0;
            }
        }
    }

    for (int x = 0; x < size.x ; x++)
    {
        for (int y = 0; y < size.y; y++)
        {
            int i = (y + pos.y) * pitch + (x + pos.x); //current cell index
            int n = (y + pos.y - 1) * pitch + (x + pos.x); //northern neighbour
            int s = (y + pos.y + 1) * pitch + (x + pos.x); //southern neighbour
            int w = (y + pos.y) * pitch + (x + pos.x -1); //western neighbour
            int e = (y + pos.y) * pitch + (x + pos.x +1); //eastern neighbour

            //If the cell exists, check if it needs edges
            if (mCellWorld[i].bExist)
            {
                if(x == 0)
                {
                    //create new edge
                    sEdge edge;
                    edge.start  = {(pos.x + x) * dBlockWidth, (pos.y + y) * dBlockWidth };
                    edge.end    = { edge.start.x, edge.start.y + dBlockWidth };


                    //Add it to polygon pool
                    int edge_id = outEdges.size();
                    outEdges.push_back(edge);

                    mCellWorld[i].edgeID[CardinalPoints::NWest] = edge_id;
                    mCellWorld[i].bEdgeExist[CardinalPoints::NWest] = true;
                }
                else
                {
                    //if the cell has no western neighbour, create a western edge
                    if (!mCellWorld[w].bExist)
                    {
                        //extend the one from northern neighbour, or create a new one
                        if (mCellWorld[n].bEdgeExist[CardinalPoints::NWest])
                        {
                            //Northern neighbour has a western edge, so extend it downward
                            outEdges[mCellWorld[n].edgeID[CardinalPoints::NWest]].end.y += dBlockWidth;
                            mCellWorld[i].edgeID[CardinalPoints::NWest] = mCellWorld[n].edgeID[CardinalPoints::NWest];
                            mCellWorld[i].bEdgeExist[CardinalPoints::NWest] = true;
                        }
                        else
                        {
                            //create new edge
                            sEdge edge;
                            edge.start  = {(pos.x + x) * dBlockWidth, (pos.y + y) * dBlockWidth };
                            edge.end    = { edge.start.x, edge.start.y + dBlockWidth };


                            //Add it to polygon pool
                            int edge_id = outEdges.size();
                            outEdges.push_back(edge);

                            mCellWorld[i].edgeID[CardinalPoints::NWest] = edge_id;
                            mCellWorld[i].bEdgeExist[CardinalPoints::NWest] = true;

                        }
                    }
                }

                if(x == size.x -1)
                {
                    //create new edge
                    sEdge edge;
                    edge.start = { (pos.x + x + 1) * dBlockWidth, (pos.y + y) * dBlockWidth };
                    edge.end = { edge.start.x, edge.start.y + dBlockWidth };


                    //Add it to polygon pool
                    int edge_id = outEdges.size();
                    outEdges.push_back(edge);

                    mCellWorld[i].edgeID[CardinalPoints::NEast] = edge_id;
                    mCellWorld[i].bEdgeExist[CardinalPoints::NEast] = true;
                }
                else
                {
                    //if the cell has no eastern neighbour, create a eastern edge
                    if (!mCellWorld[e].bExist)
                    {
                        //extend the one from northern neighbour, or create a new one
                        if (mCellWorld[n].bEdgeExist[CardinalPoints::NEast])
                        {
                            //Northern neighbour has a western edge, so extend it downward
                            outEdges[mCellWorld[n].edgeID[CardinalPoints::NEast]].end.y += dBlockWidth;
                            mCellWorld[i].edgeID[CardinalPoints::NEast] = mCellWorld[n].edgeID[CardinalPoints::NEast];
                            mCellWorld[i].bEdgeExist[CardinalPoints::NEast] = true;
                        }
                        else
                        {
                            //create new edge
                            sEdge edge;
                            edge.start = { (pos.x + x + 1) * dBlockWidth, (pos.y + y) * dBlockWidth };
                            edge.end = { edge.start.x, edge.start.y + dBlockWidth };


                            //Add it to polygon pool
                            int edge_id = outEdges.size();
                            outEdges.push_back(edge);

                            mCellWorld[i].edgeID[CardinalPoints::NEast] = edge_id;
                            mCellWorld[i].bEdgeExist[CardinalPoints::NEast] = true;

                        }
                    }
                }
                
                if(y == 0)
                {
                    //create new edge
                    sEdge edge;
                    edge.start = { (pos.x + x) * dBlockWidth, (pos.y + y) * dBlockWidth };
                    edge.end = { edge.start.x + dBlockWidth, edge.start.y };


                    //Add it to polygon pool
                    int edge_id = outEdges.size();
                    outEdges.push_back(edge);

                    mCellWorld[i].edgeID[CardinalPoints::NNorth] = edge_id;
                    mCellWorld[i].bEdgeExist[CardinalPoints::NNorth] = true;
                }
                else
                {
                    //if the cell has no western neighbour, create a western edge
                    if (!mCellWorld[n].bExist)
                    {
                        //extend the one from northern neighbour, or create a new one
                        if (mCellWorld[w].bEdgeExist[CardinalPoints::NNorth])
                        {
                            //Northern neighbour has a western edge, so extend it downward
                            outEdges[mCellWorld[w].edgeID[CardinalPoints::NNorth]].end.x += dBlockWidth;
                            mCellWorld[i].edgeID[CardinalPoints::NNorth] = mCellWorld[w].edgeID[CardinalPoints::NNorth];
                            mCellWorld[i].bEdgeExist[CardinalPoints::NNorth] = true;
                        }
                        else
                        {
                            //create new edge
                            sEdge edge;
                            edge.start = { (pos.x + x) * dBlockWidth, (pos.y + y) * dBlockWidth };
                            edge.end = { edge.start.x + dBlockWidth, edge.start.y };


                            //Add it to polygon pool
                            int edge_id = outEdges.size();
                            outEdges.push_back(edge);

                            mCellWorld[i].edgeID[CardinalPoints::NNorth] = edge_id;
                            mCellWorld[i].bEdgeExist[CardinalPoints::NNorth] = true;

                        }
                    }
                }
               
                if(y == size.y -1)
                {
                    //create new edge
                    sEdge edge;
                    edge.start = { (pos.x + x) * dBlockWidth, (pos.y + y + 1) * dBlockWidth };
                    edge.end = { edge.start.x + dBlockWidth, edge.start.y};


                    //Add it to polygon pool
                    int edge_id = outEdges.size();
                    outEdges.push_back(edge);

                    mCellWorld[i].edgeID[CardinalPoints::NSouth] = edge_id;
                    mCellWorld[i].bEdgeExist[CardinalPoints::NSouth] = true;
                }
                else
                {
                    //if the cell has no western neighbour, create a western edge
                    if (!mCellWorld[s].bExist)
                    {
                        //extend the one from northern neighbour, or create a new one
                        if (mCellWorld[w].bEdgeExist[CardinalPoints::NSouth])
                        {
                            //Northern neighbour has a western edge, so extend it downward
                            outEdges[mCellWorld[w].edgeID[CardinalPoints::NSouth]].end.x += dBlockWidth;
                            mCellWorld[i].edgeID[CardinalPoints::NSouth] = mCellWorld[w].edgeID[CardinalPoints::NSouth];
                            mCellWorld[i].bEdgeExist[CardinalPoints::NSouth] = true;
                        }
                        else
                        {
                            //create new edge
                            sEdge edge;
                            edge.start = { (pos.x + x) * dBlockWidth, (pos.y + y + 1) * dBlockWidth };
                            edge.end = { edge.start.x + dBlockWidth, edge.start.y};


                            //Add it to polygon pool
                            int edge_id = outEdges.size();
                            outEdges.push_back(edge);

                            mCellWorld[i].edgeID[CardinalPoints::NSouth] = edge_id;
                            mCellWorld[i].bEdgeExist[CardinalPoints::NSouth] = true;

                        }
                    }
                }
                
            }
        }
    }
}

void TAUDoom::CalculateVisibilityPolygon(olc::vd2d pos, double dRadius, const std::vector<sEdge>& Edges, std::vector<std::tuple<double, double, double>>& outVisibilityPolygonPoints)
{
    outVisibilityPolygonPoints.clear();

    for (auto& e : Edges)
    {
        for (int i = 0; i < 2; i++)
        {
            double rdx, rdy;
            rdx = (i == 0 ? e.start.x : e.end.x) - pos.x;
            rdy = (i == 0 ? e.start.y : e.end.y) - pos.y;

            double base_ang = atan2(rdy, rdx);
            double ang = 0;

            for (int j = 0; j < 3; j++)
            {
                if (j == 0) ang = base_ang - 0.0001;
                if (j == 1) ang = base_ang;
                if (j == 2) ang = base_ang + 0.0001;

                rdx = dRadius * cos(ang);
                rdy = dRadius * sin(ang);

                double min_t1 = INFINITY;
                double min_px = 0, min_py = 0, min_ang = 0;
                bool bValid = false;


                for (auto& e2 : Edges)
                {
                    double sdx = e2.end.x - e2.start.x;
                    double sdy = e2.end.y - e2.start.y;

                    if (abs(sdx - rdx) > 0. && abs(sdy - rdy) > 0.)
                    {
                        // t2 is normalised distance from line segment start to line segment end of intersect point
                        double t2 = (rdx * (e2.start.y - pos.y) + (rdy * (pos.x - e2.start.x))) / (sdx * rdy - sdy * rdx);
                        // t1 is normalised distance from source along ray to ray length of intersect point
                        double t1 = (e2.start.x + sdx * t2 - pos.x) / rdx;

                        // If intersect point exists along ray, and along line 
                        // segment then intersect point is valid
                        if (t1 > 0 && t2 >= 0 && t2 <= 1.0f)
                        {
                            // Check if this intersect point is closest to source. If
                            // it is, then store this point and reject others
                            if (t1 < min_t1)
                            {
                                min_t1 = t1;
                                min_px = pos.x + rdx * t1;
                                min_py = pos.y + rdy * t1;
                                min_ang = atan2f(min_py - pos.y, min_px - pos.x);
                                bValid = true;
                            }
                        }
                    }

                }

                if (bValid)// Add intersection point to visibility polygon perimeter
                    outVisibilityPolygonPoints.push_back({ min_ang, min_px, min_py });
            }
            
        }
    }

    // Sort perimeter points by angle from source. This will allow
    // us to draw a triangle fan.
    std::sort(
        outVisibilityPolygonPoints.begin(),
        outVisibilityPolygonPoints.end(),
        [&](const std::tuple<float, float, float>& t1, const std::tuple<float, float, float>& t2)
        {
            return get<0>(t1) < get<0>(t2);
        });

}

void TAUDoom::DrawPlayer()
{

    
    
}

void TAUDoom::Render2DMap(const olc::vi2d& vPos, const olc::vi2d& vResolution, bool bShowMapSprite)
{
    
    olc::vi2d mapScaleFactor(double(vResolution.x) / mapWidth, double(vResolution.y) / mapHeight);
    olc::vi2d scaleFactor = mapScaleFactor;
    olc::vd2d projFactor = scaleFactor;

    if(bShowMapSprite)
        DrawSprite(vPos, pMiniMapSprite, scaleFactor.y);
    else
    {
        scaleFactor = olc::vi2d(double(vResolution.x) / ShadowMapWidth, double(vResolution.y) / ShadowMapHeight);
        DrawSprite(vPos, pShadowMap, scaleFactor.y);
        projFactor *= olc::vd2d( double(ShadowMapWidth) / mapWidth, double(ShadowMapHeight)/mapHeight);
    }
    DrawRect(vPos, vResolution);

    for (int i = 0; i < mapWidth; i++)
    {
        DrawLine(olc::vi2d(vPos.x + i * mapScaleFactor.x, vPos.y), olc::vi2d(vPos.x + i * mapScaleFactor.x, vPos.y + vResolution.y));
    }

    for (int j = 0; j < mapWidth; j++)
    {
        DrawLine(olc::vi2d(vPos.x,  vPos.y + j *mapScaleFactor.y), olc::vi2d(vPos.x + vResolution.x, vPos.y + j *mapScaleFactor.y));
    }

    //Player pos
    olc::vi2d vProjectedTranslation( player.vPosition.y * mapScaleFactor.y, player.vPosition.x * mapScaleFactor.x );
    olc::vi2d vProjectedLoc = vPos + vProjectedTranslation;
    FillCircle(vProjectedLoc, mapScaleFactor.x /2, olc::CYAN);

    //Sight lines
    DrawLine(vProjectedLoc, vProjectedLoc + olc::vf2d(sin(player.dLookAtAngle - player.dFoV / 2), cos(player.dLookAtAngle - player.dFoV / 2)) * mapScaleFactor.x * 2, olc::DARK_MAGENTA);
    DrawLine(vProjectedLoc, vProjectedLoc + olc::vf2d(sin(player.dLookAtAngle + player.dFoV / 2), cos(player.dLookAtAngle + player.dFoV / 2)) * mapScaleFactor.x * 2, olc::DARK_MAGENTA);

    olc::vi2d vMapPos = player.vPosition;
    DrawRect(vPos + olc::vi2d(vMapPos.y * projFactor.x, vMapPos.x * projFactor.y), projFactor, olc::BLACK);

    for (auto point : sHitPoints)
    {
        FillCircle(vPos + olc::vi2d(int(point.y * mapScaleFactor.x), int(point.x * mapScaleFactor.y)), mapScaleFactor.x / 8, olc::DARK_YELLOW);
    }

}

void TAUDoom::RenderShadowMap(const olc::vi2d& vPos, const olc::vi2d& vResolution, bool bShowMapSprite)
{
    olc::vi2d scaleFactor(double(vResolution.x) / ShadowMapWidth, double(vResolution.y) / ShadowMapHeight);

    if(bShowMapSprite)
        DrawSprite(vPos, pShadowMap, scaleFactor.x);
    DrawRect(vPos, vResolution);

    //Player pos
    olc::vi2d vProjectedTranslation( player.vPosition.y/ mapHeight *  ShadowMapHeight * scaleFactor.x, player.vPosition.x/mapWidth * ShadowMapWidth * scaleFactor.y);
    olc::vi2d vProjectedLoc = vPos + vProjectedTranslation;
    FillCircle(vProjectedLoc, scaleFactor.x *  mapWidth /2, olc::CYAN);

    //Sight lines
    DrawLine(vProjectedLoc, vProjectedLoc + olc::vf2d(sin(player.dLookAtAngle - player.dFoV / 2), cos(player.dLookAtAngle - player.dFoV / 2)) * scaleFactor.x * (double(ShadowMapWidth) / mapWidth) * 2, olc::DARK_MAGENTA);
    DrawLine(vProjectedLoc, vProjectedLoc + olc::vf2d(sin(player.dLookAtAngle + player.dFoV / 2), cos(player.dLookAtAngle + player.dFoV / 2)) * scaleFactor.x * (double(ShadowMapWidth) / mapWidth) * 2, olc::DARK_MAGENTA);

    olc::vi2d vMapPos = player.vPosition * scaleFactor * (double(ShadowMapWidth) / mapWidth);
    DrawRect(vPos + olc::vi2d(vMapPos.y, vMapPos.x), scaleFactor, olc::RED);

    for (auto point : sHitPoints)
    {
        FillCircle(vPos + olc::vi2d(int(point.y / mapHeight *  ShadowMapHeight * scaleFactor.x), int(point.x/mapWidth * ShadowMapWidth * scaleFactor.y)), 3, olc::DARK_YELLOW);
    }
}

void TAUDoom::RenderDoomMap(const olc::vi2d& vPos, const olc::vi2d& vResolution)
{
    DrawRect(vPos, vResolution, olc::CYAN);
    //return;
    //return true;
    int iDebugDisplayStep = vResolution.x / m_iDebugRays;
    sHitPoints.clear();
    
    //floor casting
    for(int y = vResolution.y /2 + 1; y < vResolution.y; y++)
    {
        // rayDir for leftmost ray (x = 0) and rightmost ray (x = w)
        olc::vd2d vRayDir0;
        vRayDir0.x = cos(player.dLookAtAngle + (player.dFoV / 2) * 1);
        vRayDir0.y = sin(player.dLookAtAngle + (player.dFoV / 2) * 1);

        olc::vd2d vRayDir1;
        vRayDir1.x = cos(player.dLookAtAngle + (player.dFoV / 2) * -1);
        vRayDir1.y = sin(player.dLookAtAngle + (player.dFoV / 2) * -1);

        // Current y position compared to the center of the screen (the horizon)
        int p = y - vResolution.y / 2;

        // Vertical position of the camera.
        double posZ = 0.5 * vResolution.y;

        // Horizontal distance from the camera to the floor for the current row.
        // 0.5 is the z position exactly in the middle between floor and ceiling.
        double rowDistance = posZ / p;

        // calculate the real world step vector we have to add for each x (parallel to camera plane)
        // adding step by step avoids multiplications with a weight in the inner loop
        double floorStepX = rowDistance * (vRayDir1.x - vRayDir0.x) / vResolution.x;
        double floorStepY = rowDistance * (vRayDir1.y - vRayDir0.y) / vResolution.x;

        // real world coordinates of the leftmost column. This will be updated as we step to the right.
        double floorX = player.vPosition.x + rowDistance * vRayDir0.x;
        double floorY = player.vPosition.y + rowDistance * vRayDir0.y;

        for(int x = 0; x < vResolution.x; ++x)
        {
            // the cell coord is simply got from the integer parts of floorX and floorY
            int cellX = (int)(floorX);
            int cellY = (int)(floorY);
            
            int tx = (int)(floorX * double(ShadowMapWidth) / mapWidth);
            int ty = (int)(floorY * double(ShadowMapHeight) / mapHeight);
            
            // get the texture coordinate from the fractional part
            // int tx = (int)( ShadowMapWidth * (floorX - cellX)) & (ShadowMapWidth - 1);
            // int ty = (int)(ShadowMapHeight * (floorY - cellY)) & (ShadowMapHeight - 1);
            
            floorX += floorStepX;
            floorY += floorStepY;

            // choose texture and draw the pixel
            int floorTexture = 3;
            int ceilingTexture = 6;
            olc::Pixel color;

            //floor
            if(pShadowMap->GetPixel(ty, tx) != olc::BLACK)
                Draw(x,y, olc::VERY_DARK_GREY);
            
            //ceil
            Draw(x,vResolution.y - y - 1, olc::VERY_DARK_BLUE);

            // floor
            // color = texture[floorTexture][texWidth * ty + tx];
            // color = (color >> 1) & 8355711; // make a bit darker
            // buffer[y][x] = color;

            //ceiling (symmetrical, at screenHeight - y - 1 instead of y)
            // color = texture[ceilingTexture][texWidth * ty + tx];
            // color = (color >> 1) & 8355711; // make a bit darker
            // buffer[screenHeight - y - 1][x] = color;
        }
    }

    //return;
    //wall casting 
    for (int k = 0; k < vResolution.x; k++)
    {
        double dCamX = 2 * k / double(vResolution.x) - 1;
        olc::vd2d vRayDir;
        vRayDir.x = cos(player.dLookAtAngle + (player.dFoV / 2) * -dCamX);
        vRayDir.y = sin(player.dLookAtAngle + (player.dFoV / 2) * -dCamX);

        //DDA Algorithm : https://lodev.org/cgtutor/raycasting.html
        olc::vi2d vMapPos;
        vMapPos.x = player.vPosition.x;
        vMapPos.y = player.vPosition.y;

        //length of ray from current position to next x or y-side
        double sideDistX;
        double sideDistY;

        //length of ray from one x or y-side to next x or y-side
        //these are derived as:
        //deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX))
        //deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY))
        //which can be simplified to abs(|rayDir| / rayDirX) and abs(|rayDir| / rayDirY)
        //where |rayDir| is the length of the vector (rayDirX, rayDirY). Its length,
        //unlike (dirX, dirY) is not 1, however this does not matter, only the
        //ratio between deltaDistX and deltaDistY matters, due to the way the DDA
        //stepping further below works. So the values can be computed as below.
        // Division through zero is prevented, even though technically that's not
        // needed in C++ with IEEE 754 floating point values.
        double deltaDistX = (vRayDir.x == 0) ? 1e30 : std::abs(1 / vRayDir.x); // sqrt(1 + (vRayDir.y / vRayDir.x) * (vRayDir.y / vRayDir.x));// ;
        double deltaDistY = (vRayDir.y == 0) ? 1e30 : std::abs(1 / vRayDir.y); // sqrt(1 + (vRayDir.x / vRayDir.y) * (vRayDir.x / vRayDir.y));// std::abs(1/ vRayDir.y);

        double perpWallDist;

        //what direction to step in x or y-direction (either +1 or -1)
        int stepX;
        int stepY;

        int hit = 0; //was there a wall hit?
        int side; //was a NS or a EW wall hit?
        //calculate step and initial sideDist
        if (vRayDir.x < 0)
        {
            stepX = -1;
            sideDistX = (player.vPosition.x - double(vMapPos.x)) * deltaDistX;
        }
        else
        {
            stepX = 1;
            sideDistX = (double(vMapPos.x + 1) - player.vPosition.x) * deltaDistX;
        }
        if (vRayDir.y < 0)
        {
            stepY = -1;
            sideDistY = (player.vPosition.y - double(vMapPos.y)) * deltaDistY;
        }
        else
        {
            stepY = 1;
            sideDistY = (double(vMapPos.y + 1) - player.vPosition.y) * deltaDistY;
        }
        //perform DDA

        double fMaxDistance = 1000.;
        double fDistance = 0.;
        while (hit == 0 /*&&  fDistance < fMaxDistance*/)
        {
            //jump to next map square, either in x-direction, or in y-direction
            if (sideDistX < sideDistY)
            {
                fDistance += sideDistY;
                sideDistX += deltaDistX;
                vMapPos.x += stepX;
                vMapPos.x = std::clamp(vMapPos.x, 0, mapWidth - 1);
                side = 0;
            }
            else
            {
                fDistance += sideDistY;
                sideDistY += deltaDistY;
                vMapPos.y += stepY;
                vMapPos.y = std::clamp(vMapPos.y, 0, mapHeight -1);
                side = 1;
            }
            //Check if ray has hit a wall
            if (worldMap[vMapPos.x][vMapPos.y] > 0) hit = 1;
        }
        //Calculate distance projected on camera direction. This is the shortest distance from the point where the wall is
        //hit to the camera plane. Euclidean to center camera point would give fisheye effect!
        //This can be computed as (mapX - posX + (1 - stepX) / 2) / rayDirX for side == 0, or same formula with Y
        //for size == 1, but can be simplified to the code below thanks to how sideDist and deltaDist are computed:
        //because they were left scaled to |rayDir|. sideDist is the entire length of the ray above after the multiple
        //steps, but we subtract deltaDist once because one step more into the wall was taken above.
        if (side == 0) perpWallDist = (sideDistX - deltaDistX);
        else          perpWallDist = (sideDistY - deltaDistY);

        //Calculate height of line to draw on screen
        int lineHeight = 0;
        
        if(hit)
            lineHeight = (int)(vResolution.y / perpWallDist);

        //calculate lowest and highest pixel to fill in current stripe
        int drawStart = vPos.y + -lineHeight / 2. + vResolution.y / 2;
        if (drawStart < vPos.y ) drawStart = vPos.y;
        int drawEnd = drawStart + lineHeight;
        if (drawEnd >= vPos.y + vResolution.y) drawEnd = vPos.y + vResolution.y - 1;

        //choose wall color
        olc::Pixel color;
        switch (worldMap[vMapPos.x][vMapPos.y])
        {
        case 0:  color = olc::BLACK;  break;
        case 1:  color = olc::RED;    break; //red
        case 2:  color = olc::GREEN;  break; //green
        case 3:  color = olc::BLUE;   break; //blue
        case 4:  color = olc::WHITE;  break; //white
        default: color = olc::YELLOW; break; //yellow
        }

        olc::vd2d hitPoint { player.vPosition.x + perpWallDist * vRayDir.x, player.vPosition.y + perpWallDist * vRayDir.y};
        int tx = (int)(hitPoint.x * double(ShadowMapWidth) / mapWidth);
        int ty = (int)(hitPoint.y * double(ShadowMapHeight) / mapHeight);

        color /= 1.5; // i don't like brightness
        if(pShadowMap->GetPixel(ty, tx ) == olc::BLACK
            ||pShadowMap->GetPixel(ty +1, tx ) == olc::BLACK
            ||pShadowMap->GetPixel(ty, tx+1 ) == olc::BLACK
            ||pShadowMap->GetPixel(ty+1, tx+1 ) == olc::BLACK
            ||pShadowMap->GetPixel(ty -1, tx ) == olc::BLACK
            ||pShadowMap->GetPixel(ty, tx-1 ) == olc::BLACK
            ||pShadowMap->GetPixel(ty-1, tx-1 ) == olc::BLACK)
        {
            color /=4;
        }
        
        //draw the pixels of the stripe as a vertical line
       DrawLine({ vPos.x + k, drawStart }, { vPos.x + k, drawEnd }, color);
       
        
        if(k % iDebugDisplayStep)
        {
            sHitPoints.insert(hitPoint);
        }
        
        // if (lineHeight != 0) //if we draw a wall, draw its shadow
        // {
        //     int shadowStart = drawEnd + 1;
        //     drawEnd = shadowStart + 0.2 * lineHeight;
        //     if(drawEnd >= vPos.y + vResolution.y) drawEnd = vPos.y + vResolution.y - 1;
        //
        //     DrawLine({ vPos.x + k, shadowStart + 1 }, { vPos.x + k, drawEnd }, olc::VERY_DARK_GREY / 4);
        // }
        //
        // if(drawEnd +1 < vPos.y + vResolution.y)
        //     DrawLine({ vPos.x + k, drawEnd + 1 }, { vPos.x + k, vPos.y + vResolution.y - 1 }, olc::VERY_DARK_GREY);
    }
}

olc::vd2d TAUPlayer::GetRightVector()
{
    return olc::vd2d(+vLookAt.y , -vLookAt.x);
}

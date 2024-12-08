#include "../Public/TAUDoom.h"

bool TAUDoom::OnUserCreate()
{
    //
    if (olc::Sprite::loader)
    {
        //olc::Sprite::loader->LoadImageResource(pWallSprite, std::string("../Ressources/Tile.png"), nullptr);

        pPlayerSprite   = new olc::Sprite(RESSOURCE_PATH  +  "Player.png");
        pWallSprite     = new olc::Sprite(RESSOURCE_PATH  +  "Tile.png");

    //C:\Users\TAU\Dev\PixelRendering\Source\Private
    //C : \Users\TAU\Dev\PixelRendering\Ressources

        if (pWallSprite) pWallDecal     = new olc::Decal(pWallSprite);
        if (pPlayerSprite) pPlayerDecal = new olc::Decal(pPlayerSprite);
    }
    player = { {12., 12.} };
    olc_UpdateMouseFocus(true);

    Setup2DMap();
	return true;
}

bool TAUDoom::OnUserUpdate(float fElapsedTime)
{
	//Quit app
	if (GetKey(olc::ESCAPE).bReleased) return false;

	//Basic commands
	if (GetKey(olc::Z).bHeld) player.vPosition += player.vLookAt * player.dSpeed * fElapsedTime;
	if (GetKey(olc::S).bHeld) player.vPosition -= player.vLookAt * player.dSpeed * fElapsedTime;
    if (GetKey(olc::Q).bHeld) player.vPosition -= player.GetRightVector() * player.dSpeed * fElapsedTime;
	if (GetKey(olc::D).bHeld) player.vPosition += player.GetRightVector() * player.dSpeed * fElapsedTime;

    if (GetKey(olc::RIGHT).bHeld)
    {
        player.dLookAtAngle += player.dRotateSpeed * fElapsedTime;
        player.vLookAt.x = cos(player.dLookAtAngle);
        player.vLookAt.y = sin(player.dLookAtAngle);
    }
    if (GetKey(olc::LEFT).bHeld)
    {
        player.dLookAtAngle -= player.dRotateSpeed * fElapsedTime;
        player.vLookAt.x = cos(player.dLookAtAngle);
        player.vLookAt.y = sin(player.dLookAtAngle);
    }

	Clear(olc::BLACK);

    

    //return true;
	for (int k = 0; k < ScreenWidth(); k++)
	{
		double dCamX = 2 * k / double(ScreenWidth()) - 1;
        olc::vd2d vRayDir;
        vRayDir.x = cos(player.dLookAtAngle + (player.dFoV /2 ) * dCamX);
        vRayDir.y = sin(player.dLookAtAngle + (player.dFoV /2) * dCamX);

		//DDA Algorithm : https://lodev.org/cgtutor/raycasting.html
        olc::vi2d vMapPos;
        vMapPos.x = player.vPosition.x;
        vMapPos.y = player.vPosition.x;

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
        double deltaDistX = (vRayDir.x == 0) ? 1e30 : sqrt(1 + (vRayDir.y / vRayDir.x) * (vRayDir.y / vRayDir.x));// std::abs(1 / vRayDir.x);
        double deltaDistY = (vRayDir.y == 0) ? 1e30 : sqrt(1 + (vRayDir.x / vRayDir.y) * (vRayDir.x / vRayDir.y));// std::abs(1/ vRayDir.y);

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
            sideDistX = (player.vPosition.x  - double(vMapPos.x)) * deltaDistX;
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
            sideDistY = (double(vMapPos.y + 1) - player.vPosition.y ) * deltaDistY;
        }
        //perform DDA

        double fMaxDistance = 100.;
        double fDistance = 0.;
        while (hit == 0 && fDistance < fMaxDistance)
        {
            //jump to next map square, either in x-direction, or in y-direction
            if (sideDistX < sideDistY)
            {
                fDistance = sideDistY;
                sideDistX += deltaDistX;
                vMapPos.x += stepX;
                vMapPos.x = std::clamp(vMapPos.x, 0, mapWidth);
                side = 0;
            }
            else
            {
                fDistance = sideDistY;
                sideDistY += deltaDistY;
                vMapPos.y += stepY;
                vMapPos.y = std::clamp(vMapPos.y, 0, mapHeight);
                side = 1;
            }
            //Check if ray has hit a wall
            if(vMapPos.x  >= 0 && vMapPos.x < mapWidth && vMapPos.y >= 0 && vMapPos.y < mapHeight)
                if (worldMap[vMapPos.x][vMapPos.y] > 0) hit = 1;
        }
        //Calculate distance projected on camera direction. This is the shortest distance from the point where the wall is
        //hit to the camera plane. Euclidean to center camera point would give fisheye effect!
        //This can be computed as (mapX - posX + (1 - stepX) / 2) / rayDirX for side == 0, or same formula with Y
        //for size == 1, but can be simplified to the code below thanks to how sideDist and deltaDist are computed:
        //because they were left scaled to |rayDir|. sideDist is the entire length of the ray above after the multiple
        //steps, but we subtract deltaDist once because one step more into the wall was taken above.
        if (side == 0) perpWallDist = (sideDistX - deltaDistX);
        else          perpWallDist = (sideDistY - deltaDistY );

        //Calculate height of line to draw on screen
        int lineHeight = (int)(ScreenHeight() / perpWallDist);

        //calculate lowest and highest pixel to fill in current stripe
        int drawStart = -lineHeight / 2 + ScreenHeight() / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + ScreenHeight() / 2;
        if (drawEnd >= ScreenHeight()) drawEnd = ScreenHeight() - 1;

        //choose wall color
        olc::Pixel color;
        switch (worldMap[vMapPos.x][vMapPos.y])
        {
        case 1:  color = olc::RED;    break; //red
        case 2:  color = olc::GREEN;  break; //green
        case 3:  color = olc::BLUE;   break; //blue
        case 4:  color = olc::WHITE;  break; //white
        default: color = olc::YELLOW; break; //yellow
        }

        //give x and y sides different brightness
        if (side == 1) { color = color / 2; }

        //draw the pixels of the stripe as a vertical line
        DrawLine({ k, drawStart }, { k, drawEnd }, color);
        DrawLine({ k, drawEnd + 1 }, { k, ScreenHeight() - 1 }, olc::VERY_DARK_GREY);
    }

    Render2DMap();
    olc::vi2d vMapPos = player.vPosition;
    DrawRect(olc::vi2d(ScreenWidth() - (mapWidth * miniMapScale + 2) + vMapPos.x * miniMapScale, ScreenHeight() - (mapHeight * miniMapScale + 2) + vMapPos.y * miniMapScale), olc::vi2d(miniMapScale, miniMapScale));
	return true;
}

void TAUDoom::Setup2DMap()
{
    pMiniMapSprite = new olc::Sprite(mapWidth * miniMapScale, mapWidth * miniMapScale);

    SetDrawTarget(pMiniMapSprite); //draw on sprite
    Clear(olc::BLACK);

    

    float fTileSize = (float)miniMapScale / pWallSprite->width;
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
                olc::vi2d(i * miniMapScale, j * miniMapScale),
                olc::vi2d(miniMapScale, miniMapScale),
                color
             );
        }
    }

    
    SetDrawTarget(nullptr); //release sprite writing 

    pMiniMapDecal = new olc::Decal(pMiniMapSprite);
}

void TAUDoom::DrawPlayer()
{

    olc::vi2d minimapOrigin(ScreenWidth() - (mapWidth * miniMapScale), ScreenHeight() - (mapHeight * miniMapScale));
    olc::vi2d vProjectedTranslation((player.vPosition.x / mapSizeX) * miniMapScale * mapWidth, (player.vPosition.y / mapSizeY) * miniMapScale * mapWidth);
    olc::vi2d vProjectedLoc = minimapOrigin + vProjectedTranslation;

    //DrawTriangle(vProjectedLoc)
    DrawLine(vProjectedLoc, vProjectedLoc + olc::vf2d(cos(player.dLookAtAngle - player.dFoV /2), sin(player.dLookAtAngle - player.dFoV / 2)) * 25, olc::DARK_MAGENTA);
    DrawLine(vProjectedLoc, vProjectedLoc + olc::vf2d(cos(player.dLookAtAngle + player.dFoV / 2) , sin(player.dLookAtAngle + player.dFoV / 2)) * 25, olc::DARK_MAGENTA);
    FillCircle(vProjectedLoc, miniMapScale / 2, olc::CYAN);
    //std::cout << vProjectedLoc << '\n';

    DrawLine(vProjectedLoc, vProjectedLoc + olc::vf2d(cos(player.dLookAtAngle + (player.dFoV / 2) * -1), sin(player.dLookAtAngle + (player.dFoV / 2) * -1)) * 25, olc::RED);
    DrawLine(vProjectedLoc, vProjectedLoc + olc::vf2d(cos(player.dLookAtAngle + (player.dFoV / 2) * 1), sin(player.dLookAtAngle + (player.dFoV / 2) * 1)) * 25, olc::RED);
    
}

void TAUDoom::Render2DMap()
{
    DrawRect(
        olc::vi2d(ScreenWidth() - (mapWidth * miniMapScale + 2), ScreenHeight() - (mapHeight * miniMapScale + 2)),
        olc::vi2d(mapWidth * miniMapScale + 1, mapHeight * miniMapScale +1),
        olc::WHITE);

    DrawSprite(olc::vi2d(ScreenWidth() - (mapWidth * miniMapScale + 1), ScreenHeight() - (mapHeight * miniMapScale +1)), pMiniMapSprite);

    DrawPlayer();

}

void TAUDoom::RenderDoomMap()
{
}

olc::vd2d TAUPlayer::GetRightVector()
{
    return olc::vd2d(-vLookAt.y , vLookAt.x);
}

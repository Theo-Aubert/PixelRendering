#include "../Public/TAUDoom.h"

#define RESSOURCE_PATH std::string(".../../Ressources/")

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
	player = { {GetScreenSize() / 2.}, { 0., 1.}, {0.66, 0.} };
    olc_UpdateMouseFocus(true);
	return true;
}

bool TAUDoom::OnUserUpdate(float fElapsedTime)
{
	//Quit app
	if (GetKey(olc::ESCAPE).bReleased) return false;

	//Basic commands
	if (GetKey(olc::Z).bHeld) player.vPosition.y -= std::max(0.,player.dSpeed * fElapsedTime);
	if (GetKey(olc::S).bHeld) player.vPosition.y += std::min(double(ScreenHeight()), player.dSpeed * fElapsedTime);
	if (GetKey(olc::Q).bHeld) player.vPosition.x -= std::max(0. ,player.dSpeed * fElapsedTime);
	if (GetKey(olc::D).bHeld) player.vPosition.x += std::min(double(ScreenWidth()), player.dSpeed * fElapsedTime);

    if (GetKey(olc::RIGHT).bHeld)
    {
        double oldDirX = player.vLookAt.x;
        player.vLookAt.x = player.vLookAt.x * cos(-player.dRotateSpeed * fElapsedTime) - player.vLookAt.y * sin(-player.dRotateSpeed * fElapsedTime);
        player.vLookAt.y = oldDirX * sin(-player.dRotateSpeed *fElapsedTime) + player.vLookAt.y * cos(-player.dRotateSpeed);

        double oldPlaneX = player.vCameraPlane.x;
        player.vCameraPlane.x = player.vCameraPlane.x * cos(-player.dRotateSpeed * fElapsedTime) - player.vCameraPlane.y * sin(-player.dRotateSpeed * fElapsedTime);
        player.vCameraPlane.y = oldPlaneX * sin(-player.dRotateSpeed * fElapsedTime) + player.vCameraPlane.y * cos(-player.dRotateSpeed * fElapsedTime);
    }
    if (GetKey(olc::LEFT).bHeld)
    {
        double oldDirX = player.vLookAt.x;
        player.vLookAt.x = player.vLookAt.x * cos(player.dRotateSpeed * fElapsedTime) - player.vLookAt.y * sin(player.dRotateSpeed * fElapsedTime);
        player.vLookAt.y = oldDirX * sin(player.dRotateSpeed * fElapsedTime) + player.vLookAt.y * cos(player.dRotateSpeed);

        double oldPlaneX = player.vCameraPlane.x;
        player.vCameraPlane.x = player.vCameraPlane.x * cos(player.dRotateSpeed * fElapsedTime) - player.vCameraPlane.y * sin(player.dRotateSpeed * fElapsedTime);
        player.vCameraPlane.y = oldPlaneX * sin(player.dRotateSpeed * fElapsedTime) + player.vCameraPlane.y * cos(player.dRotateSpeed * fElapsedTime);
    }

	Clear(olc::BLACK);
	//DrawCircle(player.vPosition, 6);

    if (pWallDecal)
    {
        DrawDecal(player.vPosition, pWallDecal, { 1.f,1.f }, olc::RED);
    }

    if (pPlayerDecal) DrawDecal(player.vPosition + olc::vd2d{ 10., 10.}, pPlayerDecal);

    return true;
	for (int k = 0; k < ScreenWidth(); k++)
	{
		double dCamX = 2 * k / double(m_iStripes) - 1;
		olc::vd2d vRayDir = player.vLookAt + player.vCameraPlane * dCamX;

		//DDA Algorithm : https://lodev.org/cgtutor/raycasting.html
		olc::vi2d vMapPos = (player.vPosition / GetScreenSize()) * m_vMapSize;

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
        double deltaDistX = (vRayDir.x == 0) ? 1e30 : std::abs(1 / vRayDir.x);
        double deltaDistY = (vRayDir.y == 0) ? 1e30 : std::abs(1 / vRayDir.y);

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
            sideDistX = (player.vPosition.x - vMapPos.x) * deltaDistX;
        }
        else
        {
            stepX = 1;
            sideDistX = (player.vPosition.x + 1.0 - vMapPos.x) * deltaDistX;
        }
        if (vRayDir.y < 0)
        {
            stepY = -1;
            sideDistY = (player.vPosition.y - vMapPos.y) * deltaDistY;
        }
        else
        {
            stepY = 1;
            sideDistY = (player.vPosition.y + 1.0 - vMapPos.y) * deltaDistY;
        }
        //perform DDA
        while (hit == 0)
        {
            //jump to next map square, either in x-direction, or in y-direction
            if (sideDistX < sideDistY)
            {
                sideDistX += deltaDistX;
                vMapPos.x += stepX;
                side = 0;
            }
            else
            {
                sideDistY += deltaDistY;
                vMapPos.y += stepY;
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
    }
	return true;
}

void TAUDoom::Render2DMap()
{
}

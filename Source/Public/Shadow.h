#include "PixelApp.h"
#include "AppManager.h"
#include "Constants.h"

#pragma once

class Shadow : public PixelApp
{
public:

	struct Player
	{
		olc::vi2d position;
		float moveSpeed;

		olc::vf2d forwardVector;
		float viewAperture;
		
	};

	struct Mirror
	{
		olc::vi2d start;
		olc::vi2d end;
	};

	Shadow();

	bool OnUserCreate() override;

	bool OnUserUpdate(float fElapsedTime) override;

private :
	
	void RotatePlayerView();

	void DrawFrustum();

	olc::vi2d LineTrace(const olc::vi2d& startPoint, olc::vf2d& dir) const;

	bool IsRayIntersectingMirror(const olc::vf2d& rayStart, const olc::vf2d& rayDir, const Mirror& mirror, olc::vf2d& intersection, olc::vf2d& bounceDir);
private:

	byte* map;

	olc::Sprite* spriteMap;
	Player player;

	std::vector<Mirror> mirrors;
	uint8_t numBounces = 12;
};
REGISTER_CLASS(Shadow)


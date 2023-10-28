#include "../Public/Shadow.h"
#include <cmath>


#define EvilString(s) std::string s = #s

Shadow::Shadow()
{
	sAppName = "Shadow Casting";
}

bool Shadow::OnUserCreate()
{
	map = new byte[ScreenWidth() * ScreenHeight()];

	olc::vi2d recSize(90, 70);

	mirrors.push_back(Mirror(olc::vi2d(40, 55), olc::vi2d(125, 440)));
	mirrors.push_back(Mirror(olc::vi2d(670, 55), olc::vi2d(325, 523)));
	mirrors.push_back(Mirror(olc::vi2d(75, 124), olc::vi2d(125, 440)));
	mirrors.push_back(Mirror(olc::vi2d(520, 36), olc::vi2d(25, 40)));
	mirrors.push_back(Mirror(olc::vi2d(155, 55), olc::vi2d(115, 440)));

	DrawRect(olc::vi2d(ScreenWidth() / 2, ScreenHeight() / 2) - recSize /2, recSize);

	for (const auto& mirror : mirrors)
	{
		DrawLine(mirror.start, mirror.end, olc::BLUE);
	}

	spriteMap = GetDrawTarget()->Duplicate();

	Clear(olc::BLACK);

	player.position = olc::vi2d(ScreenWidth() / 2, ScreenHeight() / 2);
	player.moveSpeed = 0.2f;

	player.forwardVector = olc::vf2d(1.f, 0.f);
	player.viewAperture = PI_F / 4.f;

	return true;
}

bool Shadow::OnUserUpdate(float fElapsedTime)
{

	if (GetKey(olc::SPACE).bHeld)
	{
		return true;
	}

	Clear(olc::BLACK);

	for (const auto& mirror : mirrors)
	{
		DrawLine(mirror.start, mirror.end, olc::BLUE);
	}

	//Rotate player view
	if (GetMouse(olc::Mouse::LEFT).bHeld)
	{
		RotatePlayerView();
		DrawLine(player.position, player.position + player.forwardVector * 25.f, olc::RED);
	}

	//Update player pos
	if (GetMouse(olc::Mouse::RIGHT).bHeld)
	{
		player.position = GetMousePos();
		//DrawLine(player.position, GetMousePos());
	}


	if (GetKey(olc::Key::RIGHT).bHeld)
	{
		player.position.x = std::min(int(player.position.x + player.moveSpeed * fElapsedTime), ScreenWidth() - 1);
	}
	if (GetKey(olc::Key::LEFT).bHeld)
	{
		player.position.x = std::max(0, int(player.position.x - player.moveSpeed * fElapsedTime));
	}
	if (GetKey(olc::Key::DOWN).bHeld)
	{
		player.position.y = std::min(int(player.position.y + player.moveSpeed * fElapsedTime), ScreenHeight() - 1);
	}
	if (GetKey(olc::Key::UP).bHeld)
	{
		player.position.y = std::max(0, int(player.position.y - player.moveSpeed * fElapsedTime));
	}


	//DrawCircle(player.position, 5);
	//DrawLine(player.position, LineTrace(player.position, player.forwardVector));
	//DrawFrustum();

	olc::vf2d rayStart = player.position;
	olc::vf2d rayDir = player.forwardVector;

	olc::vf2d closestIntersection;
	olc::vf2d closestBounce;

	Mirror* currentMirror = nullptr;
	Mirror* closestMirror = nullptr;
	for (int k = 0; k <= numBounces; k++)
	{
		float closestDist = std::numeric_limits<float>::infinity();

		bool bHitMirror = false;
		for (auto it = mirrors.begin(); it < mirrors.end(); it++)
		{
			if (currentMirror == nullptr || currentMirror != &(*it))

			{
				olc::vf2d intersection;
				olc::vf2d bounceDir; 
				
				if (IsRayIntersectingMirror(rayStart, rayDir, (*it), intersection, bounceDir))
				{
					bHitMirror = true;

					float dist = (intersection - rayStart).mag2();
					if (dist < closestDist)
					{
						closestDist = dist;
						closestIntersection = intersection;
						closestBounce = bounceDir;
						closestMirror = &(*it);
					}
				}
			}
		}

		if (bHitMirror)  
		{
			DrawLine(rayStart, closestIntersection);
			rayStart = closestIntersection;
			rayDir = closestBounce;
			currentMirror = closestMirror;
		}
		else
		{
			DrawLine(rayStart, rayStart + rayDir * float(ScreenHeight() * ScreenWidth()));
			break;
		}
	}
	
	return true;
}

void Shadow::RotatePlayerView()
{
	olc::vi2d mousePos = GetMousePos();
	 
	player.forwardVector = mousePos - player.position;
	player.forwardVector = player.forwardVector.norm();
}

void Shadow::DrawFrustum()
{
	float angleOffsetX = cos( player.viewAperture /2.f);
	float angleOffsetY = sin( player.viewAperture /2.f);

	olc::vf2d upperBound = olc::vf2d(
		cos(player.viewAperture / 2.f) * player.forwardVector.x - sin(player.viewAperture / 2.f) * player.forwardVector.y,
		sin(player.viewAperture / 2.f) * player.forwardVector.x + cos(player.viewAperture / 2.f) * player.forwardVector.y
	);

	olc::vf2d lowerBound = olc::vf2d(
		cos(-player.viewAperture / 2.f) * player.forwardVector.x - sin(-player.viewAperture / 2.f) * player.forwardVector.y,
		sin(-player.viewAperture / 2.f) * player.forwardVector.x + cos(-player.viewAperture / 2.f) * player.forwardVector.y
	);

	DrawLine(player.position, LineTrace(player.position, upperBound));
	DrawLine(player.position, LineTrace(player.position, lowerBound));
}

olc::vi2d Shadow::LineTrace(olc::vi2d& startPoint, olc::vf2d& dir)
{
	olc::vi2d result = startPoint;
	olc::vf2d compute = startPoint;

	bool bHit = false;

	while (!bHit)
	{
		compute += dir;
		bHit = compute.x < 0 || compute.x > ScreenWidth() - 1 || compute.y < 0 || compute.y > ScreenHeight() - 1 ||
			spriteMap->GetPixel(compute) == olc::WHITE;
	}   

	return compute;
}

bool Shadow::IsRayIntersectingMirror(const olc::vf2d& rayStart, const olc::vf2d& rayDir, const Mirror& mirror, olc::vf2d& intersection, olc::vf2d& bounceDir)
{
	if (rayDir == olc::vf2d())
	{
		return false;
	}

	//compute vectors from the raycaster to the mirror edges
	olc::vf2d upperCone = mirror.start - rayStart;
	olc::vf2d lowerCone = mirror.end - rayStart;

	//computedirector vector for the mirror
	olc::vf2d mirrorVec = (mirror.end - mirror.start);

	//Normalize
	mirrorVec = mirrorVec.norm(); 
	olc::vf2d rayVec = rayDir.norm();

	//Get the two normal to the mirror surface
	olc::vf2d N1(  mirrorVec.y, - mirrorVec.x);
	olc::vf2d N2(- mirrorVec.y,   mirrorVec.x);

	olc::vf2d normal;


	//Find normal pointing toward the ray emitter
	if ((rayVec + N1).mag2() < (rayVec + N2).mag2())
	{
		normal = N1.norm();
	}
	else
	{
		normal = N2.norm();
	}

	
	upperCone = upperCone.norm();
	lowerCone = lowerCone.norm();

	float lowerCross = rayVec.cross(lowerCone);
	float upperCross = rayVec.cross(upperCone);

	//determine if the ray is between the 2 vectors joining the emission point and the mirror edges
	float order = lowerCone.cross(upperCone);
	float minCross;
	float maxCross;

	if(order < 0)
	{
		minCross = lowerCross;
		maxCross = upperCross;
	}
	else
	{
		maxCross = lowerCross;
		minCross = upperCross;
	}

	//std::cout << "upper cross" << upperCross << "\n";
	//std::cout << "lower cross" << lowerCross << "\n";

	if (!(minCross >= 0 && maxCross <= 0))
	{ 
		return false;
	}

	//defining line equations
	float mirrorA =	  mirrorVec.y;
	float mirrorB = - mirrorVec.x;
	float mirrorC = - mirrorVec.y * mirror.start.x + mirrorVec.x * mirror.start.y;

	float rayA = rayDir.y;
	float rayB = - rayDir.x;
	float rayC = - rayDir.y * rayStart.x + rayDir.x * rayStart.y;
		
	float intersectY = ((mirrorA / rayA) * rayC - mirrorC) / (mirrorB - (rayB * mirrorA / rayA));
	float intersectX = -(rayB * intersectY + rayC) / rayA;  

	intersection = olc::vf2d(intersectX, intersectY);
	bounceDir = 2 * normal.dot(-rayVec) * normal + rayVec;
	bounceDir = bounceDir.norm();

	//DrawCircle(intersection, 5, olc::RED);
	//DrawLine(intersection, intersection + normal * 15.f, olc::GREEN);
	//DrawLine(intersection, intersection + N1 * 15.f, olc::DARK_GREEN);
	//DrawLine(intersection, intersection + N2 * 15.f, olc::VERY_DARK_GREEN);
	//DrawLine(intersection, intersection + bounceDir * 150, olc::CYAN);

	return true;

	
}

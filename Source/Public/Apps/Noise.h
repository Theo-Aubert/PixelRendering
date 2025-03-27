#pragma once
#include "AppManager.h"
#include "Constants.h"

class Noise : public PixelApp
{
public :

	struct Agent
	{
		//(x, y)
		float positionX;
		float positionY;
		float angle;
	};

	Noise()
	{
		sAppName = "PseudoRandomNoise";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		TrailBuffer = olc::Sprite(960, 540);
		ProcessedTrailBuffer = olc::Sprite(960, 540);

		std::array<float, 3> e {0.f, 0.f, 0.f};

		Trail = std::vector<std::array<float, 3>>(ScreenWidth() * ScreenHeight());
		ProcessedTrail = std::vector<std::array<float, 3>>(ScreenWidth() * ScreenHeight());

		std::fill(Trail.begin(), Trail.end(), e);
		std::fill(ProcessedTrail.begin(), ProcessedTrail.end(), e);

		initAgents();
		return true;
	}


	bool OnUserUpdate(float fElapsedTime) override
	{

		//Clear(olc::BLACK);

		if (GetKey(olc::Key::SPACE).bHeld)
		{
			return true;
		}

		if (GetKey(olc::Key::R).bReleased)
		{
			std::array<float, 3> e {0.f, 0.f, 0.f};

			std::fill(Trail.begin(), Trail.end(), e);
			std::fill(ProcessedTrail.begin(), ProcessedTrail.end(), e);

			initAgents();
		}

		if (GetKey(olc::Key::UP).bReleased)
		{
			diffuseSpeed = std::min(100.f, diffuseSpeed + 10.f);
		}

		if (GetKey(olc::Key::DOWN).bReleased)
		{
			diffuseSpeed = std::max(0.f, diffuseSpeed - 10.f);
		}

		for (int i = 0; i < agents.size(); ++i)
		{
			Agent agent = agents[i];

			uint32_t random = hash(uint32_t(agent.positionY * ScreenWidth() + agent.positionX) + hash(i));

			float directionX = std::cos(agent.angle);
			float directionY = std::sin(agent.angle);

			float newPosX = agent.positionX + directionX * moveSpeed * fElapsedTime;
			float newPosY = agent.positionY + directionY * moveSpeed * fElapsedTime;

			float weightForward = sense(agent, 0);
			float weightLeft = sense(agent, sensorAngleSpacing);
			float weightRight = sense(agent, -sensorAngleSpacing);

			float randomSteerStrength = random / 4294967295.0f;

			if (weightForward > weightLeft && weightForward > weightRight)
			{
				agents[i].angle += 0;
			}
			else if (weightForward < weightLeft && weightForward < weightRight)
			{
				agents[i].angle += (randomSteerStrength - .5f) * 2 * turnSpeed * fElapsedTime;
			}
			else if (weightRight > weightLeft)
			{
				agents[i].angle -= randomSteerStrength * turnSpeed * fElapsedTime;
			}
			else if (weightLeft > weightRight)
			{
				agents[i].angle += randomSteerStrength * turnSpeed * fElapsedTime;

			}

			if (newPosX < 0 || newPosX >= ScreenWidth() || newPosY < 0 || newPosY >= ScreenHeight())
			{


				float currentAngle = agents[i].angle;

				//define in which quadrant(s) of the unit circle we can bounce back, to avoid staggering
				if (newPosX < 0)
				{
					agents[i].angle = ((random / 4294967295.0f) * PI_F);
				}
				else if (newPosX >= ScreenWidth())
				{
					agents[i].angle = ((random / 4294967295.0f) * PI_F) + (PI_F);
				}
				else if (newPosY < 0)
				{
					agents[i].angle = (random / 4294967295.0f) * PI_F + (PI_F / 2);
				}
				else if (newPosY >= ScreenHeight())
				{
					agents[i].angle = (random / 4294967295.0f) * PI_F - (PI_F / 2);
				}

				newPosX = std::min(ScreenWidth() - 0.01f, std::max(0.f, newPosX));
				newPosY = std::min(ScreenHeight() - 0.01f, std::max(0.f, newPosY));
			}

			agents[i].positionX = newPosX;
			agents[i].positionY = newPosY;
			//frontBuffer.SetPixel(int(newPosX), int(newPosY), olc::Pixel(255, 255, 255));

			Trail[int(newPosY) * ScreenWidth() + int(newPosX)][0] = 255.f; 
			Trail[int(newPosY) * ScreenWidth() + int(newPosX)][1] = 255.f;
			Trail[int(newPosY) * ScreenWidth() + int(newPosX)][2] = 255.f;
			//TrailBuffer.SetPixel(int(newPosX), int(newPosY), olc::Pixel(255U, 255U, 255U));
		}


		for (int x = 0; x < ScreenWidth(); x++)
		{
			for (int y = 0; y < ScreenHeight(); y++)
			{
				//olc::Pixel originalValue = TrailBuffer.GetPixel(x, y);
				std::array<float, 3> originalValue = Trail[y * ScreenWidth() + x];

				//Convolution
				float sum[3] = { 0.f, 0.f, 0.f };

				//3x3 kernel
				for (int offsetX = -1; offsetX <= 1; offsetX++)
				{
					for (int offsetY = -1; offsetY <= 1; offsetY++)
					{
						int sampleX = x + offsetX;
						int sampleY = y + offsetY;

						if (sampleX >= 0 && sampleX < ScreenWidth() && sampleY >= 0 && sampleY < ScreenHeight())
						{
							std::array<float, 3> samplePx = Trail[sampleY * ScreenWidth() + sampleX];
							//olc::Pixel samplePx = TrailBuffer.GetPixel(sampleX, sampleY);
							sum[0] += samplePx[0];
							sum[1] += samplePx[1];
							sum[2] += samplePx[2];
						}
					}
				}


				float blurPx[3]{ sum[0] / 9.f, sum[1] / 9.f, sum[2] / 9.f };
				float diffusedPx[3]{ std::lerp(originalValue[0],blurPx[0],diffuseSpeed * fElapsedTime), std::lerp(originalValue[1],blurPx[1],diffuseSpeed * fElapsedTime), std::lerp(originalValue[2],blurPx[2],diffuseSpeed * fElapsedTime)};
				//float diffusedPx[3]{ originalValue[0], originalValue[1], originalValue[2]};
				float evaporatedValue[3]{ std::max(0.f,diffusedPx[0] - evaporateSpeed * fElapsedTime), std::max(0.f,diffusedPx[1] - evaporateSpeed * fElapsedTime), std::max(0.f,diffusedPx[2] - evaporateSpeed * fElapsedTime)};
				
				ProcessedTrail[y * ScreenWidth() + x][0] = evaporatedValue[0];
				ProcessedTrail[y * ScreenWidth() + x][1] = evaporatedValue[1];
				ProcessedTrail[y * ScreenWidth() + x][2] = evaporatedValue[2];

				Draw(x, y, olc::Pixel(uint8_t(evaporatedValue[0]), uint8_t(evaporatedValue[1]),uint8_t(evaporatedValue[2])));
				//Draw(x, y, olc::Pixel(uint8_t(originalValue[0]), uint8_t(originalValue[1]), uint8_t(originalValue[2])));
			}
		}

		Trail = ProcessedTrail;
		
		//TrailBuffer = ProcessedTrailBuffer;

		return true;
	}

private:

	int32_t hash(int32_t state)
	{
		state ^= 2747636419u;
		state *= 2654435769u;
		state ^= state >> 16;
		state *= 2654435769u;
		state ^= state >> 16;
		state *= 2654435769u;
		return state;
	}

	void initAgents()
	{
		agents.reserve(numAgents);
		for (int i = 0; i < numAgents; ++i)
		{
			Agent newAgent;
			newAgent.positionX = ScreenWidth() / 2.f;
			newAgent.positionY = ScreenHeight() / 2.f;
			newAgent.angle = (i / float(numAgents)) * 2 * PI_F;
			agents.emplace_back(newAgent);
		}
	}

	float sense(Agent agent, float sensorAngleOffset)
	{
		float sensorAngle = agent.angle + sensorAngleOffset;
		float sensorDirX = std::cos(sensorAngle);
		float sensorDirY = std::sin(sensorAngle);
		int sensorCenterX = int(agent.positionX + sensorDirX * sensorOffsetDst);
		int sensorCenterY = int(agent.positionY + sensorDirY * sensorOffsetDst);
		float sum = 0;

		for (int offsetX = -sensorSize; offsetX <= sensorSize; offsetX++)
		{
			for (int offsetY = -sensorSize; offsetY <= sensorSize; offsetY++)
			{
				int posX = sensorCenterX + offsetX;
				int posY = sensorCenterY + offsetY;

				if (posX >= 0 && posX < ScreenWidth() && posY >= 0 && posY < ScreenHeight())
				{
					sum += Trail[posY * ScreenWidth() + posX][0];
					//sum += TrailBuffer.GetPixel(posX, posY).r;
					//Draw(posX, posY, olc::BLUE);
				}

			}
		}

		return sum;
		//return sensorAngleOffset;
	}

	int numAgents =1000;
	std::vector<Agent> agents;

	float moveSpeed = 150.0f;
	float turnSpeed = PI_F;

	float evaporateSpeed = 100.f;
	float diffuseSpeed = 5.f;

	float sensorOffsetDst = 50.f;
	float sensorAngleSpacing = PI_F / 8.f;
	int sensorSize = 9;

	olc::Sprite TrailBuffer;
	olc::Sprite ProcessedTrailBuffer;

	std::vector<std::array<float,3>> ProcessedTrail;
	std::vector<std::array<float,3>> Trail;
};
REGISTER_CLASS(Noise)
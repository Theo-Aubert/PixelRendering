#define OLC_PGE_APPLICATION
#include "Externals/olcPixelGameEngine.h"
#include "Public/Shadow.h"
#include "Public/SandSimulation.h"
#include "Public/Universe.h"
#include "Public/DreamCatcher.h"
#include "Public/TAUDoom.h"
#include "Public/ImageProcessor.h"
#include "Public/WaveFC.h"
#include "Public/ProceduralAnim.h"
#include "Public/YamScoreBoard.h"
#include <cmath>

// Override base class with your custom functionality
class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		// Name your application
		sAppName = "Example";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Called once per frame, draws random coloured pixels
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, olc::Pixel(rand() % 256, rand() % 256, rand() % 256));
		return true;
	}
};

class CellularAutomata : public olc::PixelGameEngine
{
public:
	CellularAutomata()
	{
		// Name your application
		sAppName = "Example";
	}

public:
	bool OnUserCreate() override
	{
		output = new int32_t[int(ScreenWidth() * ScreenHeight())];
		state = new int32_t[int(ScreenWidth() * ScreenHeight())];

		memset(output, 0, int(ScreenWidth() * ScreenHeight()) * sizeof(int32_t));
		memset(state, 0, int(ScreenWidth() * ScreenHeight()) * sizeof(int32_t));

		randomInit();

		// Called once at the start, so create things here
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{

		std::this_thread::sleep_for(std::chrono::milliseconds(16));

		if (GetKey(olc::Key::R).bReleased)
		{
			randomInit();
		}

		if (GetKey(olc::Key::SPACE).bHeld)
		{
			return true;
		}



		auto cell = [&](int x, int y)
		{
			return output[y * ScreenWidth() + x];
		};

		for (int i = 0; i < ScreenWidth() * ScreenHeight(); i++)
		{
			output[i] = state[i];
		}

		for (int x = 1; x < ScreenWidth() - 1; x++)
		{
			for (int y = 1; y < ScreenHeight() - 1; y++)
			{

				int Neighbours =
					cell(x - 1, y - 1) + cell(x, y - 1) + cell(x + 1, y - 1) +
					cell(x - 1, y) + 0 + cell(x + 1, y) +
					cell(x - 1, y + 1) + cell(x, y + 1) + cell(x + 1, y + 1);

				if (cell(x, y) == 1)
				{
					state[y * ScreenWidth() + x] = Neighbours == 2 || Neighbours == 3;
					Draw(x, y, olc::Pixel(olc::WHITE));
				}
				else
				{
					state[y * ScreenWidth() + x] = Neighbours == 3;
					Draw(x, y, olc::Pixel(olc::BLACK));
				}
			}
		}

		return true;
	}

	int hash(int state)
	{
		state ^= 2747636419u;
		state *= 2654435769u;
		state ^= state >> 16;
		state *= 2654435769u;
		state ^= state >> 16;
		state *= 2654435769u;
		return state;
	}

	void randomInit()
	{
		for (int i = 0; i < ScreenWidth() * ScreenHeight(); i++)
		{
			state[i] = hash(rand()) % 2;
		}
	}

private:
	int32_t* output;
	int32_t* state;

};


class Noise : public olc::PixelGameEngine
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

class Bouncer : public olc::PixelGameEngine
{
public:

	struct BounceOject
	{
		olc::vf2d position;
		olc::vf2d dPosition;
		olc::Pixel color;

		int nbBounces = 10;
		float size = 15.f;
		float bounciness = 0.7f; //[0,1]
		
	};

	Bouncer()
	{
		// Name your application
		sAppName = "Bouncer";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);
		if (GetMouse(olc::Mouse::LEFT).bHeld)
		{
			bouncers.push_back(BounceOject(GetMousePos(), olc::vf2d(rand() % 300 - 150.f, 0.f), olc::Pixel(rand() % 256, rand() % 256, rand() % 256)));
		}

		for (auto it = bouncers.begin(); it < bouncers.end(); it++)
		{
			if ((*it).nbBounces < 0)
			{
				continue;
			}
			DrawCircle(olc::vi2d((*it).position), int32_t((*it).size), (*it).color);

			UpdatePhysics((*it), fElapsedTime);
			
		}

		return true;
	}

	void UpdatePhysics(BounceOject& bouncer, float fElapsedTime)
	{
		bouncer.dPosition.y += gravity;
		bouncer.position += bouncer.dPosition * fElapsedTime;

		if (bouncer.position.y + bouncer.size > ScreenHeight())
		{
			bouncer.dPosition.y *= -bouncer.bounciness;
			bouncer.position.y = ScreenHeight() - bouncer.size;
			bouncer.nbBounces--;
		}

		if (bouncer.position.y - bouncer.size < 0)
		{
			bouncer.dPosition.y *= -bouncer.bounciness;
			bouncer.position.y = bouncer.size;
			bouncer.nbBounces--;
		}

		if (bouncer.position.x - bouncer.size < 0)
		{
			bouncer.dPosition.x *= -bouncer.bounciness;
			bouncer.position.x = bouncer.size;
			bouncer.nbBounces--;
		}

		if (bouncer.position.x + bouncer.size > ScreenWidth())
		{
			bouncer.dPosition.x *= -bouncer.bounciness;
			bouncer.position.x = ScreenWidth() - bouncer.size;
			bouncer.nbBounces--;
		}
	}

	float gravity = 0.2f;
	std::vector<BounceOject> bouncers;
};

class Cave : public olc::PixelGameEngine
{
public:
	Cave()
	{
		// Name your application
		sAppName = "Cave Generator";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here

		output = new uint8_t[int(ScreenWidth() * ScreenHeight())];
		state = new uint8_t[int(ScreenWidth() * ScreenHeight())];

		memset(output, 0, int(ScreenWidth() * ScreenHeight()) * sizeof(uint8_t));
		memset(state, 0, int(ScreenWidth() * ScreenHeight()) * sizeof(uint8_t));

		randomInit();

		DrawCave();

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		//std::this_thread::sleep_for(std::chrono::milliseconds(100));

		if (GetKey(olc::Key::R).bReleased)
		{
			randomInit();
			generation = 0;
		}

		if (!GetKey(olc::Key::SPACE).bReleased)
		{
			return true;
		}

		DrawCave();

		int offset = convolutionSize / 2;

		//std::cout << offset << "\n";

		for (int x = 1 ; x < ScreenWidth() - 1; x++)
		{
			for (int y = 1; y < ScreenHeight() - 1; y++)
			{
				int sum = 0;
				for (int kx = -offset; kx <= offset; kx++)
				{
					for (int ky = -offset; ky <= offset; ky++)
					{
						if(!(kx + x == x && ky + y  == y))
						{
							sum += state[(y + ky) * ScreenWidth() + x + kx];
						}

						//std::cout << ky << "\n";
					}
				}

				if ((state[y * ScreenWidth() + x] == Dead) && (sum >= 6))
				{
					output[y * ScreenWidth() + x] = Alive;
				}

				if ((state[y * ScreenWidth() + x] == Alive) && (sum <= 3))
				{
					output[y * ScreenWidth() + x] = Dead;
				}

			}
		}

		generation++;

		memcpy(state, output, ScreenHeight() * ScreenWidth() * sizeof(uint8_t));

		return true;
	}

	void randomInit()
	{

		for (int x = 0; x < ScreenWidth(); x++)
		{
			for (int y = 0; y < ScreenHeight(); y++)
			{
				if (x == 0 || y == 0 || x == ScreenWidth() - 1 || y == ScreenHeight() - 1)
				{
					state[y * ScreenWidth() + x] = Alive;
					output[y * ScreenWidth() + x] = Alive;
				}
				else
				{
					if (hash(rand()) % 100 <= prob)
					{
						state[y * ScreenWidth() + x] = Alive;
						output[y * ScreenWidth() + x] = Alive;
					}
					else
					{  
						state[y * ScreenWidth() + x] = Dead;
						output[y * ScreenWidth() + x] = Dead;
					}
				}
			}
		}
	}


	void DrawCave()
	{
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
			{
				if (state[y * ScreenWidth() + x] == Alive)
				{
					Draw(x, y, olc::BLACK);
				}
				else
				{
					Draw(x, y, olc::WHITE);
				}

			}
	}

	int hash(int state)
	{
		state ^= 2747636419u;
		state *= 2654435769u;
		state ^= state >> 16;
		state *= 2654435769u;
		state ^= state >> 16;
		state *= 2654435769u;
		return state;
	}

	const uint8_t Alive = 1;
	const uint8_t Dead = 0;

	uint8_t* output;
	uint8_t* state;
	uint8_t prob = 25;
	uint32_t generation = 0;
	int convolutionSize = 3;
};

int main(int argc, char* argv[])
{
	srand(time(NULL));

	TAUDoom demo;
	//ImageProcessor demo;
	//WaveFC demo;
	////SandSimulation demo;
	//ProceduralAnim demo;
	//YamScoreBoard demo;
	if (demo.Construct(1920, 1080, 1, 1, false))
	{
		demo.Start();
		//demo.olc_UpdateMouseFocus(true);
		//demo.olc_UpdateKeyFocus(true);
	}
	return 0;
}
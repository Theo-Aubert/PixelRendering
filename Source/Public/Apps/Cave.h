#pragma once
#include "../AppManager.h"

class Cave : public PixelApp
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
REGISTER_CLASS(Cave)
#pragma once
#include "../AppManager.h"

class CellularAutomaton : public PixelApp
{
public:
    CellularAutomaton()
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
REGISTER_CLASS(CellularAutomaton)
#define OLC_PGE_APPLICATION
#include "Externals/olcPixelGameEngine.h"
#include "Public/AppManager.h"

// Override base class with your custom functionality
// class Example : public olc::PixelGameEngine
// {
// public:
// 	Example()
// 	{
// 		// Name your application
// 		sAppName = "Example";
// 	}
//
// public:
// 	bool OnUserCreate() override
// 	{
// 		// Called once at the start, so create things here
// 		return true;
// 	}
//
// 	bool OnUserUpdate(float fElapsedTime) override
// 	{
// 		// Called once per frame, draws random coloured pixels
// 		for (int x = 0; x < ScreenWidth(); x++)
// 			for (int y = 0; y < ScreenHeight(); y++)
// 				Draw(x, y, olc::Pixel(rand() % 256, rand() % 256, rand() % 256));
// 		return true;
// 	}
// };
//
// int main(int argc, char* argv[])
// {
// 	srand(time(NULL));
// 	
// 	AppManager::Run();
// 	return 0;
// }

#include "Public/Widgets/headers/gui.h"

bool first_run = true;
bool settings_finished = false;

class SettingsPrompt : public olc::PixelGameEngine
{
public:
	SettingsPrompt()
	{
		sAppName = "GAME NAME HERE";
	}

	GUI gui;
public:
	bool OnUserCreate() override
	{
		if (first_run)
			gui.create();
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);

		if (GetKey(olc::ESCAPE).bPressed)
			return false;
        
		if (first_run)
		{
			if (!gui.run())
			{
				first_run = false;
				settings_finished = true;
				return false;
			}
			else
				return true;
		}
		return true;
	}
};

int main()
{
	SettingsPrompt* settings_prompt = new SettingsPrompt;

	if (settings_prompt->Construct(1200, 750, 1, 1, false))
		settings_prompt->Start();

	delete settings_prompt;

	if (settings_finished)
	{
		SettingsPrompt* main_program = new SettingsPrompt;

		std::ifstream file("settings.json");
		json settings = json::parse(file);

		if (settings["custom_title"].size() > 0)
			main_program->sAppName = settings["custom_title"];

		if (main_program->Construct(settings["resolution_width"], settings["resolution_height"], 1, 1, settings["fullscreen"]))
			main_program->Start();
	}
	return 0;
}
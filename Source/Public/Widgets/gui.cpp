#define OLC_PGEX_FUI
#include "headers/gui.h"


void GUI::create()
{
    //buttons.Load("textures/buttons.png");
    texture_position.push_back({ 1, 73 });
    texture_position.push_back({ 1, 37 });
    texture_position.push_back({ 1, 1 });

    frost_ui.create_window("window", "Console Window", { 100, 100 }, { 300, 200 });
    frost_ui.find_window("window")->disable_close(true);
    frost_ui.add_console("window", "console", "Console", { 0, 76 }, { 290, 100 }, 20);
    // command handler is mandatory to have for console window, if you don't want to execute any actions simply do '*return_msg = command'
    // for it to print out any text you type into the input field of the console
    frost_ui.find_element("console")->add_command_handler([](std::string& command, std::string* return_msg) { *return_msg = command; });

    frost_ui.add_groupbox("groupbox1", "", { 0, 0 }, { 200, 250 });

    frost_ui.add_dropdown("resolution", "Resolution: ", { 80, 10 }, { 100, 20 });
    frost_ui.find_element("resolution")->add_item("1920x1080");
    frost_ui.find_element("resolution")->add_item("1280x720");
    frost_ui.find_element("resolution")->add_item("800x600");
    frost_ui.find_element("resolution")->add_item("640x360");
    frost_ui.find_element("resolution")->set_default_item(0);
    frost_ui.find_element("resolution")->set_animation_speed(1000);
    frost_ui.find_element("resolution")->set_max_display_items(4);

    frost_ui.add_dropdown("quality", "Quality: ", { 80, 40 }, { 100, 20 });
    frost_ui.find_element("quality")->add_item("High");
    frost_ui.find_element("quality")->add_item("Medium");
    frost_ui.find_element("quality")->add_item("Low");
    frost_ui.find_element("quality")->add_item("High");
    frost_ui.find_element("quality")->add_item("Medium");
    frost_ui.find_element("quality")->add_item("Low");
    frost_ui.find_element("quality")->add_item("High");
    frost_ui.find_element("quality")->add_item("Medium");
    frost_ui.find_element("quality")->add_item("Low");
    frost_ui.find_element("quality")->set_default_item(0);
    frost_ui.find_element("quality")->set_animation_speed(1000);
    frost_ui.find_element("quality")->set_max_display_items(5);

    frost_ui.add_checkbox("fullscreen", "Fullscreen: ", { 80, 70 }, { 20, 20 });

    frost_ui.add_inputfield("custom_title", "Custom title: ", { 80, 100 }, { 100, 20 });
    frost_ui.find_element("custom_title")->inputfield_scale({ 1.25f, 1.25f });

    frost_ui.add_button("launch", "Launch game", { 10, 130 }, { 180, 20 }, [&]
        {
            
            auto res = frost_ui.find_element("resolution")->get_selected_item();
            auto qual = frost_ui.find_element("quality")->get_selected_item();
            auto title = frost_ui.find_element("custom_title")->get_inputfield_value();
            auto fullscreen = frost_ui.find_element("fullscreen")->get_checkbox_state();
            switch (res)
            {
            case 0:
                screen_width = 1920;
                screen_height = 1080;
                break;
            case 1:
                screen_width = 1280;
                screen_height = 720;
                break;
            case 2:
                screen_width = 800;
                screen_height = 600;
                break;
            case 3:
                screen_width = 640;
                screen_height = 360;
                break;
            }

            switch (qual)
            {
            case 0:
                quality = 0;
                break;
            case 1:
                quality = 1;
                break;
            case 2:
                quality = 2;
                break;
            }

            json settings;

            settings["resolution_width"] = screen_width;
            settings["resolution_height"] = screen_height;
            settings["quality"] = quality;
            settings["fullscreen"] = fullscreen;
            settings["custom_title"] = title;

            std::ofstream file("settings.json");

            file << settings.dump(4);

            run_return = false;
            

            auto x = frost_ui.find_element("slider")->get_slider_value<int>();
            auto y = frost_ui.find_element("slider1")->get_slider_value<float>();
            auto k = frost_ui.find_element("fullscreen")->get_checkbox_state();
            auto j = frost_ui.find_element("test")->get_button_state();

            std::cout << x << " - " << y << " - " << k << " - " << j << "\n";
        });
    //frost_ui.find_element("launch")->add_texture(buttons.Decal(), texture_position, { 276, 34 });

    frost_ui.add_button("window", "test", "Testing", { 5, 5 }, { 100, 20 }, []() {});
    frost_ui.find_element("test")->make_toggleable(true);
    frost_ui.find_element("resolution")->set_colors({ olc::Pixel(120, 120, 120), olc::Pixel(100, 100, 100), olc::Pixel(80, 80, 80)});

    frost_ui.add_int_slider("slider", "Testing: ", { 80, 160 }, { 100, 10 }, { -5, 5 });
    frost_ui.find_element("slider")->set_slider_value(2000); // the set_slider_value function will clamp this value automatically
    frost_ui.add_float_slider("slider1", "Testing 2: ", { 80, 190 }, { 100, 10 }, { -155, 372 });
    frost_ui.find_element("slider1")->set_slider_value(100000.f); // .f will indicate that this is a float value (this function is templated to avoid implicit casting)
}

bool GUI::run()
{
    frost_ui.run();

    return run_return;
}
#pragma once
#include <ranges>
#include "PixelApp.h"
#include <memory>

constexpr std::string DefaultAppName = "PixelGallery";

class AppManager
{
public:
    
    using CreatorFunc = std::unique_ptr<PixelApp>(*)();
    
    static void Init();
    static bool Run();

    static void RegisterApp(const std::string& sAppName, CreatorFunc creator)
    {
        if(!GetAppRegistry().contains(sAppName))
            GetAppRegistry().insert(std::make_pair(sAppName,creator));
    }

    static std::vector<std::string> GetRegisteredApps();

    static std::unique_ptr<PixelApp> Create(const std::string& sAppName);

    static bool LaunchApp(const std::string& sAppName);

    static void ReceiveExit(const std::string& sAppName);
    static void SignalNextApp(const std::string& sAppName);

private:

    static std::unordered_map<std::string, CreatorFunc>& GetAppRegistry()
    {
        static std::unordered_map<std::string, CreatorFunc> AppRegistry;
        return AppRegistry;
    }

    //static std::unordered_map<std::string, CreaterFunc> AppRegistry;

    static std::unique_ptr<PixelApp> AppBuffer[2];
    static int iAppIndex;
    static std::string sExitingApp;
    static std::string sNextApp;
    static std::string sCurrentApp;

    static bool bIsRunning;
};

// Macro pour enregistrer les classes automatiquement
#define REGISTER_CLASS(CLASS_NAME) \
    static bool _registered_##CLASS_NAME = []() { \
        AppManager::RegisterApp(#CLASS_NAME, []() -> std::unique_ptr<PixelApp> {\
            std::unique_ptr<CLASS_NAME> app = std::make_unique<CLASS_NAME>();\
            app->sAppName = #CLASS_NAME; \
            return app; \
        }); \
        return true; \
    }();

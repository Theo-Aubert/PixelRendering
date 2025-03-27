#include "../Public/AppManager.h"

std::unique_ptr<PixelApp> AppManager::AppBuffer[2] = { nullptr, nullptr};
int AppManager::iAppIndex = 0;
std::string AppManager::sExitingApp = "" ;
std::string AppManager::sNextApp = DefaultAppName;
std::string AppManager::sCurrentApp = "" ;
bool AppManager::bIsRunning = true;

void AppManager::Init()
{
    LaunchApp(DefaultAppName);
}

bool AppManager::Run()
{
    while(bIsRunning && !sNextApp.empty() && sNextApp != sCurrentApp)
    {
        sCurrentApp = sNextApp;
        sNextApp = "";
        LaunchApp(sCurrentApp);
        
    }

    return true;
}

std::vector<std::string> AppManager::GetRegisteredApps()
{
    std::vector<std::string> names;
    for (const auto& [name, _] : GetAppRegistry()) {
        names.push_back(name);
    }
    return names;
}

std::unique_ptr<PixelApp> AppManager::Create(const std::string& sAppName)
{
    auto it = GetAppRegistry().find(sAppName);
    return (it!=GetAppRegistry().end()) ? it->second() : nullptr;
}

bool AppManager::LaunchApp(const std::string& sAppName)
{
    auto pLaunchedApp = Create(sAppName);
    if(pLaunchedApp != nullptr)
    {
        if(pLaunchedApp->Construct(1920, 1080, 1, 1, false))
        {
            iAppIndex++;
            iAppIndex = iAppIndex % 2;
            AppBuffer[iAppIndex] = std::move(pLaunchedApp);
            return AppBuffer[iAppIndex]->Start();
        }
        
        return false;
    }

    return false;
}

void AppManager::ReceiveExit(const std::string& sAppName)
{
    sExitingApp = sAppName;

    if(sExitingApp != DefaultAppName)
        SignalNextApp(DefaultAppName);
        
}

void AppManager::SignalNextApp(const std::string& sAppName)
{
    sNextApp = sAppName;
}

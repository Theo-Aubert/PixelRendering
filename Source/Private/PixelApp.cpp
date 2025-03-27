#include "../Public/PixelApp.h"

#include "../Public/AppManager.h"


bool PixelApp::OnUserDestroy()
{
    NotifyExit();
    return true;
}

void PixelApp::NotifyExit() const
{
    AppManager::ReceiveExit(sAppName);
}

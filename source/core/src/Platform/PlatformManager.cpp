#include "Platform/PlatformManager.h"

PlatformManager* PlatformManager::platformManagerInstance = nullptr;

void PlatformManager::Init()
{

}

void PlatformManager::DeInit()
{
}

PlatformManager * PlatformManager::GetSingleton()
{
    if (platformManagerInstance == nullptr)
    {
        platformManagerInstance = new PlatformManager();
    }
    return platformManagerInstance;

}

PlatformManager::~PlatformManager()
{
}

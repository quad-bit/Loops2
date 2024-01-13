#include "Utility/SceneLoader.h"
#include "ECS/ECS_Setting.h"

Core::Utility::GltfLoader::GltfLoader(Core::ECS::World* worldObj)
{
    m_ecsWorldObj = worldObj;
}

void Core::Utility::GltfLoader::LoadGltf(const std::string& gltfPath, std::vector<Core::ECS::EntityHandle*>& entityList)
{
    // load file from gltfPath

    // create entity and fill entityList
}

Core::Utility::GltfLoader::~GltfLoader()
{
}

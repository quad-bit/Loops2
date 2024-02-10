#include "resourceManagement/MaterialFactory.h"
#include "Utility/HashManager.h"
#include "Platform/Assertion.h"
//#include "ShaderFactory.h"
#include "Utility/RenderingWrappers/RenderingWrapper.h"
#include "ECS/Events/EventBus.h"
#include "ECS/Events/ComponentAdditionEvent.h"

Renderer::ResourceManagement::MaterialFactory* Renderer::ResourceManagement::MaterialFactory::instance = nullptr;

uint32_t Renderer::ResourceManagement::MaterialFactory::GetMatId()
{
    return matIdCounter++;
}

void Renderer::ResourceManagement::MaterialFactory::DecrementMatCounter()
{
    matIdCounter--;
}

void Renderer::ResourceManagement::MaterialFactory::Init()
{

}

void Renderer::ResourceManagement::MaterialFactory::DeInit()
{
    for each (auto const& var in idToMaterialMap)
    {
        delete var.second;
    }
    idToMaterialMap.clear();
}

void Renderer::ResourceManagement::MaterialFactory::Update()
{

}

Renderer::ResourceManagement::MaterialFactory * Renderer::ResourceManagement::MaterialFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new Renderer::ResourceManagement::MaterialFactory();
    }
    return instance;
}
          
Renderer::ResourceManagement::MaterialFactory::~MaterialFactory()
{
}

Core::ECS::Components::Material* Renderer::ResourceManagement::MaterialFactory::CreateMaterial(const Core::ECS::Components::MaterialCreateInfo& info)
{
    uint32_t matId = GetMatId();

    Core::ECS::Components::Material* mat = new Core::ECS::Components::Material(info);
    mat->componentId = matId;

    idToMaterialMap.insert(std::pair<uint32_t, Core::ECS::Components::Material*>(
        { matId, mat }));

    Core::ECS::Events::MaterialCreationEvent eve;
    eve.material = mat;
    Core::ECS::Events::EventBus::GetInstance()->Publish(&eve);

    return mat;
}

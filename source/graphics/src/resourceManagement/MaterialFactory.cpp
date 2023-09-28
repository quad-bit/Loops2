#include "resourceManagement/MaterialFactory.h"
#include "Utility/HashManager.h"
#include "Platform/Assertion.h"
//#include "ShaderFactory.h"
#include "Utility/RenderingWrappers/RenderingWrapper.h"
//#include "EventBus.h"
//#include "MeshAdditionEvent.h"

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

Core::ECS::Components::Material* Renderer::ResourceManagement::MaterialFactory::CreateMaterial(const std::string& effectName)
{
    uint32_t matId = GetMatId();

    Core::ECS::Components::Material* mat = new Core::ECS::Components::Material(effectName);
    mat->componentId = matId;

    idToMaterialMap.insert(std::pair<uint32_t, Core::ECS::Components::Material*>(
        { matId, mat }));
    return mat;
}

Core::ECS::Components::Material* Renderer::ResourceManagement::MaterialFactory::CreateMaterial(const Core::ECS::Components::EffectType& effectType)
{
    uint32_t matId = GetMatId();

    Core::ECS::Components::Material* mat = new Core::ECS::Components::Material(effectType);
    mat->componentId = matId;

    idToMaterialMap.insert(std::pair<uint32_t, Core::ECS::Components::Material*>(
        { matId, mat }));

    return mat;
}

#if 0
Material * Renderer::ResourceManagement::MaterialFactory::CreateMaterial(ShaderDescription * shaderDescriptions, const uint32_t & shaderCount, 
    const uint32_t & meshId, const RenderPassTag & tag)
{
    std::vector<std::string> shaderNames;
    shaderNames.resize(shaderCount);

    for (uint32_t i = 0; i < shaderCount; i++)
    {
        shaderNames[i] = shaderDescriptions[i].shaderName;
    }
    
    uint32_t matId = GetMatId();

    // check for hash (shader names)
    int id = HashManager::GetInstance()->FindMaterialHash(&shaderNames, matId);
    Material * mat;
    MatSetWrapper matSetWrapper;

    if (id == -1)
    {
        // use matid and create a material
        Shader * shaders = new Shader[shaderCount];
        for (uint32_t i = 0; i < shaderCount; i++)
        {
            shaders[i].shaderType = shaderDescriptions[i].type;
            shaders[i].shaderName = shaderDescriptions[i].shaderName;
        }
        
        std::vector<SetWrapper*> setWrapperList =  ShaderFactory::GetInstance()->CreateShader(meshId, shaders, shaderCount, tag);
        
        mat = new Material(shaders, shaderCount);
        idToMaterialMap.insert(std::pair<uint32_t, Material*>(
            { matId, mat }));
        
        // get the setWrappers for the shader combination
        mat->resourceLayoutList = setWrapperList;
        mat->meshList.push_back(meshId);
        // using the set wrappers create shader resources
        CreateSetResources(setWrapperList);
        mat->componentId = matId;

        matSetWrapper.mat = mat;
        matSetWrapper.wrapperList = setWrapperList;
        matSetWrapperList.push_back(matSetWrapper);
        
        MeshToMatAdditionEvent event;
        event.meshId = meshId;
        event.setWrapperList = mat->resourceLayoutList;
        EventBus::GetInstance()->Publish(&event);
    }
    else
    {
        if (idToMaterialMap.find(id) != idToMaterialMap.end())
        {
            mat = idToMaterialMap[id];
        }
        else
        {
            ASSERT_MSG_DEBUG(0, "Mat not found");
        }

        DecrementMatCounter();
        // if it exists then add the meshid to the list
        AddMeshIds(mat, meshId);
    }
    
    return mat;
}

Material * Renderer::ResourceManagement::MaterialFactory::CreateMaterial(ShaderDescription * shaderDescriptions, const uint32_t & shaderCount, const uint32_t & meshId, const uint16_t & tagMask)
{
    std::vector<std::string> shaderNames;
    shaderNames.resize(shaderCount);

    for (uint32_t i = 0; i < shaderCount; i++)
    {
        shaderNames[i] = shaderDescriptions[i].shaderName;
    }

    uint32_t matId = GetMatId();

    // check for hash (shader names)
    int id = HashManager::GetInstance()->FindMaterialHash(&shaderNames, matId);
    Material * mat;
    MatSetWrapper matSetWrapper;

    if (id == -1)
    {
        // use matid and create a material
        Shader * shaders = new Shader[shaderCount];
        for (uint32_t i = 0; i < shaderCount; i++)
        {
            shaders[i].shaderType = shaderDescriptions[i].type;
            shaders[i].shaderName = shaderDescriptions[i].shaderName;
        }

        std::vector<SetWrapper*> setWrapperList = ShaderFactory::GetInstance()->CreateShader(meshId, shaders, shaderCount, tagMask);

        mat = new Material(shaders, shaderCount);
        idToMaterialMap.insert(std::pair<uint32_t, Material*>(
        { matId, mat }));

        // get the setWrappers for the shader combination
        mat->resourceLayoutList = setWrapperList;
        mat->meshList.push_back(meshId);
        // using the set wrappers create shader resources
        CreateSetResources(setWrapperList);
        mat->componentId = matId;

        matSetWrapper.mat = mat;
        matSetWrapper.wrapperList = setWrapperList;
        matSetWrapperList.push_back(matSetWrapper);

        MeshToMatAdditionEvent event;
        event.meshId = meshId;
        event.setWrapperList = mat->resourceLayoutList;
        EventBus::GetInstance()->Publish(&event);
    }
    else
    {
        if (idToMaterialMap.find(id) != idToMaterialMap.end())
        {
            mat = idToMaterialMap[id];
        }
        else
        {
            ASSERT_MSG_DEBUG(0, "Mat not found");
        }

        DecrementMatCounter();
        // if it exists then add the meshid to the list
        AddMeshIds(mat, meshId);
    }

    return mat;
}

void Renderer::ResourceManagement::MaterialFactory::AddMeshIds(Material * mat, const uint32_t & meshId)
{
    ShaderFactory::GetInstance()->AddMeshToShader(meshId, mat->shaders, mat->numShaders);
    mat->meshList.push_back(meshId);

    MeshToMatAdditionEvent event;
    event.meshId = meshId;
    event.setWrapperList = mat->resourceLayoutList;
    EventBus::GetInstance()->Publish(&event);
}

std::vector<uint32_t> Renderer::ResourceManagement::MaterialFactory::GetMeshList(SetWrapper * setwrapper, const uint32_t & setWrapperCount)
{
    std::vector<uint32_t> meshList;

    for (uint32_t i = 0; i < setWrapperCount; i++)
    {
        SetWrapper * wrapper = &setwrapper[i];
        for each(auto obj in matSetWrapperList )
        {
            std::vector<SetWrapper *>::iterator it;
            it = std::find_if(obj.wrapperList.begin(), obj.wrapperList.end(), [=](SetWrapper * e) { 
                return e->id == wrapper->id && e->setValue == wrapper->setValue; });

            if (it != obj.wrapperList.end())
            {
                // found
                for each(auto meshId in obj.mat->meshList)
                {
                    std::vector<uint32_t>::iterator itt;
                    itt = std::find_if(obj.mat->meshList.begin(), obj.mat->meshList.end(), [=](uint32_t e) {
                        return e == meshId;
                    });

                    if (itt != obj.mat->meshList.end())
                    {
                        meshList.push_back(*itt);
                    }
                }
            }
        }
    }

    return meshList;
}
#endif
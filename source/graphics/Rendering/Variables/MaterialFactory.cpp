#include "MaterialFactory.h"
#include "HashManager.h"
#include "Assertion.h"
#include "ShaderFactory.h"
#include "RenderingWrapper.h"
#include "EventBus.h"
#include "MeshAdditionEvent.h"

MaterialFactory* MaterialFactory::instance = nullptr;

uint32_t MaterialFactory::GetMatId()
{
    return matIdCounter++;
}

void MaterialFactory::DecrementMatCounter()
{
    matIdCounter--;
}

void MaterialFactory::CreateSetResources(std::vector<SetWrapper*> setWrapperList)
{
    // Iterate through all the wrappers check if the corresponding resources have been 
    // created for each set
}

void MaterialFactory::Init()
{

}

void MaterialFactory::DeInit()
{
    for each (auto const& var in idToMaterialMap)
    {
        delete var.second;
    }
    idToMaterialMap.clear();
}

void MaterialFactory::Update()
{

}

MaterialFactory * MaterialFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new MaterialFactory();
    }
    return instance;
}
          
MaterialFactory::~MaterialFactory()
{
}

Material * MaterialFactory::CreateMaterial(ShaderDescription * shaderDescriptions, const uint32_t & shaderCount, 
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

Material * MaterialFactory::CreateMaterial(ShaderDescription * shaderDescriptions, const uint32_t & shaderCount, const uint32_t & meshId, const uint16_t & tagMask)
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

void MaterialFactory::AddMeshIds(Material * mat, const uint32_t & meshId)
{
    ShaderFactory::GetInstance()->AddMeshToShader(meshId, mat->shaders, mat->numShaders);
    mat->meshList.push_back(meshId);

    MeshToMatAdditionEvent event;
    event.meshId = meshId;
    event.setWrapperList = mat->resourceLayoutList;
    EventBus::GetInstance()->Publish(&event);
}

std::vector<uint32_t> MaterialFactory::GetMeshList(SetWrapper * setwrapper, const uint32_t & setWrapperCount)
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

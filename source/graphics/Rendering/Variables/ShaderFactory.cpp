#include "ShaderFactory.h"
#include "AttributeHelper.h"
#include <CorePrecompiled.h>
#include <algorithm>
#include <Shader.h>
#include <GraphicsPipelineManager.h>
#include "PlatformSettings.h"

#if (RENDERING_API == VULKAN)
#include <VulkanInterface.h>
#elif (RENDERING_API == DX)
#include <DxInterface.h>
#endif

ShaderFactory* ShaderFactory::instance = nullptr;

ShaderFactory * ShaderFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new ShaderFactory();
    }
    return instance;
}

ShaderFactory::~ShaderFactory()
{
}

std::vector<SetWrapper*> ShaderFactory::CreateShader(const uint32_t & meshId, Shader * shaders, const uint32_t & shaderCount, const RenderPassTag & tag)
{
    // Get the ids for each shader object.
    char ** shaderNames = new char*[shaderCount];
    ShaderType * types = new ShaderType[shaderCount];
    uint32_t * ids = new uint32_t[shaderCount];

    for (uint32_t i = 0; i < shaderCount; i++)
    {
        shaderNames[i] = const_cast<char*>(shaders[i].shaderName.c_str());
        types[i] = shaders[i].shaderType;
    }

    apiInterface->GetShaderIds(shaderNames, types, ids, shaderCount);

    for (uint32_t i = 0; i < shaderCount; i++)
    {
        shaders[i].shaderId = ids[i];
    }
    shaderList.push_back(shaders);

    GraphicsPipelineManager<ApiInterface>::GetInstance()->CreatShaderPipelineState(meshId, shaders, shaderCount, tag);
    std::vector<SetWrapper*> list = GraphicsPipelineManager<ApiInterface>::GetInstance()->CreatResourceLayoutState(meshId, shaders, shaderCount);

    delete[] ids;
    delete[] types;
    delete[] shaderNames;

    return list;
}

std::vector<SetWrapper*> ShaderFactory::CreateShader(const uint32_t & meshId, Shader * shaders, const uint32_t & shaderCount, const uint16_t & tagMask)
{
    // Get the ids for each shader object.
    char ** shaderNames = new char*[shaderCount];
    ShaderType * types = new ShaderType[shaderCount];
    uint32_t * ids = new uint32_t[shaderCount];

    for (uint32_t i = 0; i < shaderCount; i++)
    {
        shaderNames[i] = const_cast<char*>(shaders[i].shaderName.c_str());
        types[i] = shaders[i].shaderType;
    }

    apiInterface->GetShaderIds(shaderNames, types, ids, shaderCount);

    for (uint32_t i = 0; i < shaderCount; i++)
    {
        shaders[i].shaderId = ids[i];
    }
    shaderList.push_back(shaders);

    GraphicsPipelineManager<ApiInterface>::GetInstance()->CreatShaderPipelineState(meshId, shaders, shaderCount, tagMask);
    std::vector<SetWrapper*> list = GraphicsPipelineManager<ApiInterface>::GetInstance()->CreatResourceLayoutState(meshId, shaders, shaderCount);

    delete[] ids;
    delete[] types;
    delete[] shaderNames;

    return list;
}

void ShaderFactory::AddMeshToShader(const uint32_t & meshId, Shader * shaders, const uint32_t & shaderCount)
{
    GraphicsPipelineManager<ApiInterface>::GetInstance()->CreatShaderPipelineState(meshId, shaders, shaderCount);
    std::vector<SetWrapper*> list = GraphicsPipelineManager<ApiInterface>::GetInstance()->CreatResourceLayoutState(meshId, shaders, shaderCount);
}

void ShaderFactory::Init(ApiInterface * apiInterface)
{
    this->apiInterface = apiInterface;
}

void ShaderFactory::DeInit()
{
    // as shader are getting stored as array within material, it getting deleted in material
    for each (auto var in shaderList)
    {
        delete[] var;
    }
    shaderList.clear();
}

void ShaderFactory::Update()
{
}


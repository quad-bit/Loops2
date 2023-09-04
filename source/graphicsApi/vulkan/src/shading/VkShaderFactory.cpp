#include "shading/VkShaderFactory.h"
#include <iostream>
#include <IO/FileReader.h>
#include <Utility/RenderingWrappers/RenderingWrapper.h>
#include <Platform/Assertion.h>
#include "utility/VulkanUtility.h"
#include "utility/VulkanSettings.h"

GfxVk::Shading::VkShaderFactory* GfxVk::Shading::VkShaderFactory::instance = nullptr;

void GfxVk::Shading::VkShaderFactory::LoadSpv()
{
}

uint32_t GfxVk::Shading::VkShaderFactory::AssignId()
{
    return idCounter++;
}

VkShaderModule * GfxVk::Shading::VkShaderFactory::CreateShaderModule(uint32_t * shaderCode, size_t codeLength)
{
    VkShaderModule * shaderModuleObj = new VkShaderModule;

    VkShaderModuleCreateInfo createInfo{};
    createInfo.codeSize = codeLength;
    createInfo.pCode = shaderCode;
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    GfxVk::Utility::ErrorCheck(vkCreateShaderModule(*GfxVk::Utility::CoreObjects::logicalDeviceObj, &createInfo, GfxVk::Utility::CoreObjects::pAllocator, shaderModuleObj));

    return shaderModuleObj;
}

void GfxVk::Shading::VkShaderFactory::GetSpvNameList(const std::vector<std::string> & fileNameList)
{
    for (const auto & entry : fileNameList)
    {
        if( entry.find(".spv") != std::string::npos)
            spvNames.push_back(entry);
    }
}

std::string GfxVk::Shading::VkShaderFactory::GetSpvName(const std::string & shaderName)
{
    std::string fileNameWithoutExt;
    size_t lastindex = shaderName.find_last_of(".");
    fileNameWithoutExt = shaderName.substr(0, lastindex);

    if (shaderName.find(".frag") != std::string::npos)
    {
        return fileNameWithoutExt +".frag.spv";
    }
    else if (shaderName.find(".vert") != std::string::npos)
    {
        return fileNameWithoutExt +".vert.spv";
    }
    
    ASSERT_MSG_DEBUG(0, "Invalid shader name");
    return "NULL";
}

void GfxVk::Shading::VkShaderFactory::Init()
{
    std::vector<std::string> fileList;
    Core::IO::GetFilesInFolder(VULKAN_ASSETS_PATH + std::string{ "/Spvs" }, fileList);
    
    for each (std::string var in fileList)
    {
        // if its not spv return
        if (var.find(".spv") == std::string::npos)
            continue;

        // load the text
        size_t fileLength = 0;
        const char* shaderText = Core::IO::ReadSpvFile((uint32_t&)fileLength, var.c_str());
        
        ShaderModuleWrapper * wrapper = new ShaderModuleWrapper;

        // if path contains frag then it fragment shader
        if (var.find("frag") != std::string::npos)
        {
            wrapper->shaderType = new Core::Enums::ShaderType{ Core::Enums::ShaderType::FRAGMENT };
            wrapper->stageFlag = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
        }
        else if (var.find("vert") != std::string::npos)
        {
            wrapper->shaderType = new Core::Enums::ShaderType{ Core::Enums::ShaderType::VERTEX };
            wrapper->stageFlag = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
        }
        
        wrapper->shaderName = var;
        wrapper->shaderId = AssignId();
        wrapper->module = CreateShaderModule((uint32_t*)shaderText, (size_t)fileLength );

        // add the module to list
        shaderModuleList.push_back(wrapper);
    }
}

void GfxVk::Shading::VkShaderFactory::DeInit()
{
    for each (ShaderModuleWrapper* var in shaderModuleList)
    {
        vkDestroyShaderModule(*GfxVk::Utility::CoreObjects::logicalDeviceObj, *var->module, GfxVk::Utility::CoreObjects::pAllocator);
        delete var->module;
        delete var->shaderType;
        delete var;
    }

    shaderModuleList.clear();
}

void GfxVk::Shading::VkShaderFactory::Update()
{

}

GfxVk::Shading::VkShaderFactory* GfxVk::Shading::VkShaderFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new GfxVk::Shading::VkShaderFactory();
    }
    return instance;
}

GfxVk::Shading::VkShaderFactory::~VkShaderFactory()
{
}

void GfxVk::Shading::VkShaderFactory::GetShaderIds( char ** shaderName, Core::Enums::ShaderType * type, uint32_t * id, const uint32_t & shaderCount)
{
    std::vector<ShaderModuleWrapper *>::iterator it;

    for (uint32_t i = 0; i < shaderCount; i++)
    {
        std::string spvName = GetSpvName(shaderName[i]);
        it = std::find_if(shaderModuleList.begin(), shaderModuleList.end(), [&](ShaderModuleWrapper * wrapper) { 
            return ( wrapper->shaderName.find (spvName) != std::string::npos &&
                 *wrapper->shaderType == type[i]); });

        ASSERT_MSG_DEBUG(it != shaderModuleList.end(), "Shader not found");
        id[i] = (*it)->shaderId;
    }
}

VkShaderModule * GfxVk::Shading::VkShaderFactory::GetShaderModule(const uint32_t & id)
{
    std::vector<ShaderModuleWrapper *>::iterator it;
    for (it = shaderModuleList.begin(); it != shaderModuleList.end(); it++)
    {
        if ((*it)->shaderId == id)
            return (*it)->module;
    }
    
    ASSERT_MSG_DEBUG(0, "shader not found");
    return nullptr;
}

VkShaderStageFlagBits GfxVk::Shading::VkShaderFactory::GetShaderStageFlag(const uint32_t & id)
{
    std::vector<ShaderModuleWrapper *>::iterator it;
    for (it = shaderModuleList.begin(); it != shaderModuleList.end(); it++)
    {
        if ((*it)->shaderId == id)
            return (*it)->stageFlag;
    }

    ASSERT_MSG_DEBUG(0, "shader not found");
    return VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
}

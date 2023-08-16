#include "VkShaderFactory.h"
#include <iostream>
#include <FileReader.h>
#include <RenderingWrapper.h>
#include <Assertion.h>
#include "VulkanUtility.h"
#include "VulkanSettings.h"

VkShaderFactory* VkShaderFactory::instance = nullptr;

void VkShaderFactory::LoadSpv()
{
}

uint32_t VkShaderFactory::AssignId()
{
    return idCounter++;
}

VkShaderModule * VkShaderFactory::CreateShaderModule(uint32_t * shaderCode, size_t codeLength)
{
    VkShaderModule * shaderModuleObj = new VkShaderModule;

    VkShaderModuleCreateInfo createInfo{};
    createInfo.codeSize = codeLength;
    createInfo.pCode = shaderCode;
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    ErrorCheck(vkCreateShaderModule(*CoreObjects::logicalDeviceObj, &createInfo, CoreObjects::pAllocator, shaderModuleObj));

    return shaderModuleObj;
}

void VkShaderFactory::GetSpvNameList(const std::vector<std::string> & fileNameList)
{
    for (const auto & entry : fileNameList)
    {
        if( entry.find(".spv") != std::string::npos)
            spvNames.push_back(entry);
    }
}

std::string VkShaderFactory::GetSpvName(const std::string & shaderName)
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

void VkShaderFactory::Init()
{
    std::vector<std::string> fileList;
    GetFilesInFolder(VULKAN_ASSETS_PATH + std::string{ "/Spvs" }, fileList);
    
    for each (std::string var in fileList)
    {
        // if its not spv return
        if (var.find(".spv") == std::string::npos)
            continue;

        // load the text
        size_t fileLength = 0;
        const char* shaderText = ReadSpvFile((uint32_t&)fileLength, var.c_str());
        
        ShaderModuleWrapper * wrapper = new ShaderModuleWrapper;

        // if path contains frag then it fragment shader
        if (var.find("frag") != std::string::npos)
        {
            wrapper->shaderType = new ShaderType{ ShaderType::FRAGMENT };
            wrapper->stageFlag = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
        }
        else if (var.find("vert") != std::string::npos)
        {
            wrapper->shaderType = new ShaderType{ ShaderType::VERTEX };
            wrapper->stageFlag = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
        }
        
        wrapper->shaderName = var;
        wrapper->shaderId = AssignId();
        wrapper->module = CreateShaderModule((uint32_t*)shaderText, (size_t)fileLength );

        // add the module to list
        shaderModuleList.push_back(wrapper);
    }
}

void VkShaderFactory::DeInit()
{
    for each (ShaderModuleWrapper* var in shaderModuleList)
    {
        vkDestroyShaderModule(*CoreObjects::logicalDeviceObj, *var->module, CoreObjects::pAllocator);
        delete var->module;
        delete var->shaderType;
        delete var;
    }

    shaderModuleList.clear();
}

void VkShaderFactory::Update()
{

}

VkShaderFactory * VkShaderFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new VkShaderFactory();
    }
    return instance;
}

VkShaderFactory::~VkShaderFactory()
{
}

void VkShaderFactory::GetShaderIds( char ** shaderName, ShaderType * type, uint32_t * id, const uint32_t & shaderCount)
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

VkShaderModule * VkShaderFactory::GetShaderModule(const uint32_t & id)
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

VkShaderStageFlagBits VkShaderFactory::GetShaderStageFlag(const uint32_t & id)
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

#include "VkShaderResourceManager.h"
#include "FileReader.h"
#include <stdio.h>
#include "VulkanUtility.h"
#include "VulkanManager.h"
#include <Assertion.h>
#include <HashManager.h>
#include <VkRenderingUnwrapper.h>
#include "VkShaderResourceAllocator.h"
#include "VkDescriptorPoolFactory.h"
#include "VkBufferFactory.h"
#include "VkAttachmentFactory.h"
#include "VkSamplerFactory.h"

using namespace rapidjson;

VkShaderResourceManager* VkShaderResourceManager::instance = nullptr;

uint32_t GetGPUAlignedSize(uint32_t unalignedSize)
{
    size_t uboAlignment = VulkanManager::GetInstance()->GetPhysicalDeviceProps().limits.minUniformBufferOffsetAlignment;//vkh::GContext.gpu.deviceProps.limits.minUniformBufferOffsetAlignment;
    return static_cast<uint32_t>((unalignedSize / uboAlignment) * uboAlignment + ((unalignedSize % uboAlignment) > 0 ? uboAlignment : 0));
}

DescriptorType StringToDescType(const char* str, const char * bindingName)
{
    if (!strcmp(str, "UNIFORM")) return DescriptorType::UNIFORM_BUFFER;
    if (!strcmp(str, "SAMPLER"))
    {
        const char * pch = strstr(bindingName, "combined_");
        if (pch != NULL)
            return DescriptorType::COMBINED_IMAGE_SAMPLER;
        else
            return DescriptorType::SAMPLER;
    }

    //if (!strcmp(str, "SAMPLED_IMAGE")) return DescriptorType::SAMPLED_IMAGE;

    ASSERT_MSG_DEBUG(0 , "trying to convert an invalid string to input type");
    return DescriptorType::NUM_TYPES;
}

uint32_t VkShaderResourceManager::GetBindingID()
{
    return bindingIdCounter++;
}

uint32_t VkShaderResourceManager::GetSetLayoutID()
{
    return setWrapperIdCounter++;
}

uint32_t VkShaderResourceManager::GetPipelineLayoutID()
{
    return pipelineLayoutIdCounter++;
}

uint32_t VkShaderResourceManager::GetVkDescriptorSetLayoutWrapperID()
{
    return setLayoutWrapperIdCounter++;
}

uint32_t VkShaderResourceManager::GetVkDescriptorSetID()
{
    return vkdescriptorSetIdCounter++;
}

void VkShaderResourceManager::CreateUniqueSetLayoutWrapper(std::vector<BindingWrapper> & bindingList, std::string shaderName, uint32_t set)
{
    std::string glslShader = GetShaderNameFromRefl(shaderName);

    SetWrapper * setWrapper = new SetWrapper;
    setWrapper->bindingWrapperList = bindingList;
    setWrapper->setValue = set;
    setWrapper->id = GetSetLayoutID();

    // use the hash of the set wrapper to check its existence
    int result = HashManager::GetInstance()->FindDescriptorSetHash(setWrapper, setWrapper->id);
    // if exists add the shader name to that object's list
    if (result != -1)
    {
        std::vector<SetWrapper*>::iterator it;
        it = std::find_if(setWrapperList.begin(), setWrapperList.end(), [&](SetWrapper* e) { return e->id == result; });

        if (it == setWrapperList.end())
        {
            ASSERT_MSG_DEBUG(0, "Id mismatch");
        }

        (*it)->shaderNames.push_back(glslShader);
        (*it)->shaderFlags.push_back(GetTypeFromName(glslShader));

        DecrementSetLayoutId();

        AccumulateSetLayoutPerShader(glslShader, (*it));

        delete setWrapper;
    }
    else
    {
        // if not add this new object to the wrapper list
        setWrapper->shaderNames.push_back(glslShader);
        setWrapper->shaderFlags.push_back(GetTypeFromName(glslShader));
        setWrapperList.push_back(setWrapper);
        
        AccumulateSetLayoutPerShader(glslShader, setWrapper);

        // create vulkan descriptor set layout per set wrapper
        {
            VkDescriptorSetLayout * setLayout = CreateSetLayouts(setWrapper);
            uint32_t id = GetVkDescriptorSetLayoutWrapperID();
            idToSetLayoutMap.insert(std::pair<uint32_t, VkDescriptorSetLayout *>(id, setLayout));
            setWrapper->descriptorSetLayoutId = id;
        }

        // Create per scene descriptor sets
        {
            /*uint32_t id = GetVkDescriptorSetID();
            VkDescriptorSet * set = resourceAllocator->AllocateDescriptor(setWrapper);
            setWrapper->descriptorSetId = id;

            idToSetMap.insert(std::pair<uint32_t, VkDescriptorSet *>(id, set));
            */
        }
    }
}

void VkShaderResourceManager::CreateUniqueSetLayoutWrapper(const std::vector<BindingWrapper>& bindingList, const std::vector<std::string>& shaderName, uint32_t set)
{
    SetWrapper* setWrapper = new SetWrapper;
    setWrapper->bindingWrapperList = bindingList;
    setWrapper->setValue = set;
    setWrapper->id = GetSetLayoutID();

    // use the hash of the set wrapper to check its existence
    int result = HashManager::GetInstance()->FindDescriptorSetHash(setWrapper, setWrapper->id);
    // if exists add the shader name to that object's list
    if (result != -1)
    {
        std::vector<SetWrapper*>::iterator it;
        it = std::find_if(setWrapperList.begin(), setWrapperList.end(), [&](SetWrapper* e) { return e->id == result; });

        if (it == setWrapperList.end())
        {
            ASSERT_MSG_DEBUG(0, "Id mismatch");
        }

        (*it)->shaderNames.push_back(glslShader);
        (*it)->shaderFlags.push_back(GetTypeFromName(glslShader));

        DecrementSetLayoutId();

        AccumulateSetLayoutPerShader(glslShader, (*it));

        delete setWrapper;
    }
    else
    {
        // if not add this new object to the wrapper list
        setWrapper->shaderNames.push_back(glslShader);
        setWrapper->shaderFlags.push_back(GetTypeFromName(glslShader));
        setWrapperList.push_back(setWrapper);

        AccumulateSetLayoutPerShader(glslShader, setWrapper);

        // create vulkan descriptor set layout per set wrapper
        {
            VkDescriptorSetLayout* setLayout = CreateSetLayouts(setWrapper);
            uint32_t id = GetVkDescriptorSetLayoutWrapperID();
            idToSetLayoutMap.insert(std::pair<uint32_t, VkDescriptorSetLayout*>(id, setLayout));
            setWrapper->descriptorSetLayoutId = id;
        }

        // Create per scene descriptor sets
        {
            /*uint32_t id = GetVkDescriptorSetID();
            VkDescriptorSet * set = resourceAllocator->AllocateDescriptor(setWrapper);
            setWrapper->descriptorSetId = id;

            idToSetMap.insert(std::pair<uint32_t, VkDescriptorSet *>(id, set));
            */
        }
    }

}

void VkShaderResourceManager::AccumulateSetLayoutPerShader(std::string glslName, SetWrapper * setWrapper)
{
    //std::string glslShader = GetShaderNameFromRefl(reflName);

    std::vector<ShaderResources>::iterator it;
    it = std::find_if(perShaderResourceList.begin(), perShaderResourceList.end(), [&](ShaderResources e) { return e.shaderName == glslName; });

    if (it == perShaderResourceList.end())
    {
        ShaderResources obj = {};
        obj.shaderName = glslName;
        obj.setWrappers.push_back(setWrapper);
        perShaderResourceList.push_back(obj);
    }
    else
    {
        it->setWrappers.push_back(setWrapper);
    }
}


VkDescriptorSetLayout * VkShaderResourceManager::CreateSetLayouts(SetWrapper * setWrapper)
{
    std::vector<VkDescriptorSetLayoutBinding> bindingList;
    uint32_t numBindings = (uint32_t)setWrapper->bindingWrapperList.size();
    bindingList.resize(numBindings);

    for (uint32_t i = 0; i < numBindings; i++)
    {
       DescriptorSetLayoutBinding obj = setWrapper->bindingWrapperList[i].bindingObj;
       bindingList[i].binding = obj.binding;
       bindingList[i].descriptorCount = obj.descriptorCount;
       bindingList[i].descriptorType = UnwrapDescriptorType(obj.descriptorType);
       bindingList[i].stageFlags = VulkanUnwrap::UnwrapShaderStage(obj.stageFlags.data(), (uint32_t)obj.stageFlags.size());
       bindingList[i].pImmutableSamplers = nullptr;
    }

    VkDescriptorSetLayout * layout = new VkDescriptorSetLayout;
    VkDescriptorSetLayoutCreateInfo info = {};
    info.bindingCount = numBindings;
    info.pBindings = bindingList.data();
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.pNext = nullptr;

    ErrorCheck(vkCreateDescriptorSetLayout(*CoreObjects::logicalDeviceObj, &info, CoreObjects::pAllocator, layout));

    return layout;
}

VkDescriptorType VkShaderResourceManager::UnwrapDescriptorType(const DescriptorType & descType)
{
    VkDescriptorType vkType;

    switch (descType)
    {
        case DescriptorType::SAMPLER:
            vkType = VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLER;
            break;
        case DescriptorType::COMBINED_IMAGE_SAMPLER:
            vkType = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            break;
        case DescriptorType::SAMPLED_IMAGE:
            vkType = VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            break;
        case DescriptorType::STORAGE_IMAGE:
            vkType = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            break;
        case DescriptorType::UNIFORM_TEXEL_BUFFER:
            vkType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
            break;
        case DescriptorType::STORAGE_TEXEL_BUFFER:
            vkType = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
            break;
        case DescriptorType::UNIFORM_BUFFER:
            vkType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            break;
        case DescriptorType::STORAGE_BUFFER:
            vkType = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            break;
        case DescriptorType::UNIFORM_BUFFER_DYNAMIC:
            vkType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            break;
        case DescriptorType::STORAGE_BUFFER_DYNAMIC:
            vkType = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
            break;
        case DescriptorType::INPUT_ATTACHMENT:
            vkType = VkDescriptorType::VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
            break;
    }

    return vkType;
}

ShaderType VkShaderResourceManager::GetTypeFromName(const std::string & shaderName)
{
    ShaderType type;
    if (shaderName.find("frag") != std::string::npos)
        type = ShaderType::FRAGMENT;
    else if (shaderName.find("vert") != std::string::npos)
        type = ShaderType::VERTEX;

    return type;
}

std::string VkShaderResourceManager::GetShaderNameFromRefl(const std::string & reflName)
{
    std::string shaderName = reflName.substr(0, reflName.find_last_of("."));

    shaderName = shaderName.substr(reflName.find_last_of("\\")+1);
    return shaderName;
}

VkDescriptorSet VkShaderResourceManager::GetDescriptorSet(const uint32_t & id)
{
    return idToSetMap[id];
}


void VkShaderResourceManager::Init()
{
    //resourceAllocator = new VkShaderResourceAllocator();
    Init(EFFECTS_PATH + std::string{"\\lowEndForwardPipeline.json"});

    std::vector<std::string> fileList;
    GetFilesInFolder(VULKAN_ASSETS_PATH + std::string{ "/Reflections" }, fileList);

    uint32_t numUniform = 0;

    for each (std::string var in fileList)
    {
        // load the json file
        size_t fileLength = 0;
        const char* reflString = ReadTxtFile((uint32_t&)fileLength, var.c_str());

        Document reflDoc;
        reflDoc.Parse(reflString, fileLength);
        if (reflDoc.HasParseError())
            ASSERT_MSG_DEBUG(0, "Error parsing material file");
        free((void*)reflString);

        //TODO : Include push constants 
        if (reflDoc.HasMember("push_constants"))
        {
            const Value& pushConstants = reflDoc["push_constants"];

            const Value& elements = pushConstants["elements"];
            if (!elements.IsArray())
                printf("push constant block in reflection file does not contain a member array\n");

            for (SizeType elem = 0; elem < elements.Size(); elem++)
            {
                const Value& element = elements[elem];
                if (element["name"].GetStringLength() > 31)
                    ASSERT_MSG_DEBUG(0, "opaque block member names must be less than 32 characters");

                UniformStructMember mem;
                snprintf(mem.name, sizeof(mem.name), "%s", element["name"].GetString());
                mem.offset = element["offset"].GetInt();
                mem.size = element["size"].GetInt();
            }
        }

        bool hasDescriptors = reflDoc.HasMember("descriptor_sets");
        if (hasDescriptors)
        {
            const Value& reflFileDescSets = reflDoc["descriptor_sets"];
            numUniform = reflFileDescSets.Size();
        }
        else
            numUniform = 0;

        if (numUniform > 0)
        {
            const Value& reflFileDescSets = reflDoc["descriptor_sets"];

            std::vector<BindingWrapper> bindingList;
            int currentSet = -1;

            std::map<uint32_t, bool> setCreatedMap;

            for (SizeType dsIdx = 0; dsIdx < reflFileDescSets.Size(); dsIdx++)
            {
                const Value& currentInputFromReflData = reflFileDescSets[dsIdx];

                // this logic works only if the sets are kept in increasing order and 
                // they are not repeating in a given shader, they can have multiple bindings.
                int set = currentInputFromReflData["set"].GetInt();

                if (currentSet == -1)
                    currentSet = set;

                if (currentSet != set )
                {
                    setCreatedMap.insert(std::pair<uint32_t, bool>({ currentSet, true }));
                    // create the set using previously accumulated bindings
                    CreateUniqueSetLayoutWrapper(bindingList, var, currentSet);

                    // clear the binding list
                    bindingList.clear();

                    // change the current set
                    currentSet = set;
                }

                // add binding to the binding list

                BindingWrapper wrapper = {};
                //wrapper.id = GetBindingID();
                wrapper.bindingObj.binding = currentInputFromReflData["binding"].GetInt();
                
                int arraySize = currentInputFromReflData["ArraySize"].GetInt();
                if(arraySize == 0)
                    wrapper.bindingObj.descriptorCount = 1;
                else
                    wrapper.bindingObj.descriptorCount = arraySize;

                wrapper.bindingName = currentInputFromReflData["name"].GetString();
                wrapper.bindingObj.descriptorType = StringToDescType(currentInputFromReflData["type"].GetString(), wrapper.bindingName.c_str());
                //add all members to block definition
                const Value& reflBlockMembers = currentInputFromReflData["members"];
                for (SizeType m = 0; m < reflBlockMembers.Size(); m++)
                {
                    const Value& reflBlockMember = reflBlockMembers[m];

                    UniformStructMember mem = {};
                    mem.offset = reflBlockMember["offset"].GetInt();
                    mem.size = reflBlockMember["size"].GetInt();

                    snprintf(mem.name, sizeof(mem.name), "%s", reflBlockMember["name"].GetString());

                    wrapper.memberList.push_back(mem);
                }

                ShaderType shaderType;
                if (var.find("frag") != std::string::npos)
                    shaderType = ShaderType::FRAGMENT;
                else if (var.find("vert") != std::string::npos)
                    shaderType = ShaderType::VERTEX;
                wrapper.bindingObj.stageFlags.push_back(shaderType);

                bindingList.push_back(wrapper);
                
                if (dsIdx == reflFileDescSets.Size() - 1 && setCreatedMap.find(currentSet) == setCreatedMap.end())
                {
                    // create the set using previously accumulated bindings
                    CreateUniqueSetLayoutWrapper(bindingList, var, currentSet);
                }

                continue;

                // DONT DELETE
                /*
                ShaderType shaderType;
                if (var.find("frag") != std::string::npos)
                    shaderType = ShaderType::FRAGMENT;
                else if(var.find("vert") != std::string::npos)
                    shaderType = ShaderType::VERTEX;

                
                res.uniformDef[dsIdx].uniformSize = GetGPUAlignedSize(currentInputFromReflData["size"].GetInt());
                res.uniformDef[dsIdx].set = currentInputFromReflData["set"].GetInt();
                res.uniformDef[dsIdx].binding = currentInputFromReflData["binding"].GetInt();
                res.uniformDef[dsIdx].type = StringToDescType(currentInputFromReflData["type"].GetString());
                res.uniformDef[dsIdx].uniformName = currentInputFromReflData["name"].GetString();
                res.uniformDef[dsIdx].shaderType = shaderType;
                res.uniformDef[dsIdx].shaderName = var;

                BindingWrapper wrapper = {};
                wrapper.id = GetBindingID();
                wrapper.bindingObj.binding = currentInputFromReflData["binding"].GetInt();
                wrapper.bindingObj.descriptorCount = currentInputFromReflData["ArraySize"].GetInt();
                wrapper.bindingObj.descriptorType = StringToDescType(currentInputFromReflData["type"].GetString());
                */
            }
        }
        // shader doesn't has any descriptor sets
        else
        {
            ShaderResources obj = {};
            obj.shaderName = GetShaderNameFromRefl(var);
            perShaderResourceList.push_back(obj);
        }
    }

    // Create filler set
    VkDescriptorSetLayoutCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.bindingCount = 0;
    info.pBindings = nullptr;

    ErrorCheck(vkCreateDescriptorSetLayout(
        *CoreObjects::logicalDeviceObj,
        &info,
        CoreObjects::pAllocator,
        &fillerSetLayout
    ));

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.descriptorPool = *VkDescriptorPoolFactory::GetInstance()->GetDescriptorPool();
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &fillerSetLayout;

    ErrorCheck(vkAllocateDescriptorSets(*CoreObjects::logicalDeviceObj, &allocInfo, &fillerSet));

}

void VkShaderResourceManager::Init(const std::string& pipelineFilePath)
{
    //Iterate through pipeline json for all the effects required for the application

    //Load the json file
    auto LoadJson = [&](const char* filename) -> Document
    {
        size_t fileLength = 0;
        const char* fileText = ReadTxtFile((uint32_t&)fileLength, filename);

        Document jsonDoc;
        jsonDoc.Parse(fileText, fileLength);
        if (jsonDoc.HasParseError())
            ASSERT_MSG_DEBUG(0, "Error parsing material file");
        free((void*)fileText);
        
        return jsonDoc;
    };

    Document pipelineDoc = LoadJson(pipelineFilePath.c_str());
    if (pipelineDoc.HasMember("Effects"))
    {
        //Per effect
        const Value& pipelineEffects = pipelineDoc["Effects"];
        for (SizeType m = 0; m < pipelineEffects.Size(); m++)
        {
            const Value& effect = pipelineEffects[m];
            std::string effectPath = EFFECTS_PATH + std::string{"\\"} + std::string{effect.GetString()};
            printf("%s \n", effectPath.c_str());

            Document effectDoc = LoadJson(effectPath.c_str());
            if (effectDoc.HasMember("Shaders"))
            {
                std::map<uint32_t, std::vector<BindingWrapper>> setMap;
                std::vector<std::string> shaderNameList;

                uint32_t numUniform = 0;

                const Value& effectShaders = effectDoc["Shaders"];
                for (SizeType i = 0; i < effectShaders.Size(); i++)
                {
                    const Value& effect = effectShaders[i];
                    std::string shaderName = effect["name"].GetString();
                    std::string shaderTypeVal = effect["type"].GetString();
                    shaderNameList.push_back(shaderName);

                    std::string reflPath;
                    
                    if (effect.HasMember("reflPath"))
                    {
                        std::string path = effect["reflPath"].GetString();
                        if (!path.empty())
                        {
                            reflPath = path;
                        }
                    }
                    else
                    {
                        reflPath = VULKAN_ASSETS_PATH + std::string{"/Reflections/"} + shaderName + std::string{".refl"};
                    }

                    printf("%s %s %s \n", shaderName.c_str(), shaderType.c_str(), reflPath.c_str());

                    Document reflDoc = LoadJson(reflPath.c_str());
                    bool hasDescriptors = reflDoc.HasMember("descriptor_sets");
                    if (hasDescriptors)
                    {
                        const Value& reflFileDescSets = reflDoc["descriptor_sets"];
                        numUniform = reflFileDescSets.Size();
                    }
                    else
                        numUniform = 0;

                    if (numUniform > 0)
                    {
                        const Value& reflFileDescSets = reflDoc["descriptor_sets"];

                        for (SizeType dsIdx = 0; dsIdx < reflFileDescSets.Size(); dsIdx++)
                        {
                            const Value& currentInputFromReflData = reflFileDescSets[dsIdx];

                            // this logic works only if the sets are kept in increasing order and 
                            // they are not repeating in a given shader, they can have multiple bindings.
                            int set = currentInputFromReflData["set"].GetInt();

                            // add binding to the binding list
                            BindingWrapper wrapper = {};
                            wrapper.bindingObj.binding = currentInputFromReflData["binding"].GetInt();

                            int arraySize = currentInputFromReflData["ArraySize"].GetInt();
                            if (arraySize == 0)
                                wrapper.bindingObj.descriptorCount = 1;
                            else
                                wrapper.bindingObj.descriptorCount = arraySize;

                            wrapper.bindingName = currentInputFromReflData["name"].GetString();
                            wrapper.bindingObj.descriptorType = StringToDescType(currentInputFromReflData["type"].GetString(), wrapper.bindingName.c_str());
                            
                            //add all members to block definition
                            const Value& reflBlockMembers = currentInputFromReflData["members"];
                            for (SizeType m = 0; m < reflBlockMembers.Size(); m++)
                            {
                                const Value& reflBlockMember = reflBlockMembers[m];

                                UniformStructMember mem = {};
                                mem.offset = reflBlockMember["offset"].GetInt();
                                mem.size = reflBlockMember["size"].GetInt();

                                snprintf(mem.name, sizeof(mem.name), "%s", reflBlockMember["name"].GetString());

                                wrapper.memberList.push_back(mem);
                            }

                            ShaderType shaderType;
                            if (shaderTypeVal == "FRAGMENT")
                                shaderType = ShaderType::FRAGMENT;
                            else if (shaderTypeVal == "VERTEX")
                                shaderType = ShaderType::VERTEX;
                            wrapper.bindingObj.stageFlags.push_back(shaderType);

                            if (setMap.find(set) == setMap.end()) 
                            {
                                setMap.insert(std::pair<uint32_t, std::vector<BindingWrapper>>(set, std::vector{wrapper}));
                            }
                            else 
                            {
                                setMap[set].push_back(wrapper);
                            }

                            
                            continue;

                            // DONT DELETE
                            /*
                            ShaderType shaderType;
                            if (var.find("frag") != std::string::npos)
                                shaderType = ShaderType::FRAGMENT;
                            else if(var.find("vert") != std::string::npos)
                                shaderType = ShaderType::VERTEX;


                            res.uniformDef[dsIdx].uniformSize = GetGPUAlignedSize(currentInputFromReflData["size"].GetInt());
                            res.uniformDef[dsIdx].set = currentInputFromReflData["set"].GetInt();
                            res.uniformDef[dsIdx].binding = currentInputFromReflData["binding"].GetInt();
                            res.uniformDef[dsIdx].type = StringToDescType(currentInputFromReflData["type"].GetString());
                            res.uniformDef[dsIdx].uniformName = currentInputFromReflData["name"].GetString();
                            res.uniformDef[dsIdx].shaderType = shaderType;
                            res.uniformDef[dsIdx].shaderName = var;

                            BindingWrapper wrapper = {};
                            wrapper.id = GetBindingID();
                            wrapper.bindingObj.binding = currentInputFromReflData["binding"].GetInt();
                            wrapper.bindingObj.descriptorCount = currentInputFromReflData["ArraySize"].GetInt();
                            wrapper.bindingObj.descriptorType = StringToDescType(currentInputFromReflData["type"].GetString());
                            */
                        }
                    }
                }

                for (auto const& [key, val] : setMap)
                {
                    CreateUniqueSetLayoutWrapper(val, shaderNameList, key);
                }

            }
            else
            {
                ASSERT_MSG_DEBUG(0, "Shaders not found in effect file");
            }
        }
    }
    else
    {
        ASSERT_MSG_DEBUG(0, "Effects not found in effect file");
    }

    //The effect file will contain the shader names for the effect

    //Check for the corresponding refl file to create the descriptor set, layout and pipeline and its layout

}

void VkShaderResourceManager::DeInit()
{
    //delete resourceAllocator;

    for (uint32_t i = 0; i < setWrapperList.size(); i++)
    {
        delete setWrapperList[i];
    }

    setWrapperList.clear();
    perShaderResourceList.clear();
    pipelineLayoutIdToSetValuesMap.clear();

    for (uint32_t i = 0; i < (uint32_t)pipelineLayoutWrapperList.size(); i++)
    {
        
        vkDestroyPipelineLayout(*CoreObjects::logicalDeviceObj, *pipelineLayoutWrapperList[i].pipelineLayout, CoreObjects::pAllocator);
        delete pipelineLayoutWrapperList[i].pipelineLayout;
    }

    for (auto const& obj : idToSetLayoutMap)
    {
        vkDestroyDescriptorSetLayout(*CoreObjects::logicalDeviceObj, *obj.second, CoreObjects::pAllocator);
        delete obj.second;
    }

    idToSetLayoutMap.clear();

    for (auto const& obj : fillerSetLayouts)
    {
        vkDestroyDescriptorSetLayout(*CoreObjects::logicalDeviceObj, obj, CoreObjects::pAllocator);
    }

    fillerSetLayouts.clear();
    vkDestroyDescriptorSetLayout(*CoreObjects::logicalDeviceObj, fillerSetLayout, CoreObjects::pAllocator);

}

void VkShaderResourceManager::Update()
{

}

VkShaderResourceManager * VkShaderResourceManager::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new VkShaderResourceManager();
    }
    return instance;
}

VkShaderResourceManager::~VkShaderResourceManager()
{
}

std::vector<SetWrapper*> VkShaderResourceManager::GetSetsForShaders(const std::vector<std::string>& shaderNames)
{
    std::vector<SetWrapper*> setList;

    for each (std::string name in shaderNames)
    {
        std::vector<ShaderResources>::iterator it;
        it = std::find_if(perShaderResourceList.begin(), perShaderResourceList.end(), [&](ShaderResources e) { return e.shaderName == name; });

        if (it == perShaderResourceList.end())
        {
            ASSERT_MSG_DEBUG(0, "shader name mismatch");
        }
        else if (it->setWrappers.size() == 0)
            continue;
        else
        {
            for(uint32_t i = 0; i < it->setWrappers.size(); i++)
            {
                SetWrapper* obj = it->setWrappers[i];
                std::vector<SetWrapper*>::iterator itt;
                itt = std::find_if(setList.begin(), setList.end(), [&](SetWrapper* e) { return e->id == obj->id; });

                if(itt == setList.end())
                    setList.push_back(it->setWrappers[i]);
            }
        }
    }

    return setList;
}

uint32_t VkShaderResourceManager::CreatePipelineLayout(SetWrapper ** setWrapperList, const size_t & numSets)
{
    PipelineLayoutWrapper pipelineLayoutWrapperObj = {};
    pipelineLayoutWrapperObj.id = GetPipelineLayoutID();
    //pipelineLayoutWrapperObj.setLayoutList.resize(numSets);

    GetSetLayouts(setWrapperList, (uint32_t)numSets, pipelineLayoutWrapperObj.setLayoutList, pipelineLayoutWrapperObj.setValuesInPipelineLayout);
    
    VkPipelineLayoutCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.flags = 0;
    pipelineCreateInfo.pPushConstantRanges= nullptr;
    pipelineCreateInfo.pSetLayouts = pipelineLayoutWrapperObj.setLayoutList.data();
    pipelineCreateInfo.pNext = nullptr;
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineCreateInfo.pushConstantRangeCount = 0;
    pipelineCreateInfo.setLayoutCount = (uint32_t)pipelineLayoutWrapperObj.setLayoutList.size();

    VkPipelineLayout * pipelineLayout = new VkPipelineLayout;
    ErrorCheck(vkCreatePipelineLayout(*CoreObjects::logicalDeviceObj, &pipelineCreateInfo, CoreObjects::pAllocator, pipelineLayout));

    pipelineLayoutWrapperObj.pipelineLayout = pipelineLayout;
    pipelineLayoutWrapperList.push_back(pipelineLayoutWrapperObj);
    
    pipelineLayoutIdToSetValuesMap.insert(std::pair<uint32_t, std::vector<int>>(
    { pipelineLayoutWrapperObj.id, pipelineLayoutWrapperObj.setValuesInPipelineLayout}));

    return pipelineLayoutWrapperObj.id;
}

VkPipelineLayout * VkShaderResourceManager::GetPipelineLayout(const uint32_t & id)
{
    std::vector<PipelineLayoutWrapper>::iterator it;

    it = std::find_if(pipelineLayoutWrapperList.begin(), pipelineLayoutWrapperList.end(), [&](PipelineLayoutWrapper e) {
        return e.id == id; });
    
    ASSERT_MSG_DEBUG(it != pipelineLayoutWrapperList.end(), "id not found");
    
    return (it)->pipelineLayout;
}

std::vector<SetWrapper*>* VkShaderResourceManager::GetSetWrapperList()
{
    return &setWrapperList;
}

void VkShaderResourceManager::GetSetLayouts(SetWrapper ** setWrapperList, const uint32_t & numSets, std::vector<VkDescriptorSetLayout> & layoutList, std::vector<int> & setValueList)
{
    // order of sets (descriptor types) should match order mentioned in shaders 

    std::vector<uint32_t> setValues;
    for (uint32_t i = 0; i < numSets; i++)
    {
        SetWrapper * wrapper = setWrapperList[i];
        setValues.push_back(wrapper->setValue);
    }

    std::sort(setValues.begin(), setValues.end());
    std::vector<uint32_t> completeSet;
    {
        for (uint32_t i = setValues.front(); i <= setValues.back(); i++)
        {
            completeSet.push_back(i);
        }

        for each(auto obj in completeSet)
        {
            std::vector<uint32_t>::iterator it;
            it = std::find_if(setValues.begin(), setValues.end(), [&](uint32_t e) {
                return e == obj; });

            // if the setvalue is present in setwrapper add the existing setlayout
            if (it != setValues.end())
            {
                for (uint32_t i = 0; i < numSets; i++)
                {
                    SetWrapper * wrapper = setWrapperList[i];
                    if (wrapper->setValue == obj)
                    {
                        layoutList.push_back(*idToSetLayoutMap[wrapper->descriptorSetLayoutId]);
                        setValueList.push_back(obj);
                        break;
                    }
                }
            }
            // if not create a dummy set layout and add it to the list
            else
            {
                //VkDescriptorSetLayoutCreateInfo info = {};
                //info.bindingCount = 0;
                //info.pBindings = nullptr;
                //info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

                //VkDescriptorSetLayout setLayout;
                //ErrorCheck(vkCreateDescriptorSetLayout(
                //    *CoreObjects::logicalDeviceObj,
                //    &info,
                //    CoreObjects::pAllocator,
                //    &setLayout
                //));
                layoutList.push_back(fillerSetLayout);
                //fillerSetLayouts.push_back(setLayout);
                setValueList.push_back(-1);
            }
        }
    }

    return;
    /*
    uint32_t setValue = 0; 

    for (uint32_t i = 0; i < (uint32_t)setValues.size(); i++)
    {
        uint32_t diff = setValues[i] - setValue;
        if (diff > 1)
        {
            // Create new diff * layouts
            for (uint32_t i = 0; i < diff - 1; i++)
            {
                VkDescriptorSetLayoutBinding obj = {};
                obj.binding = 0;
                obj.descriptorCount = 0;
                obj.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                obj.pImmutableSamplers = nullptr;
                obj.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;

                VkDescriptorSetLayoutCreateInfo info = {};
                info.bindingCount = 0;
                info.pBindings = nullptr;
                info.sType= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

                VkDescriptorSetLayout setLayout;
                ErrorCheck(vkCreateDescriptorSetLayout(
                    *CoreObjects::logicalDeviceObj,
                    &info,
                    CoreObjects::pAllocator,
                    &setLayout
                ));
                layoutList.push_back(setLayout);
                fillerSetLayouts.push_back(setLayout);
                //setValue += 1;
            }
        }
//        else
        {
            for (uint32_t i = 0; i < numSets; i++)
            {
                SetWrapper * wrapper = setWrapperList[i];

                if (idToSetLayoutMap.find(wrapper->descriptorSetLayoutId) == idToSetLayoutMap.end())
                {
                    ASSERT_MSG(0, "Id mismatch");
                }
                else if(setValues[i] == wrapper->setValue)
                {
                    layoutList.push_back(*idToSetLayoutMap[wrapper->descriptorSetLayoutId]);
                    break;
                }
            }
            setValue = setValues[i];
        }
    }
    */
}

std::vector<VkDescriptorSet> VkShaderResourceManager::GetDescriptors(uint32_t * ids, const uint32_t & count, const uint32_t & pipelineLayoutId)
{
    std::vector<int> * setValues = &pipelineLayoutIdToSetValuesMap[pipelineLayoutId];
    std::vector<VkDescriptorSet> list;

    for (uint32_t i = 0, k = 0; i < setValues->size(); i++)
    {
        if (setValues->at(i) != -1)
        {
            VkDescriptorSet set = idToSetMap[ids[k]];
            ASSERT_MSG_DEBUG(set != VK_NULL_HANDLE, "Set not found");
            list.push_back(set);
            k++;
        }
        else
        {
            list.push_back(fillerSet);
        }
    }
    return list;
}

std::tuple<std::vector<VkDescriptorSet>, uint32_t> VkShaderResourceManager::GetDescriptors(uint32_t * ids, const uint32_t & count, const uint32_t & pipelineLayoutId, const uint32_t & firstSet)
{
    std::vector<int> * setValues = &pipelineLayoutIdToSetValuesMap[pipelineLayoutId];
    std::vector<VkDescriptorSet> list;
    uint32_t offset;
    for (uint32_t i = 0, k = 0; i < setValues->size(); i++)
    {
        if (setValues->at(i) != -1)
        {
            if (firstSet == k)
                offset = i;
            VkDescriptorSet set = idToSetMap[ids[k]];
            ASSERT_MSG_DEBUG(set != VK_NULL_HANDLE, "Set not found");
            list.push_back(set);
            k++;
        }
        else
        {
            list.push_back(fillerSet);
        }
    }

    return std::make_tuple(list, offset);
}

uint32_t * VkShaderResourceManager::AllocateDescriptorSets(SetWrapper * setwrapper, const uint32_t & numDescriptors)
{
    uint32_t * ids = new uint32_t[numDescriptors];
    VkDescriptorPool * pool = VkDescriptorPoolFactory::GetInstance()->GetDescriptorPool();

    auto it = idToSetLayoutMap.find(setwrapper->descriptorSetLayoutId);

    if (it == idToSetLayoutMap.end())
    {
        ASSERT_MSG(0, "set layout not found");
    }

    VkDescriptorSetLayout * layout = (it)->second;// idToSetLayoutMap[setwrapper->descriptorSetLayoutId];

    std::vector<VkDescriptorSetLayout> setLayouts;
    std::vector<VkDescriptorSet> sets;
    sets.resize(numDescriptors);

    for (uint32_t i = 0; i < numDescriptors; i++)
    {
        ids[i] = GetVkDescriptorSetID();
        setLayouts.push_back(*layout);
    }

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.descriptorPool = *pool;
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorSetCount = numDescriptors;
    allocInfo.pSetLayouts = setLayouts.data();

    ErrorCheck( vkAllocateDescriptorSets(*CoreObjects::logicalDeviceObj, &allocInfo, sets.data()));
    
    for (uint32_t i = 0; i < numDescriptors; i++)
        idToSetMap.insert(std::pair<uint32_t, VkDescriptorSet>(ids[i], sets[i]));

    return ids;
}
/*
// meant for single binding descriptor sets, should get deprecated
void VkShaderResourceManager::LinkSetBindingToResources(ShaderBindingDescription * desc)
{
    VkDescriptorType descriptorType = UnwrapDescriptorType(desc->resourceType);

    uint32_t numWrites = (uint32_t)desc->descriptorSetIds.size();
    uint32_t numDescriptors = numWrites;

    std::vector<VkDescriptorBufferInfo> bufferInfoList;
    std::vector<VkDescriptorImageInfo> imageInfoList;

    switch (descriptorType)
    {
    case VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
    {
        uint32_t numBuffers = desc->bufferBindingInfo.bufferIdList.size();
        uint32_t bufferId;

        if (numBuffers > 1)
        {
            ASSERT_MSG(numWrites != numBuffers, "Num writes != num buffers");
        }

        for (uint32_t i = 0; i < numWrites; i++)
        {
            VkBuffer * buf;

            if (numBuffers == 1)
                buf = VkBufferFactory::GetInstance()->GetBuffer(desc->bufferBindingInfo.bufferIdList[0]);
            else
                buf = VkBufferFactory::GetInstance()->GetBuffer(desc->bufferBindingInfo.bufferIdList[i]);

            VkDescriptorBufferInfo info = {};
            info.buffer = *buf;
            info.offset = desc->bufferBindingInfo.info.offsetsForEachDescriptor[i];
            info.range = desc->bufferBindingInfo.info.dataSizePerDescriptorAligned;
            bufferInfoList.push_back(info);
        }
    }
    break;

    case VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
        for (uint32_t i = 0; i < numWrites; i++)
        {
            //VkDescriptorImageInfo info = {};
            //info.imageLayout = ;
            //info.imageView = ;
            //info.sampler = ;
            //imageInfoList.push_back(info);
        }
        break;

    case VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLER:
        ASSERT_MSG_DEBUG(0, "Yet to be implemented");
        break;

    default:
        ASSERT_MSG_DEBUG(0, "Yet to be implemented");
    }

    std::vector<VkWriteDescriptorSet> writeList;

    for (uint32_t i = 0; i < numWrites; i++)
    {
        VkWriteDescriptorSet write = {};
        write.descriptorCount = 1; //since linking only one binding of a set
        write.descriptorType = descriptorType;
        write.dstArrayElement = 0;
        write.dstBinding = desc->binding;
        write.pBufferInfo = &bufferInfoList[i];
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = GetDescriptorSet(desc->descriptorSetIds[i]);
        writeList.push_back(write);
    }

    vkUpdateDescriptorSets(*CoreObjects::logicalDeviceObj, (uint32_t)writeList.size(), writeList.data(), 0, nullptr);
}
*/

void VkShaderResourceManager::LinkSetBindingToResources(ShaderBindingDescription * desc, const uint32_t & numBindings)
{
    // number of writes/set will equal to number of bindings in a set
    uint32_t numWritesPerDescriptorSet = numBindings;
    uint32_t numDescriptorSets = (uint32_t)desc->descriptorSetIds.size();

    for (uint32_t i = 0; i < numDescriptorSets; i++)
    {
        // Create writes for various type of descriptors
        std::vector<VkWriteDescriptorSet> writeList;
        writeList.resize(numWritesPerDescriptorSet);

        VkDescriptorBufferInfo bufferInfo = {};
        VkDescriptorImageInfo imageInfo = {};

        for (uint32_t k = 0; k < numWritesPerDescriptorSet; k++)
        {
            VkDescriptorType descriptorType = UnwrapDescriptorType(desc[k].resourceType);
            
            writeList[k].descriptorCount = 1;
            writeList[k].descriptorType = descriptorType;
            writeList[k].dstBinding = desc[k].binding;
            writeList[k].dstSet = GetDescriptorSet(desc->descriptorSetIds[i]);
            writeList[k].pBufferInfo = nullptr;
            writeList[k].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeList[k].pImageInfo = nullptr;
            writeList[k].pTexelBufferView = nullptr;

            switch (descriptorType)
            {
            case VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            {
                uint32_t numBuffers = (uint32_t)desc[k].bufferBindingInfo.bufferIdList.size();

                // if numBuffer == numDescriptors : no sharing
                // if numBuffer < numDescriptors : sharing

                VkBuffer * buf;

                if (numBuffers == 1)
                    buf = VkBufferFactory::GetInstance()->GetBuffer(desc[k].bufferBindingInfo.bufferIdList[0]);
                else
                    buf = VkBufferFactory::GetInstance()->GetBuffer(desc[k].bufferBindingInfo.bufferIdList[i]);

                bufferInfo.buffer = *buf;
                bufferInfo.offset = desc[k].bufferBindingInfo.info.offsetsForEachDescriptor[i];
                bufferInfo.range = desc[k].bufferBindingInfo.info.dataSizePerDescriptorAligned;
                writeList[k].pBufferInfo = &(bufferInfo);

            }
            break;

            case VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            {
                imageInfo.imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                
                uint32_t numViews = (uint32_t)desc[k].imageBindingInfo.imageId.size();
                VkImageView imageView;

                if (numViews == 1)
                    imageView = VkAttachmentFactory::GetInstance()->GetImageView(desc[k].imageBindingInfo.imageId[0]);
                else
                    imageView = VkAttachmentFactory::GetInstance()->GetImageView(desc[k].imageBindingInfo.imageId[i]);

                imageInfo.imageView = imageView;
                    
                uint32_t numSamplers = (uint32_t)desc[k].samplerBindingInfo.samplerId.size();
                VkSampler * sampler;
                if (numSamplers == 1)
                    sampler = VkSamplerFactory::GetInstance()->GetSampler(desc[k].samplerBindingInfo.samplerId[0]);
                else
                    sampler = VkSamplerFactory::GetInstance()->GetSampler(desc[k].samplerBindingInfo.samplerId[i]);

                imageInfo.sampler = *sampler;

                writeList[k].pImageInfo = &(imageInfo);

            }
            break;

            case VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLER:
                ASSERT_MSG_DEBUG(0, "Yet to be implemented");
                break;

            default:
                ASSERT_MSG_DEBUG(0, "Yet to be implemented");
            }

        }

        // update descriptor set
        vkUpdateDescriptorSets(*CoreObjects::logicalDeviceObj, (uint32_t)writeList.size(), writeList.data(), 0, nullptr);

    }
}


const std::vector<int>* VkShaderResourceManager::GetSetValuesInPipelineLayout(const uint32_t & pipelineLayoutId)
{
    return &pipelineLayoutIdToSetValuesMap[pipelineLayoutId];
}

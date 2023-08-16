#pragma once

#include <vulkan\vulkan.h>
#include <map>
#include "RenderingWrapper.h"

class VkSamplerFactory
{

private:
    VkSamplerFactory(){}
    VkSamplerFactory(VkSamplerFactory const &) {}
    VkSamplerFactory const & operator= (VkSamplerFactory const &) {}

    static VkSamplerFactory* instance;

    uint32_t idCounter = 0;
    uint32_t GenerateId();

    std::map<uint32_t, VkSampler> idToSamplerMap;

public:
    void Init();
    void DeInit();
    void Update();
    static VkSamplerFactory* GetInstance();
    ~VkSamplerFactory();

    uint32_t CreateSampler(const SamplerCreateInfo & info);
    uint32_t CreateSampler(const VkSamplerCreateInfo & info);
    VkSampler * GetSampler(const uint32_t & id);
};

#pragma once
#include <vector>
#include<vulkan\vulkan.h>
#include <array>

struct RenderPassAttachmentInfo;
struct SubpassInfo;
struct SubpassDependency;

class RenderpassWrapper
{
public:
    uint32_t id;
    VkRenderPass renderPass;
    VkRenderPassBeginInfo beginInfo;
    //std::vector<VkClearValue> clearValue;
    VkClearValue * clearValue;
    uint32_t clearValueCount;
};

class VkRenderPassFactory
{

private:
    VkRenderPassFactory(){}
    VkRenderPassFactory(VkRenderPassFactory const &) {}
    VkRenderPassFactory const & operator= (VkRenderPassFactory const &) {}

    static VkRenderPassFactory* instance;
    static uint32_t renderpassIdCounter;

    std::vector<VkAttachmentReference> refs;
    uint32_t refCounter;
    std::vector<RenderpassWrapper*> renderpassList;

    uint32_t GetId();

public:
    void Init();
    void DeInit();
    void Update();
    static VkRenderPassFactory* GetInstance();
    ~VkRenderPassFactory();

    void CreateRenderPass(
        const VkAttachmentDescription* renderpassAttachmentList, const uint32_t & attachmentCount,
        const VkSubpassDescription* subpassList, const uint32_t & subpassCount,
        const VkSubpassDependency* dependencyList, const uint32_t & dependencyCount,
        uint32_t& renderPassId
    );

    // deprecated.
    void CreateRenderPass(
        const VkAttachmentDescription* renderpassAttachmentList, const uint32_t & attachmentCount,
        const VkSubpassDescription* subpassList, const uint32_t & subpassCount,
        const VkSubpassDependency* dependencyList, const uint32_t & dependencyCount,
        const VkRenderPassBeginInfo beginInfo, uint32_t& renderPassId
    );
    void DestroyRenderPass(const uint32_t & id);

    void SetRenderPassBeginInfo(const VkRenderPassBeginInfo beginInfo, const uint32_t & renderpassId);
    void SetClearColor(std::vector<VkClearValue> clearValue, const uint32_t & renderPassId);

    VkRenderPass * GetRenderPass(const uint32_t & id);
    VkClearValue * GetClearValue(const uint32_t & renderpassId);
    uint32_t GetClearValueCount(const uint32_t & renderpassId);
};

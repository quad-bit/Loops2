#pragma once
#include <vulkan\vulkan.h>
#include <vector>

struct FenceWrapper
{
    uint32_t id;
    VkFence * fence;
};

struct SemaphoreWrapper
{
    uint32_t id;
    VkSemaphore * semaphore;
};

class VkSynchroniserFactory
{

private:
    VkSynchroniserFactory(){}
    VkSynchroniserFactory(VkSynchroniserFactory const &) {}
    VkSynchroniserFactory const & operator= (VkSynchroniserFactory const &) {}

    static VkSynchroniserFactory* instance;

    std::vector<FenceWrapper> fenceList;
    std::vector<SemaphoreWrapper> semaphoreList;

    uint32_t fenceIdCounter = 0, semaphoreIdCounter = 0;

    uint32_t GetFenceId() { return fenceIdCounter++; }
    uint32_t GetSemaphoreId() { return semaphoreIdCounter++; }

public:
    void Init();
    void DeInit();
    void Update();
    static VkSynchroniserFactory* GetInstance();
    ~VkSynchroniserFactory();

    uint32_t CreateFence(bool isFenceSignaled);
    void DestroyFence(uint32_t id);
    VkFence* GetFence(uint32_t id);

    uint32_t Create_Semaphore(bool isSemaphoreSignaled); // "CreateSemaphore" is already existing, not available
    void DestroySemaphore(uint32_t id);
    VkSemaphore* GetSemaphore(uint32_t id);
    VkSemaphore* GetSemaphore(const uint32_t * id, const uint32_t & count);
};

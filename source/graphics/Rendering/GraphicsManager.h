#pragma once
#include <stdint.h>
#include <string>
#include <Settings.h>

template <typename T>
class RenderingInterface;

#if (RENDERING_API == VULKAN)
class VulkanInterface;
struct QueueWrapper;
enum class QueuePurpose;
enum class PipelineType;
#elif (RENDERING_API == DX)
class DxInterface;
#endif

class GraphicsManager
{
private:
    GraphicsManager(){}
    GraphicsManager(GraphicsManager const &) {}
    GraphicsManager const & operator= (GraphicsManager const &) {}

    static GraphicsManager* instance;

#if (RENDERING_API == VULKAN)
    VulkanInterface * apiInterface;
    RenderingInterface<VulkanInterface> * renderingInterfaceObj;
#elif (RENDERING_API == DX)
    DxInterface * apiInterface;
#endif

public:
    void Init(uint32_t winWidth, uint32_t winHeight, std::string winName);
    void DeInit();
    void Update();
    static GraphicsManager* GetInstance();
    ~GraphicsManager();

    void SetupRenderer();
    void DislogeRenderer();

    void PreUpdate();
    void PostUpdate();

    bool IsWindowActive();
};

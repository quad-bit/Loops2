#pragma once
#include <stdint.h>
#include <string>
#include <Settings.h>


#if (RENDERING_API == VULKAN)

namespace Renderer
{
    class VulkanInterface;
    class RenderingManager;
}

namespace Core
{
    namespace Wrappers
    {
        struct QueueWrapper;
    }
    namespace Enums
    {
        enum class QueueType;
        enum class PipelineType;
    }
}

#elif (RENDERING_API == DX)
class DxInterface;
#endif

namespace Renderer
{
    class GraphicsManager
    {
    private:
        GraphicsManager() {}
        GraphicsManager(GraphicsManager const&) {}
        GraphicsManager const& operator= (GraphicsManager const&) {}

        static GraphicsManager* instance;

#if (RENDERING_API == VULKAN)
        VulkanInterface* apiInterface;
        RenderingManager* renderingMngrObj;
#elif (RENDERING_API == DX)
        DxInterface* apiInterface;
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
}
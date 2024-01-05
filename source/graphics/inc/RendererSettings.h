#ifndef RENDERER_RENDERERSETTINGS_H_
#define RENDERER_RENDERERSETTINGS_H_

#include "Utility/RenderingWrappers/RenderingWrapper.h"

namespace Renderer
{
    class RenderingManager;
    class RendererSettings
    {
    private:
        static uint32_t m_renderQueueId;
        static uint32_t m_presentationQueueId;
        static uint32_t m_computeQueueId;
        static uint32_t m_transferQueueId;

        /*static Core::Wrappers::QueueWrapper m_renderQueueWrapper;
        static Core::Wrappers::QueueWrapper m_presentationQueueWrapper;
        static Core::Wrappers::QueueWrapper m_computeQueueWrapper;
        static Core::Wrappers::QueueWrapper m_transferQueueWrapper;*/

        static std::vector<Core::Wrappers::QueueWrapper> m_queueReq;

        static bool m_msaaEnabled;
        static bool m_sampleRateShadingEnabled;
        static bool m_multiSamplingAvailable;
        static Core::Enums::Samples m_maxSampleCountAvailable;

        //static std::vector<uint32_t> depthPrepassImageId;
        static uint32_t m_shadowMapWidth, m_shadowMapHeight;

        static uint32_t m_swapBufferCount;
        static uint32_t m_maxFramesInFlight;// , m_currentFrameInFlight;
        static uint32_t m_maxFrameRate;
        static Core::Enums::Format m_bestDepthFormat;
        static Core::Enums::Format m_surfaceFormat;

    public:
        friend class RenderingManager;

        static uint32_t GetRenderQueueId();
        static uint32_t GetPresentationQueueId();
        static uint32_t GetComputeQueueId();
        static uint32_t GetTransferQueueId();

        static Core::Enums::Samples GetMaxSampleCountAvailable();

        static bool IsMsaaEnabled();
        static bool IsSampleRateShadingAvailable();
        static bool IsMultiSamplingAvailable();
        static const uint32_t& GetMaxFramesInFlightCount();
        //static const uint32_t& GetCurrentFrameIndex();
        static const uint32_t& GetSwapBufferCount();
        static const Core::Enums::Format& GetBestDepthFormat();
        static const Core::Enums::Format& GetSurfaceFormat();

        /*static const Core::Wrappers::QueueWrapper& GetRenderQueueInfo();
        static const Core::Wrappers::QueueWrapper& GetPresentationQueueInfo();
        static const Core::Wrappers::QueueWrapper& GetComputeQueueInfo();
        static const Core::Wrappers::QueueWrapper& GetTransferQueueInfo();*/
    };
}

#endif //RENDERER_RENDERERSETTINGS_H_
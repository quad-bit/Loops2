#pragma once

#include <stdint.h>
#include <vector>

namespace Core
{
    namespace Wrappers
    {
        struct QueueWrapper;
    }

    namespace Enums
    {
        enum class Samples;
    }

    struct RendererSettings
    {
        static uint32_t renderQueueId;
        static uint32_t presentationQueueId;
        static uint32_t computeQueueId;
        static uint32_t transferQueueId;

        static Wrappers::QueueWrapper* queueReq;
        static uint32_t queueRequirementCount;

        static bool MSAA_Enabled;
        static bool sampleRateShadingEnabled;
        static bool multiSamplingAvailable;
        static Enums::Samples* sampleCount;

        static std::vector<uint32_t> depthPrepassImageId;
        static uint32_t shadowMapWidth, shadowMapHeight;
    };
}
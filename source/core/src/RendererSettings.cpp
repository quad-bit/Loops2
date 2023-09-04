#include "RendererSettings.h"

uint32_t Core::RendererSettings::renderQueueId;
uint32_t Core::RendererSettings::presentationQueueId;
uint32_t Core::RendererSettings::computeQueueId;
uint32_t Core::RendererSettings::transferQueueId;

Core::Wrappers::QueueWrapper * Core::RendererSettings::queueReq;
uint32_t Core::RendererSettings::queueRequirementCount;
bool Core::RendererSettings::MSAA_Enabled;
bool Core::RendererSettings::sampleRateShadingEnabled;
bool Core::RendererSettings::multiSamplingAvailable;

Core::Enums::Samples * Core::RendererSettings::sampleCount;

std::vector<uint32_t> Core::RendererSettings::depthPrepassImageId;
uint32_t Core::RendererSettings::shadowMapWidth, Core::RendererSettings::shadowMapHeight;



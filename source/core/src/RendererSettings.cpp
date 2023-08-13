#include "RendererSettings.h"

uint32_t RendererSettings::renderQueueId;
uint32_t RendererSettings::presentationQueueId;
uint32_t RendererSettings::computeQueueId;
uint32_t RendererSettings::transferQueueId;

QueueWrapper * RendererSettings::queueReq;
uint32_t RendererSettings::queueRequirementCount;
bool RendererSettings::MSAA_Enabled;
bool RendererSettings::sampleRateShadingEnabled;
bool RendererSettings::multiSamplingAvailable;

Samples * RendererSettings::sampleCount;

std::vector<uint32_t> RendererSettings::depthPrepassImageId;
uint32_t RendererSettings::shadowMapWidth, RendererSettings::shadowMapHeight;



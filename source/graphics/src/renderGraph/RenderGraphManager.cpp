#include "RenderGraph/RenderGraphManager.h"
#include "Settings.h"
#include "VulkanInterface.h"
#include "renderGraph/Task.h"
#include "renderGraph/tasks/PresentationTask.h"
#include "renderGraph/tasks/RenderTask.h"
#include "resourceManagement/Resource.h"

uint32_t g_bufferDistributionCount = 0;
uint32_t g_semaphoreDistributionCount = 0;
uint32_t g_fenceDistributionCount = 0;

Renderer::RenderGraph::RenderGraphManager::RenderGraphManager(const Core::Utility::RenderData& renderData, const Core::WindowSettings& windowSettings):
    m_renderData(renderData), m_windowSettings(windowSettings)
{
}

void Renderer::RenderGraph::RenderGraphManager::AddPipeline(std::unique_ptr<Renderer::RenderGraph::Pipeline> pipeline)
{
    if (m_activePipeline == nullptr)
        m_activePipeline = pipeline.get();
    m_pipelines.push_back(std::move(pipeline));
}

void Renderer::RenderGraph::RenderGraphManager::AssignCommandBuffers()
{
    auto& levelInfoList = m_activePipeline->GetPerLevelTaskInfo();

    std::optional<uint32_t> queueIdTracker;

    auto IsSameQueue = [&queueIdTracker](uint32_t actualQueueId) -> bool
    {
        if (!queueIdTracker.has_value())
        {
            queueIdTracker = actualQueueId;
            return true;
        }

        if (queueIdTracker != actualQueueId)
        {
            return false;
        }
        else
        {
            return true;
        }
        return false;
    };

    std::vector<std::optional<uint32_t>> graphicsCmdBufTracker, computeCmdBufTracker, transferCmdBufTracker;
    auto GetAvailableCommandBuffer = [](std::queue<uint32_t>& m_cmdBufAvailabilityList) -> uint32_t
    {
        auto id = m_cmdBufAvailabilityList.front();
        m_cmdBufAvailabilityList.pop();
        return id;
    };

    auto GetNewCommandBuffers = [&GetAvailableCommandBuffer, this](std::vector<Renderer::RenderGraph::TaskCommandBufferInfo>& commandBufferList,
        std::vector<std::optional<uint32_t>>& cmdBufferTracker,
        const Renderer::RenderGraph::TaskType& taskType)
    {
        cmdBufferTracker.clear();
        for (uint32_t i = 0; i < g_bufferDistributionCount; i++)
        {
            switch (taskType)
            {
            case Renderer::RenderGraph::TaskType::RENDER_TASK:
                cmdBufferTracker.push_back(GetAvailableCommandBuffer(m_perFrameInfo[i].m_graphicsCmdBufAvailabilityList));
                break;
            case Renderer::RenderGraph::TaskType::COMPUTE_TASK:
                cmdBufferTracker.push_back(GetAvailableCommandBuffer(m_perFrameInfo[i].m_computeCmdBufAvailabilityList));
                break;
            case Renderer::RenderGraph::TaskType::TRANSFER_TASK:
            case Renderer::RenderGraph::TaskType::DOWNLOAD_TASK:
                cmdBufferTracker.push_back(GetAvailableCommandBuffer(m_perFrameInfo[i].m_transferCmdBufAvailabilityList));
                break;
            }
            // if they have been assigned previously then get the last graphics buffer
            commandBufferList.push_back(Renderer::RenderGraph::TaskCommandBufferInfo{ cmdBufferTracker[i].value(), true, std::nullopt });
        }
    };

    std::vector<uint32_t> waitSemTracker, signalSemTracker;
    for (auto& item : m_acquireSemaphore)
        waitSemTracker.push_back(item);

    auto GetNewSemaphore = [this](std::vector<uint32_t>& semaphoreList)
    {
        for (uint32_t i = 0; i < g_semaphoreDistributionCount; i++)
        {
            semaphoreList.push_back(m_perFrameSemaphoreInfo[i].m_semaphoreAvailabilityList.front());
            m_perFrameSemaphoreInfo[i].m_semaphoreAvailabilityList.pop();
        }
    };

    auto ProcessSemaphores = [&waitSemTracker, &signalSemTracker, &GetNewSemaphore, this](
        Renderer::RenderGraph::Task* task,
        uint32_t nextTaskQueueId,
        bool shouldSignalPresentationSemaphore = false)
    {
        Core::Enums::PipelineType pipelineType;
        Core::Enums::QueueType queuePurpose;
        switch (task->GetTaskType())
        {
        case Renderer::RenderGraph::TaskType::RENDER_TASK:
            pipelineType = Core::Enums::PipelineType::GRAPHICS;
            queuePurpose = Core::Enums::QueueType::RENDER;
            break;
        case Renderer::RenderGraph::TaskType::COMPUTE_TASK:
            pipelineType = Core::Enums::PipelineType::COMPUTE;
            queuePurpose = Core::Enums::QueueType::COMPUTE;
            break;
        case Renderer::RenderGraph::TaskType::DOWNLOAD_TASK:
            ASSERT_MSG_DEBUG(0, "invalid");
            break;
        case Renderer::RenderGraph::TaskType::TRANSFER_TASK:
            pipelineType = Core::Enums::PipelineType::TRANSFER;
            queuePurpose = Core::Enums::QueueType::TRANSFER;
            break;
        default:
            ASSERT_MSG_DEBUG(0, "invalid");
            break;
        }
        // assign the existing wait to task

        std::vector<std::optional<uint32_t>> fenceIds;
        // get a new signal sem
        if (shouldSignalPresentationSemaphore)
        {
            for (uint32_t i = 0; i < g_semaphoreDistributionCount; i++)
            {
                signalSemTracker.push_back(m_presentationSemaphore[i]);
                fenceIds.push_back(m_acquireSwapchainFence[i]);
            }
        }
        else
        {
            GetNewSemaphore(signalSemTracker);
        }

        std::vector<Renderer::RenderGraph::TaskSubmitInfo> list;
        // assign the existing signal to task
        for (uint32_t i = 0; i < g_semaphoreDistributionCount; i++)
        {
            Renderer::RenderGraph::TaskSubmitInfo info = {};
            info.m_signalSemaphoreId = signalSemTracker[i];
            info.m_waitSemaphoreId = waitSemTracker[i];
            info.m_pipelineType = pipelineType;
            info.m_queuePurpose = queuePurpose;
            if (fenceIds.size() > 0)
                info.m_fenceId = fenceIds[i].value();
            list.push_back(std::move(info));
        }

        task->AssignSubmitInfo(list, nextTaskQueueId);

        // move the signal sem to wait
        waitSemTracker.clear();
        for (auto& item : signalSemTracker)
        {
            waitSemTracker.push_back(item);
        }
        signalSemTracker.clear();
    };


    auto ProcessCommandBuffers = [&IsSameQueue, &GetNewCommandBuffers, &queueIdTracker, &ProcessSemaphores](
        uint32_t queueId,
        std::vector<std::optional<uint32_t>>& cmdBufferTracker,
        Renderer::RenderGraph::Task* task,
        Renderer::RenderGraph::Task* prevTask)
    {
        std::vector<Renderer::RenderGraph::TaskCommandBufferInfo> commandBufferList;
        if (IsSameQueue(queueId))
        {
            if (cmdBufferTracker.size() == 0)
            {
                GetNewCommandBuffers(commandBufferList, cmdBufferTracker, task->GetTaskType());
            }
            else
            {
                for (auto& cmdId : cmdBufferTracker)
                    commandBufferList.push_back(Renderer::RenderGraph::TaskCommandBufferInfo{ cmdId.value(), std::nullopt, std::nullopt });
            }
            task->AssignCommandBufferInfo(commandBufferList, queueId, std::nullopt);
        }
        else
        {
            // The previous command buffer should stop recording
            prevTask->CloseTaskCommandBuffer();
            ProcessSemaphores(prevTask, queueId);

            // Queue switch has happened, get a new command buffer, start recording
            GetNewCommandBuffers(commandBufferList, cmdBufferTracker, task->GetTaskType());
            task->AssignCommandBufferInfo(commandBufferList, queueId, queueIdTracker);
            queueIdTracker = queueId;
        }
    };

    Renderer::RenderGraph::Task* prevTask = nullptr;
    for (auto& level : levelInfoList)
    {
        auto& taskInfo = level.second;
        for (auto taskNode : taskInfo.m_taskList)
        {
            auto task = ((Renderer::RenderGraph::TaskNode*)taskNode)->GetTask();
            switch (task->GetTaskType())
            {
                case Renderer::RenderGraph::TaskType::RENDER_TASK:
                    ProcessCommandBuffers(m_renderQueueId, graphicsCmdBufTracker, task, prevTask);
                    break;
                case Renderer::RenderGraph::TaskType::COMPUTE_TASK:
                    ProcessCommandBuffers(m_computeQueueId, computeCmdBufTracker, task, prevTask);
                    break;
                case Renderer::RenderGraph::TaskType::DOWNLOAD_TASK:
                    break;
                case Renderer::RenderGraph::TaskType::TRANSFER_TASK:
                    ProcessCommandBuffers(m_transferQueueId, transferCmdBufTracker, task, prevTask);
                    break;
                default:
                    break;
            }
            prevTask = task;
            m_pipelineEndTask = task;
        }
    }
    // Last task, hence command buffer should stop recording
    prevTask->CloseTaskCommandBuffer();

    // Last task, make it to signal the presentation semaphore
    //{
    //    for (uint32_t i = 0; i < g_semaphoreDistributionCount; i++)
    //    {
    //        signalSemTracker.push_back(m_presentationSemaphore[i]);
    //    }

    //    std::vector<Renderer::RenderGraph::TaskSubmitInfo> list;

    //    // assign the existing signal to task
    //    for (uint32_t i = 0; i < g_semaphoreDistributionCount; i++)
    //    {
    //        Renderer::RenderGraph::TaskSubmitInfo info = {};
    //        info.m_signalSemaphoreId = signalSemTracker[i];
    //        info.m_waitSemaphoreId = waitSemTracker[i];
    //        info.m_fenceId = m_acquireSwapchainFence[i];
    //        info.m_pipelineType = Core::Enums::PipelineType::TRANSFER;
    //        info.m_queuePurpose = Core::Enums::QueueType::TRANSFER;
    //        list.push_back(std::move(info));
    //    }

    //    ASSERT_MSG_DEBUG(prevTask->GetTaskType() == Renderer::RenderGraph::TaskType::TRANSFER_TASK, "last task has to blit to swapchain");
    //    prevTask->AssignSubmitInfo(list, m_presentationQueueId);
    //}
    ProcessSemaphores(prevTask, m_presentationQueueId, true);


}

void Renderer::RenderGraph::RenderGraphManager::AssignResourceInfo()
{
    // Calculate it for one frame, as same data will apply to all the textures for other frames
    struct TaskImageInfo
    {
        Renderer::RenderGraph::Task* m_task;
        Renderer::RenderGraph::Utils::ResourceMemoryUsage m_usage;
        Core::Enums::ImageLayout m_previous, m_expected;
    };

    std::map<ResourceAlias*, std::vector<TaskImageInfo>> resourceInfo;

    auto& levelInfoList = m_activePipeline->GetPerLevelTaskInfo();
    Renderer::RenderGraph::Task* lastTask = nullptr;

    // map a single resource to all the tasks where it is an input
    for (auto& level : levelInfoList)
    {
        auto& taskInfo = level.second;
        for (auto taskNode : taskInfo.m_taskList)
        {
            auto task = ((Renderer::RenderGraph::TaskNode*)taskNode)->GetTask();
            lastTask = task;
            auto& inputs = task->GetInputs();
            
            for (auto& input : inputs)
            {
                auto& resource = input.m_resource[0]; // only frame 0 resource is being used to generate data
                if (resource->GetResourceType() == Renderer::ResourceManagement::ResourceType::IMAGE)
                {
                    TaskImageInfo info{};
                    info.m_task = task;
                    //info.m_usage = input.m_usage;
                    info.m_previous = input.m_imageInfo->m_prevImageLayout;
                    info.m_expected = input.m_imageInfo->m_expectedImageLayout;

                    if (resourceInfo.find(resource) == resourceInfo.end())
                    {
                        resourceInfo.insert({ resource, {info} });
                    }
                    else
                    {
                        resourceInfo[resource].push_back(info);
                    }
                }
            }
        }
    }

    struct PerTaskImageInfo
    {
        ResourceAlias* m_resource;
        Core::Enums::LoadOperation m_load;
        Core::Enums::StoreOperation m_store;
        Core::Enums::ImageLayout m_prevLayout;
        Core::Enums::ImageLayout m_expectedLayout;
    };

    auto GetLoadOp = [](
        Core::Enums::ImageLayout previous,
        Core::Enums::ImageLayout expected) -> Core::Enums::LoadOperation
    {
        Core::Enums::LoadOperation load;
        if (previous == Core::Enums::ImageLayout::LAYOUT_UNDEFINED)
        {
            if (expected == Core::Enums::ImageLayout::LAYOUT_COLOR_ATTACHMENT_OPTIMAL ||
                expected == Core::Enums::ImageLayout::LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
                load = Core::Enums::LoadOperation::LOAD_OP_CLEAR;
            else
                load = Core::Enums::LoadOperation::LOAD_OP_DONT_CARE;
        }
        else
        {
            load = Core::Enums::LoadOperation::LOAD_OP_LOAD;
        }
        return load;
    };

    auto GetStoreOp = [](Core::Enums::ImageLayout previous) -> Core::Enums::StoreOperation
    {
        Core::Enums::StoreOperation store;

        return store;
    };

    std::map<Task*, std::vector<PerTaskImageInfo>> taskImageInfoList;
    // Info for one frame 
    auto CollectPerTaskInfo = [&]()
    {
        for (auto& item : resourceInfo)
        {
            Core::Enums::LoadOperation load = Core::Enums::LoadOperation::LOAD_OP_DONT_CARE;
            Core::Enums::StoreOperation store = Core::Enums::StoreOperation::STORE_OP_STORE;

            for (uint32_t i = 0; i < item.second.size(); i++)
            {
                PerTaskImageInfo perTaskImageInfo{};
                perTaskImageInfo.m_load = GetLoadOp(item.second[i].m_previous, item.second[i].m_expected);
                perTaskImageInfo.m_store = i == item.second.size() - 1 ?
                    Core::Enums::StoreOperation::STORE_OP_DONT_CARE : store;
                if (item.first->GetResourceName().find("BackBuffer") != std::string::npos)
                    perTaskImageInfo.m_store = Core::Enums::StoreOperation::STORE_OP_STORE;
                perTaskImageInfo.m_resource = item.first;
                perTaskImageInfo.m_prevLayout = item.second[i].m_previous;
                perTaskImageInfo.m_expectedLayout = item.second[i].m_expected;

                if (taskImageInfoList.find(item.second[i].m_task) == taskImageInfoList.end())
                {
                    taskImageInfoList.insert({ item.second[i].m_task, std::vector<PerTaskImageInfo>{perTaskImageInfo} });
                }
                else
                {
                    taskImageInfoList[item.second[i].m_task].push_back(perTaskImageInfo);
                }
            }
        }
    };
    CollectPerTaskInfo();

    auto isPartOfCollection = [](ResourceAlias* res,
        Renderer::RenderGraph::Utils::ConnectionInfo& connectionInfo) -> bool
    {
        auto& list = connectionInfo.m_resource;
        auto it = std::find_if(list.begin(), list.end(), [&](ResourceAlias* e)
        {return res->GetResourceName() == e->GetResourceName(); });

        if (it != list.end())
            return true;
        return false;
    };

    auto GetSrcAccessMask = [](Core::Enums::ImageLayout previousLayout) -> Core::Enums::PipelineAccessFlagBits2
    {
        Core::Enums::PipelineAccessFlagBits2 flag;
        switch (previousLayout)
        {
            case Core::Enums::ImageLayout::LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                flag = Core::Enums::PipelineAccessFlagBits2::ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
                break;
            case Core::Enums::ImageLayout::LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                flag = Core::Enums::PipelineAccessFlagBits2::ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                break;
            case Core::Enums::ImageLayout::LAYOUT_GENERAL:
                flag = Core::Enums::PipelineAccessFlagBits2::ACCESS_2_SHADER_WRITE_BIT;
                break;
            case Core::Enums::ImageLayout::LAYOUT_PRESENT_SRC_KHR: // previous present, just layout change
            case Core::Enums::ImageLayout::LAYOUT_TRANSFER_SRC_OPTIMAL: // previous present, just layout change, only read
                flag = Core::Enums::PipelineAccessFlagBits2::ACCESS_2_NONE;
                break;
            case Core::Enums::ImageLayout::LAYOUT_TRANSFER_DST_OPTIMAL:
                flag = Core::Enums::PipelineAccessFlagBits2::ACCESS_2_TRANSFER_WRITE_BIT;
                break;
            default:
                ASSERT_MSG_DEBUG(0, "invalid case");
        }
        return flag;
    };

    auto GetDstAccessMask = [](Core::Enums::ImageLayout expectedLayout) -> Core::Enums::PipelineAccessFlagBits2
    {
        Core::Enums::PipelineAccessFlagBits2 flag;
        switch (expectedLayout)
        {
        case Core::Enums::ImageLayout::LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            flag = Core::Enums::PipelineAccessFlagBits2::ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
            break;
        case Core::Enums::ImageLayout::LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            flag = Core::Enums::PipelineAccessFlagBits2::ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            break;
        case Core::Enums::ImageLayout::LAYOUT_GENERAL:
            flag = Core::Enums::PipelineAccessFlagBits2::ACCESS_2_SHADER_READ_BIT;
            break;
        case Core::Enums::ImageLayout::LAYOUT_PRESENT_SRC_KHR:
            flag = Core::Enums::PipelineAccessFlagBits2::ACCESS_2_NONE;
            break;
        case Core::Enums::ImageLayout::LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            flag = Core::Enums::PipelineAccessFlagBits2::ACCESS_2_SHADER_READ_BIT;
            break;
        case Core::Enums::ImageLayout::LAYOUT_TRANSFER_DST_OPTIMAL:
        case Core::Enums::ImageLayout::LAYOUT_TRANSFER_SRC_OPTIMAL: // doubtful case
            flag = Core::Enums::PipelineAccessFlagBits2::ACCESS_2_TRANSFER_READ_BIT;
            break;
        default:
            ASSERT_MSG_DEBUG(0, "invalid case");
        }
        return flag;
    };


    auto GetStageMask = [](Core::Enums::ImageLayout layout) ->std::vector < Core::Enums::PipelineStageFlagBits2>
    {
        std::vector<Core::Enums::PipelineStageFlagBits2> flags;
        switch (layout)
        {
        case Core::Enums::ImageLayout::LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            flags.push_back(Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
            break;
        case Core::Enums::ImageLayout::LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            flags.push_back(Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT);
            break;
        case Core::Enums::ImageLayout::LAYOUT_GENERAL:
            flags.push_back(Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_COMPUTE_SHADER_BIT);
            break;
        case Core::Enums::ImageLayout::LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            flags.push_back(Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_COMPUTE_SHADER_BIT);
            flags.push_back(Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_VERTEX_SHADER_BIT);
            flags.push_back(Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT);
            break;
        case Core::Enums::ImageLayout::LAYOUT_TRANSFER_DST_OPTIMAL:
            flags.push_back(Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_ALL_TRANSFER_BIT);
            break;
        case Core::Enums::ImageLayout::LAYOUT_PRESENT_SRC_KHR:
        case Core::Enums::ImageLayout::LAYOUT_UNDEFINED:
            flags.push_back(Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_TOP_OF_PIPE_BIT);
            break;
        case Core::Enums::ImageLayout::LAYOUT_TRANSFER_SRC_OPTIMAL:
            flags.push_back(Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_ALL_TRANSFER_BIT);
            break;
        default:
            ASSERT_MSG_DEBUG(0, "invalid case");
        }

        return flags;
    };

    auto CollectRenderingData = [](std::vector<Core::Wrappers::RenderingInfo>& renderingInfoPerFrame,
        const PerTaskImageInfo& imageInfo, const uint32_t& imageId)
    {

    };

    auto CollectBarrierData = [&GetSrcAccessMask, &GetDstAccessMask, 
    &GetStageMask](const PerTaskImageInfo& imageInfo, const uint32_t& imageId,
        bool isFlushRequiredThroughBarrier, const std::string& imageName) -> Core::Wrappers::ImageBarrier2
    {
        Core::Wrappers::ImageBarrier2 barrier{};
        barrier.m_dstQueueFamilyIndex = 0;
        barrier.m_imageId = imageId;
        barrier.m_newLayout = imageInfo.m_expectedLayout;
        barrier.m_oldLayout = imageInfo.m_prevLayout;
        barrier.m_srcQueueFamilyIndex = 0;
        barrier.m_subresourceRange.m_baseArrayLayer = 0;
        barrier.m_subresourceRange.m_baseMipLevel = 0;
        barrier.m_subresourceRange.m_imageAspect.push_back(Core::Enums::ImageAspectFlag::IMAGE_ASPECT_COLOR_BIT);
        barrier.m_subresourceRange.m_layerCount = 1;
        barrier.m_subresourceRange.m_levelCount = 1;
        barrier.m_imageName = imageName;

        if (isFlushRequiredThroughBarrier == false)
        {
            barrier.m_srcStageMask.push_back(Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_TOP_OF_PIPE_BIT);
            barrier.m_srcAccessMask.push_back(Core::Enums::PipelineAccessFlagBits2::ACCESS_2_NONE);
            barrier.m_dstStageMask.push_back(Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT);
            barrier.m_dstAccessMask.push_back(Core::Enums::PipelineAccessFlagBits2::ACCESS_2_NONE);
        }
        else
        {
            // No write needs to be flushed only layout transition required
            if (imageInfo.m_prevLayout == Core::Enums::ImageLayout::LAYOUT_SHADER_READ_ONLY_OPTIMAL ||
                imageInfo.m_prevLayout == Core::Enums::ImageLayout::LAYOUT_TRANSFER_SRC_OPTIMAL ||
                imageInfo.m_prevLayout == Core::Enums::ImageLayout::LAYOUT_PRESENT_SRC_KHR ||
                imageInfo.m_prevLayout == Core::Enums::ImageLayout::LAYOUT_PREINITIALIZED ||
                imageInfo.m_prevLayout == Core::Enums::ImageLayout::LAYOUT_UNDEFINED)
            {
                barrier.m_srcStageMask.push_back(Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_TOP_OF_PIPE_BIT);
                barrier.m_srcAccessMask.push_back(Core::Enums::PipelineAccessFlagBits2::ACCESS_2_NONE);
            }
            else
            {
                barrier.m_srcAccessMask.push_back(GetSrcAccessMask(imageInfo.m_prevLayout));
                barrier.m_srcStageMask = GetStageMask(imageInfo.m_prevLayout);
            }

            // No write needs to be flushed only layout transition required
            if (imageInfo.m_expectedLayout == Core::Enums::ImageLayout::LAYOUT_PRESENT_SRC_KHR)
            {
                barrier.m_dstAccessMask.push_back(Core::Enums::PipelineAccessFlagBits2::ACCESS_2_NONE);
                barrier.m_dstStageMask.push_back(Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT);
            }
            else
            {
                barrier.m_dstAccessMask.push_back(GetDstAccessMask(imageInfo.m_expectedLayout));
                barrier.m_dstStageMask = GetStageMask(imageInfo.m_expectedLayout);
            }
        }
        return barrier;
    };

    auto CollectTaskImageData = [&taskImageInfoList, &isPartOfCollection,
        &GetSrcAccessMask, &GetStageMask, &GetDstAccessMask, &CollectBarrierData](
            std::vector<Renderer::RenderGraph::PerFrameTaskBarrierInfo>& barrierInfoForFrames,
            std::vector<Core::Wrappers::RenderingInfo>& renderingInfoPerFrame,
            std::vector<PerTaskImageInfo>& perTaskImageInfo,
            std::vector<Renderer::RenderGraph::Utils::ConnectionInfo>& taskInputList,
            const bool& isFlushRequiredThroughBarrier)
    {
        for (auto& imageInfo : perTaskImageInfo)
        {
            // Get the collection to which the image belongs, get the remaining image ids
            for (auto& taskInput : taskInputList)
            {
                if (taskInput.m_resource[0]->GetResourceType() != Renderer::ResourceManagement::ResourceType::IMAGE)
                    continue;

                bool check = isPartOfCollection(imageInfo.m_resource, taskInput);

                if (check)
                {
                    // Create barrier/render pass info
                    uint32_t frameCount = 0;
                    for (auto& taskRes : taskInput.m_resource)
                    {
                        auto imageId = taskRes->GetPhysicalResourceId();

                        // Barrier
                        if (imageInfo.m_expectedLayout != imageInfo.m_prevLayout)
                        {
                            auto barrier = CollectBarrierData(imageInfo, imageId, isFlushRequiredThroughBarrier, taskRes->GetResourceName());
                            barrierInfoForFrames[frameCount].m_imageBarriers.push_back(barrier);
                        }

                        // Rendering Info
                        if (imageInfo.m_expectedLayout == Core::Enums::ImageLayout::LAYOUT_COLOR_ATTACHMENT_OPTIMAL||
                            imageInfo.m_expectedLayout == Core::Enums::ImageLayout::LAYOUT_DEPTH_ATTACHMENT_OPTIMAL||
                            imageInfo.m_expectedLayout == Core::Enums::ImageLayout::LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
                        {
                            Core::Wrappers::RenderingAttachmentInfo info;
                            info.m_imageId = imageId;
                            info.m_imageLayout = imageInfo.m_expectedLayout;
                            info.m_loadOp = imageInfo.m_load;
                            info.m_storeOp = imageInfo.m_store;
                            if (imageInfo.m_expectedLayout == Core::Enums::ImageLayout::LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                            {
                                info.m_clearColorValues = ((ImageResourceAlias*)taskRes)->GetClearColorValue();
                                info.m_imageFormat = ((ImageResourceAlias*)taskRes)->GetImageFormat();
                                auto& colorList = renderingInfoPerFrame[frameCount].m_colorAttachmentInfo;
                                auto colorAttachmentSlot = taskInput.m_imageInfo.value().m_colorAttachmentSlot.value();
                                
                                if (colorAttachmentSlot >= colorList.size())
                                {
                                    colorList.resize(colorAttachmentSlot + 1);
                                }

                                colorList[colorAttachmentSlot] = info;
                            }
                            else
                            {
                                info.m_clearDepthStencilValues = ((ImageResourceAlias*)taskRes)->GetClearDepthValue();
                                info.m_imageFormat = ((ImageResourceAlias*)taskRes)->GetImageFormat();
                                renderingInfoPerFrame[frameCount].m_depthAttachmentInfo = info;
                            }
                        }
                        frameCount++;
                    }
                }
            }
        }
    };


    auto CollectBufferBarrier = [](
        std::vector<Renderer::RenderGraph::PerFrameTaskBarrierInfo>& barrierInfoForFrames,
        std::vector<Renderer::RenderGraph::Utils::ConnectionInfo>& taskInputList)
    {
        for (auto& taskInput : taskInputList)
        {
            // skip barrier if its being written by host || not a buffer resource
            if (taskInput.m_resource[0]->GetResourceType() != Renderer::ResourceManagement::ResourceType::BUFFER ||
                (taskInput.m_bufInfo.value().prevShader.has_value() == false && taskInput.m_bufInfo.value().previousUsage.has_value() == false))
                continue;

            Core::Enums::PipelineStageFlagBits2 srcStage = Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_NONE;
            Core::Enums::PipelineStageFlagBits2 dstStage = Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_NONE;
            Core::Enums::PipelineAccessFlagBits2 srcAccess = Core::Enums::PipelineAccessFlagBits2::ACCESS_2_NONE;
            Core::Enums::PipelineAccessFlagBits2 dstAccess = Core::Enums::PipelineAccessFlagBits2::ACCESS_2_NONE;

            // get shader stage src stage
            if (taskInput.m_bufInfo.value().prevShader.has_value())
            {
                if (taskInput.m_bufInfo.value().prevShader.value() == Core::Enums::ShaderType::COMPUTE)
                {
                    srcStage = Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
                }
                else if (taskInput.m_bufInfo.value().prevShader.value() == Core::Enums::ShaderType::FRAGMENT)
                {
                    srcStage = Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
                }
                else if (taskInput.m_bufInfo.value().prevShader.value() == Core::Enums::ShaderType::VERTEX)
                {
                    srcStage = Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
                }
                srcAccess = Core::Enums::PipelineAccessFlagBits2::ACCESS_2_MEMORY_WRITE_BIT;
            }
            else if (taskInput.m_bufInfo.value().previousUsage.has_value())
            {
                if (taskInput.m_bufInfo.value().previousUsage.value() == Core::Enums::BufferUsage::BUFFER_USAGE_TRANSFER_DST_BIT)
                {
                    srcStage = Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_ALL_TRANSFER_BIT;
                    srcAccess = Core::Enums::PipelineAccessFlagBits2::ACCESS_2_TRANSFER_WRITE_BIT;
                }
                else if (taskInput.m_bufInfo.value().previousUsage.value() == Core::Enums::BufferUsage::BUFFER_USAGE_TRANSFER_SRC_BIT)
                {
                    srcStage = Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
                    srcAccess = Core::Enums::PipelineAccessFlagBits2::ACCESS_2_NONE;
                }
            }
            else
            {
                srcStage = Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
                srcAccess = Core::Enums::PipelineAccessFlagBits2::ACCESS_2_NONE;
            }

            ASSERT_MSG_DEBUG(srcStage != Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_NONE, " invalid ");

            if (taskInput.m_bufInfo.value().expectedShader.has_value())
            {
                if (taskInput.m_bufInfo.value().expectedShader.value() == Core::Enums::ShaderType::COMPUTE)
                {
                    dstStage = Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
                }
                else if (taskInput.m_bufInfo.value().expectedShader.value() == Core::Enums::ShaderType::FRAGMENT)
                {
                    dstStage = Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
                }
                else if (taskInput.m_bufInfo.value().expectedShader.value() == Core::Enums::ShaderType::VERTEX)
                {
                    dstStage = Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
                }
                dstAccess = Core::Enums::PipelineAccessFlagBits2::ACCESS_2_MEMORY_READ_BIT;
            }
            else
            {
                if (taskInput.m_bufInfo.value().expectedUsage == Core::Enums::BufferUsage::BUFFER_USAGE_TRANSFER_DST_BIT)
                {
                    dstStage = Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_ALL_TRANSFER_BIT;
                    dstAccess = Core::Enums::PipelineAccessFlagBits2::ACCESS_2_TRANSFER_READ_BIT;
                }
                else if (taskInput.m_bufInfo.value().expectedUsage == Core::Enums::BufferUsage::BUFFER_USAGE_TRANSFER_SRC_BIT)
                {
                    dstStage = Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
                    dstAccess = Core::Enums::PipelineAccessFlagBits2::ACCESS_2_NONE;
                }
            }

            ASSERT_MSG_DEBUG(dstStage != Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_NONE, " invalid ");

            // Create barrier
            uint32_t frameCount = 0;
            for (auto& taskRes : taskInput.m_resource)
            {
                auto bufferId = taskRes->GetPhysicalResourceId();
                auto bufResource = (BufferResourceAlias*)taskRes;

                // Barrier case
                Core::Wrappers::BufferBarrier2 barrier{};
                barrier.m_dstQueueFamilyIndex = 0;
                barrier.m_srcQueueFamilyIndex = 0;
                barrier.m_bufferName = taskRes->GetResourceName();
                barrier.m_bufferId = bufResource->GetPhysicalResourceId();
                barrier.m_dstAccessMask = std::vector<Core::Enums::PipelineAccessFlagBits2>{ dstAccess };
                barrier.m_dstStageMask = std::vector<Core::Enums::PipelineStageFlagBits2>{ dstStage };
                barrier.m_offset = bufResource->GetDataOffset();
                barrier.m_size = bufResource->GetAlignedDataSize();
                barrier.m_srcAccessMask = std::vector<Core::Enums::PipelineAccessFlagBits2>{ srcAccess };
                barrier.m_srcStageMask = std::vector<Core::Enums::PipelineStageFlagBits2>{ srcStage };

                barrierInfoForFrames[frameCount++].m_bufferBarriers.push_back(barrier);
            }
        }
    };

    auto CreatePresentationBarrier = [&GetSrcAccessMask, &GetStageMask](Renderer::RenderGraph::Task* task)
    {
        Core::Enums::ImageLayout swapchainPrevLayout = Core::Enums::ImageLayout::LAYOUT_UNDEFINED;
        std::vector<uint32_t> swapchainImageIds;

        std::vector<Core::Wrappers::ImageBarrier2> barriers;
        std::vector<PerFrameTaskBarrierInfo> perFrameInfo;

        bool swapchainFound = false;

        for (auto& input : task->GetInputs())
        {
            // Check if input contains BackBuffer
            for (auto& res : input.m_resource)
            {
                if (res->GetResourceName().find("BackBuffer") != std::string::npos)
                {
                    swapchainPrevLayout = input.m_imageInfo.value().m_expectedImageLayout;
                    swapchainImageIds.push_back(res->GetPhysicalResourceId());
                    swapchainFound = true;
                }
            }
            if (swapchainFound)
                break;
        }

        ASSERT_MSG_DEBUG(swapchainPrevLayout != Core::Enums::ImageLayout::LAYOUT_UNDEFINED, "BackBuffer not Found");
        perFrameInfo.resize(RendererSettings::GetSwapBufferCount());
        for (uint32_t i = 0; i < RendererSettings::GetSwapBufferCount(); i++)
        {
            Core::Wrappers::ImageBarrier2 barrier{};
            barrier.m_dstQueueFamilyIndex = 0;
            barrier.m_imageId = swapchainImageIds[i];
            barrier.m_oldLayout = swapchainPrevLayout;
            barrier.m_newLayout = Core::Enums::ImageLayout::LAYOUT_PRESENT_SRC_KHR;
            barrier.m_srcQueueFamilyIndex = 0;
            barrier.m_subresourceRange.m_baseArrayLayer = 0;
            barrier.m_subresourceRange.m_baseMipLevel = 0;
            barrier.m_subresourceRange.m_imageAspect.push_back(Core::Enums::ImageAspectFlag::IMAGE_ASPECT_COLOR_BIT);
            barrier.m_subresourceRange.m_layerCount = 1;
            barrier.m_subresourceRange.m_levelCount = 1;
            barrier.m_imageName = "BackBuffer_" + std::to_string(i);
            barrier.m_dstStageMask.push_back(Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT);
            barrier.m_dstAccessMask.push_back(Core::Enums::PipelineAccessFlagBits2::ACCESS_2_NONE);
            {
                // No write needs to be flushed only layout transition required
                if (swapchainPrevLayout == Core::Enums::ImageLayout::LAYOUT_SHADER_READ_ONLY_OPTIMAL ||
                    swapchainPrevLayout == Core::Enums::ImageLayout::LAYOUT_TRANSFER_SRC_OPTIMAL ||
                    swapchainPrevLayout == Core::Enums::ImageLayout::LAYOUT_PRESENT_SRC_KHR ||
                    swapchainPrevLayout == Core::Enums::ImageLayout::LAYOUT_PREINITIALIZED ||
                    swapchainPrevLayout == Core::Enums::ImageLayout::LAYOUT_UNDEFINED)
                {
                    barrier.m_srcStageMask.push_back(Core::Enums::PipelineStageFlagBits2::PIPELINE_STAGE_2_TOP_OF_PIPE_BIT);
                    barrier.m_srcAccessMask.push_back(Core::Enums::PipelineAccessFlagBits2::ACCESS_2_NONE);
                }
                else
                {
                    barrier.m_srcAccessMask.push_back(GetSrcAccessMask(swapchainPrevLayout));
                    barrier.m_srcStageMask = GetStageMask(swapchainPrevLayout);
                }
            }
            perFrameInfo[i].m_imageBarriers.push_back(barrier);
        }
        task->AssignBarrierInfo(perFrameInfo, true);
    };

    // for all frames
    auto PopulateTaskInfo = [&taskImageInfoList, &isPartOfCollection, &CreatePresentationBarrier,
    &GetSrcAccessMask, &GetStageMask, &GetDstAccessMask, &CollectTaskImageData, &CollectBufferBarrier]()
    {
        for (auto& item : taskImageInfoList)
        {
            auto& task = item.first;
            auto taskInputList = task->GetInputs();

            bool isFlushRequiredThroughBarrier = true;
            if(task->GetCommandBufferInfo()[0].m_shouldStart.has_value())
                isFlushRequiredThroughBarrier = !task->GetCommandBufferInfo()[0].m_shouldStart.value();

            std::vector<Renderer::RenderGraph::PerFrameTaskBarrierInfo> barrierInfoForFrames(g_bufferDistributionCount);
            std::vector<Core::Wrappers::RenderingInfo> renderingInfoForFrames(g_bufferDistributionCount);
            CollectTaskImageData(barrierInfoForFrames, renderingInfoForFrames, item.second,
                taskInputList, isFlushRequiredThroughBarrier);

            if(isFlushRequiredThroughBarrier)
                CollectBufferBarrier(barrierInfoForFrames, taskInputList);

            task->AssignBarrierInfo(barrierInfoForFrames);
            if (task->GetTaskType() == Renderer::RenderGraph::TaskType::RENDER_TASK)
            {
                for (auto& info : renderingInfoForFrames)
                    info.m_renderArea = ((Renderer::RenderGraph::Tasks::RenderTask*)task)->GetRenderArea();
                ((Renderer::RenderGraph::Tasks::RenderTask*)task)->AssignRenderingInfo(renderingInfoForFrames);
                ((Renderer::RenderGraph::Tasks::RenderTask*)task)->CreateGraphicsPipeline();
            }
        }
    };

    PopulateTaskInfo();
    CreatePresentationBarrier(lastTask);

    auto PrintImageInfo = [&]()
    {
        for (auto& item : resourceInfo)
        {
            std::cout << "\nImage : " << item.first->GetResourceName() << "\n";
            for (auto& info : item.second)
            {
                std::cout <<"  " << info.m_task->GetTaskName() << "  ";
            }
        }
    };

    auto PrintTaskInfo = [&]()
    {
        auto GetLoadValue = [](Core::Enums::LoadOperation load) -> std::string
        {
            std::string value = "";
            if (load == Core::Enums::LoadOperation::LOAD_OP_CLEAR)
            {
                value = "LOAD_OP_CLEAR";
            }
            else if (load == Core::Enums::LoadOperation::LOAD_OP_DONT_CARE)
            {
                value = "LOAD_OP_DONT_CARE";
            }
            else
                value = "LOAD_OP_LOAD";
            return value;
        };

        for (auto& item : taskImageInfoList)
        {
            std::cout << "\nTask : " << item.first->GetTaskName();
            for (auto& res : item.second)
            {
                std::cout << "\n    resource : " << res.m_resource->GetResourceName();
                std::cout << "\n        load : " << GetLoadValue(res.m_load);
                if (res.m_store == Core::Enums::StoreOperation::STORE_OP_STORE)
                    std::cout << "\n        store : " << "STORE_OP_STORE";
                else
                    std::cout << "\n        store : " << "STORE_OP_DONT_CARE";
            }
            std::cout << "\n=========";
        }
    };

    //PrintImageInfo();
    //PrintTaskInfo();
}

void Renderer::RenderGraph::RenderGraphManager::Init(
    uint32_t renderQueueId,
    uint32_t computeQueueId,
    uint32_t transferQueueId,
    uint32_t presentationQueueId,
    std::unique_ptr<Renderer::RenderGraph::Pipeline> pipeline)
{
    PLOGD << "RenderGraphManager Init";

    g_bufferDistributionCount = Core::Settings::m_swapBufferCount;
    g_semaphoreDistributionCount = Core::Settings::m_maxFramesInFlight;
    g_fenceDistributionCount = Core::Settings::m_maxFramesInFlight;

    m_renderQueueId = renderQueueId;
    m_computeQueueId = computeQueueId;
    m_transferQueueId = transferQueueId;
    m_presentationQueueId = presentationQueueId;
    
    auto FillCommandBuffer = [&](std::vector<uint32_t>& bufList, uint32_t bufCount, const Core::Enums::QueueType& queueType)
    {
        for (uint32_t i = 0; i < bufCount; i++)
        {
            auto id = Renderer::Utility::VulkanInterface::CreateCommandBuffer(queueType);
            bufList.push_back(id);
        }
    };

    auto FillSemaphore = [&](std::vector<uint32_t>& semaphoreList, uint32_t semaphoreCount)
    {
        for (uint32_t i = 0; i < semaphoreCount; i++)
        {
            auto id = Renderer::Utility::VulkanInterface::Create_Semaphore(false);
            semaphoreList.push_back(id);
        }
    };

    auto FillAvailability = [&](std::queue<uint32_t>& availabilityList, std::vector<uint32_t>& bufList)
    {
        for (auto id : bufList)
        {
            availabilityList.push(id);
        }
    };

    // populate command buffer list
    for (uint32_t i = 0; i < Core::Settings::m_swapBufferCount; i++)
    {
        PerFrameCommandBufferInfo info{};
        FillCommandBuffer(info.m_graphicsCmdBufList, info.c_maxGraphicsBufferCount, Core::Enums::QueueType::RENDER);
        FillCommandBuffer(info.m_computeCmdBufList, info.c_maxComputeBufferCount, Core::Enums::QueueType::COMPUTE);
        FillCommandBuffer(info.m_transferCmdBufList, info.c_maxTransferBufferCount, Core::Enums::QueueType::TRANSFER);

        FillAvailability(info.m_graphicsCmdBufAvailabilityList, info.m_graphicsCmdBufList);
        FillAvailability(info.m_computeCmdBufAvailabilityList, info.m_computeCmdBufList);
        FillAvailability(info.m_transferCmdBufAvailabilityList, info.m_transferCmdBufList);
        m_perFrameInfo.push_back(std::move(info));
    }

    FillSemaphore(m_acquireSemaphore, g_semaphoreDistributionCount);
    FillSemaphore(m_presentationSemaphore, g_semaphoreDistributionCount);

    for (uint32_t i = 0; i < g_semaphoreDistributionCount; i++)
    {
        PerFrameSemaphoreInfo info{};
        FillSemaphore(info.m_semaphoreList, info.c_maxSemaphoreCount);
        FillAvailability(info.m_semaphoreAvailabilityList, info.m_semaphoreList);
        m_perFrameSemaphoreInfo.push_back(std::move(info));
    }

    for (uint32_t i = 0; i < g_fenceDistributionCount; i++)
    {
        m_acquireSwapchainFence.push_back(Renderer::Utility::VulkanInterface::CreateFence(true));
    }

    AddPipeline(std::move(pipeline));

    for (auto& pipeline : m_pipelines)
    {
        pipeline->CreatePipeline();
    }

    for (auto& pipeline : m_pipelines)
    {
        pipeline->CompilePipeline();
    }

    AssignCommandBuffers();
    AssignResourceInfo();

    auto PrintTaskInfo = [&]()
    {
        for (auto& level : m_activePipeline->GetPerLevelTaskInfo())
        {
            auto& taskInfo = level.second;
            for (auto taskNode : taskInfo.m_taskList)
            {
                auto task = ((Renderer::RenderGraph::TaskNode*)taskNode)->GetTask();
                task->PrintTaskInfo();
            }
        }
    };

    //PrintTaskInfo();

    m_presentationTask = std::make_unique<Renderer::RenderGraph::Tasks::PresentationTask>("Presentation",
        Core::Settings::m_swapBufferCount,
        m_windowSettings.m_windowWidth,
        m_windowSettings.m_windowHeight,
        m_presentationQueueId);

    std::vector<Renderer::RenderGraph::TaskSubmitInfo> infoList;
    for (uint32_t i = 0; i < g_fenceDistributionCount; i++)
    {
        Renderer::RenderGraph::TaskSubmitInfo info{};
        info.m_fenceId = m_acquireSwapchainFence[i];
        info.m_pipelineType = Core::Enums::PipelineType::GRAPHICS;
        info.m_queuePurpose = Core::Enums::QueueType::PRESENT;
        info.m_waitSemaphoreId = m_pipelineEndTask->GetTaskSubmitInfo()[i].m_signalSemaphoreId.value();
        infoList.push_back(std::move(info));
    }
    m_presentationTask->AssignSubmitInfo(infoList, std::nullopt);
}

void Renderer::RenderGraph::RenderGraphManager::DeInit()
{
    PLOGD << "RenderGraphManager DeInit";

    m_presentationTask.reset();

    for (auto& pipeline : m_pipelines)
    {
        pipeline->DestroyPipeline();
    }

    for (uint32_t i = 0; i < g_fenceDistributionCount; i++)
    {
        Renderer::Utility::VulkanInterface::DestroyFence(m_acquireSwapchainFence[i]);
    }

    for (auto& id : m_acquireSemaphore)
        Renderer::Utility::VulkanInterface::DestroySemaphore(id);
    for (auto& id : m_presentationSemaphore)
        Renderer::Utility::VulkanInterface::DestroySemaphore(id);

    for (auto& item : m_perFrameSemaphoreInfo)
    {
        for (auto& id : item.m_semaphoreList)
            Renderer::Utility::VulkanInterface::DestroySemaphore(id);
    }

    auto FreeCommandBuffer = [&](std::vector<uint32_t>& bufList, const Core::Enums::QueueType& queueType)
    {
        for (auto bufId : bufList)
        {
            Renderer::Utility::VulkanInterface::DestroyCommandBuffer(bufId, queueType);
        }
    };

    // Delete command buffer list
    for (auto& info : m_perFrameInfo)
    {
        FreeCommandBuffer(info.m_graphicsCmdBufList, Core::Enums::QueueType::RENDER);
        FreeCommandBuffer(info.m_computeCmdBufList, Core::Enums::QueueType::COMPUTE);
        FreeCommandBuffer(info.m_transferCmdBufList, Core::Enums::QueueType::TRANSFER);
    }
}

void Renderer::RenderGraph::RenderGraphManager::Update(const Core::Wrappers::FrameInfo& frameInfo)
{
    auto& levelInfoList = m_activePipeline->GetPerLevelTaskInfo();
    for (auto& level : levelInfoList)
    {
        auto& taskInfo = level.second;
        for (auto taskNode : taskInfo.m_taskList)
        {
            auto task = ((Renderer::RenderGraph::TaskNode*)taskNode)->GetTask();
            task->Execute(frameInfo);
            m_pipelineEndTask = task;
        }
    }

    m_presentationTask->Execute(frameInfo);
}

void Renderer::RenderGraph::RenderGraphManager::SetupFrame(Core::Wrappers::FrameInfo& frameInfo)
{
    // Acquire the swapchain index
    auto currentFenceId = m_acquireSwapchainFence[m_frameInFlightIndex];
    auto currentRenderSemaphoreId = m_acquireSemaphore[m_frameInFlightIndex];

    auto currentSwapchainIndex = Renderer::Utility::VulkanInterface::GetAvailableSwapchainIndex(currentFenceId, currentRenderSemaphoreId);

    frameInfo.m_farmeInFlightIndex = m_frameInFlightIndex;
    frameInfo.m_swapBufferIndex = currentSwapchainIndex;
}

void Renderer::RenderGraph::RenderGraphManager::EndFrame()
{
    m_frameInFlightIndex = (m_frameInFlightIndex + 1) % Core::Settings::m_maxFramesInFlight;
}

void Renderer::RenderGraph::RenderGraphManager::OnExit()
{
    VulkanInterfaceAlias::IsApplicationSafeForClosure();
}

Renderer::RenderGraph::RenderGraphManager::~RenderGraphManager()
{
    for (auto& pipeline : m_pipelines)
        pipeline.reset();
    m_pipelines.clear();
}

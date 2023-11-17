#include "RenderGraph/RenderGraphManager.h"
#include "Settings.h"
#include "VulkanInterface.h"
#include "renderGraph/Task.h"

uint32_t g_bufferDistributionCount = 0;
uint32_t g_semaphoreDistributionCount = 0;
uint32_t g_fenceDistributionCount = 0;

Renderer::RenderGraph::RenderGraphManager::RenderGraphManager(const Core::Utility::RenderData& renderData):
    m_renderData(renderData)
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
        Renderer::RenderGraph::Task* task, bool shouldSignalPresentationSemaphore = false)
    {
        /*std::cout << "======= sem start" << task->GetTaskName() << "\n";
        std::cout << "wait \n";
        for (auto& item : waitSemTracker)
        {
            std::cout << item << "  ";
        }*/
        // assign the existing wait to task

        // get a new signal sem
        if (shouldSignalPresentationSemaphore)
        {
            for (uint32_t i = 0; i < g_semaphoreDistributionCount; i++)
            {
                signalSemTracker.push_back(m_presentationSemaphore[i]);
            }
        }
        else
        {
            GetNewSemaphore(signalSemTracker);
        }

        //std::cout << "\nsignal \n";
        //for (auto& item : signalSemTracker)
        //{
        //    std::cout << item << "  ";
        //}

        std::vector<Renderer::RenderGraph::TaskSubmitInfo> list;

        // assign the existing signal to task
        for (uint32_t i = 0; i < g_semaphoreDistributionCount; i++)
        {
            Renderer::RenderGraph::TaskSubmitInfo info = {};
            info.m_signalSemaphoreId = signalSemTracker[i];
            info.m_waitSemaphoreId = waitSemTracker[i];
            list.push_back(std::move(info));
        }
        
        task->AssignSubmitInfo(list);

        // move the signal sem to wait
        waitSemTracker.clear();
        for (auto& item : signalSemTracker)
        {
            waitSemTracker.push_back(item);
        }
        signalSemTracker.clear();

        /*std::cout << "\nwait \n";
        for (auto& item : waitSemTracker)
        {
            std::cout << item << "  ";
        }
        std::cout << "\n======= sem end\n";*/
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
            task->AssignCommandBufferInfo(commandBufferList);
        }
        else
        {
            // The previous command buffer should stop recording
            prevTask->CloseTaskCommandBuffer();
            ProcessSemaphores(prevTask);
            // queue switch has happened, get a new command buffer, start recording
            GetNewCommandBuffers(commandBufferList, cmdBufferTracker, task->GetTaskType());
            task->AssignCommandBufferInfo(commandBufferList);
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
        }
    }
    // Last task, hence command buffer should stop recording
    prevTask->CloseTaskCommandBuffer();
    // Last task, make it to signal the presentation semaphore
    ProcessSemaphores(prevTask, true);

    auto PrintTaskInfo = [&]()
    {
        for (auto& level : levelInfoList)
        {
            auto& taskInfo = level.second;
            for (auto taskNode : taskInfo.m_taskList)
            {
                auto task = ((Renderer::RenderGraph::TaskNode*)taskNode)->GetTask();
                task->PrintTaskInfo();
            }
        }
    };

    PrintTaskInfo();
}

void Renderer::RenderGraph::RenderGraphManager::Init(uint32_t renderQueueId, uint32_t computeQueueId, uint32_t transferQueueId)
{
    PLOGD << "RenderGraphManager Init";

    g_bufferDistributionCount = Core::Settings::m_swapBufferCount;
    g_semaphoreDistributionCount = Core::Settings::m_maxFramesInFlight;
    g_fenceDistributionCount = Core::Settings::m_maxFramesInFlight;

    m_renderQueueId = renderQueueId;
    m_computeQueueId = computeQueueId;
    m_transferQueueId = transferQueueId;

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

    for (auto& pipeline : m_pipelines)
    {
        pipeline->CompilePipeline();
    }

    AssignCommandBuffers();
}

void Renderer::RenderGraph::RenderGraphManager::DeInit()
{
    PLOGD << "RenderGraphManager DeInit";

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

void Renderer::RenderGraph::RenderGraphManager::Update(const uint32_t frameIndex)
{
}

Renderer::RenderGraph::RenderGraphManager::~RenderGraphManager()
{
    for (auto& pipeline : m_pipelines)
        pipeline.reset();
    m_pipelines.clear();
}

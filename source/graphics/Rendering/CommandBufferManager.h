#pragma once
#include <Settings.h>
#include <Assertion.h>
#include <vector>

enum class CommandPoolProperty;
enum class PipelineType;
enum class CommandBufferLevel;
enum class CommandBufferUsage;
struct CommandBufferInheritanceInfo;
typedef PipelineType CommandBufferType;

template <typename T>
class DrawCommandBuffer;

class VulkanInterface;

#define MAX_COMMANDBUFFERS 10

struct PoolWrapper
{
    uint32_t poolId;
    CommandPoolProperty * prop;
    PipelineType * pipelineType;
};


template < typename T>
class CommandBufferManager
{
private:
    CommandBufferManager() {};
    CommandBufferManager(CommandBufferManager const &) {}
    CommandBufferManager const & operator= (CommandBufferManager const &) {}

    static CommandBufferManager* instance;

    T * apiInterface;
    uint32_t idCounter = 0, drawCB_Counter = 0;

    DrawCommandBuffer<T> * drawCommandBufferList;
    //DispatchCommandBuffer<T> * DispatchCommandBufferList;
    //TransferCommandBuffer<T> * transferCommandBufferList;


public:
    static CommandBufferManager* GetInstance();
    ~CommandBufferManager();
    void Init(T * apiInterface);
    void DeInit();
    void Update();
    DrawCommandBuffer<T> * CreateDrawCommandBuffer(CommandBufferLevel* type, const uint32_t & poolId);
    void DestroyCommandBuffer(uint32_t commandBufferId);
    void DestroyDrawCommandBuffer(DrawCommandBuffer<T> * obj);
    void ResetCommandBuffer(uint32_t commandBufferId);
    void ResetDrawCommandBuffer(DrawCommandBuffer<T> * obj);
    void BeginDrawCommandBufferRecording(DrawCommandBuffer<T> * obj, CommandBufferUsage * usage, CommandBufferInheritanceInfo * inheritanceInfo);
    void EndDrawCommandBufferRecording(DrawCommandBuffer<T> * obj);
    uint32_t CreateCommandPool(PipelineType * poolType, CommandPoolProperty * poolProp);
    void DestroyCommandPool(uint32_t commandPoolId);

    DrawCommandBuffer<T> * GetCommandBuffer(uint32_t id);

};

#include "RenderingWrapper.h"
#include "DrawCommandBuffer.h"
#include <CorePrecompiled.h>

template<typename T>
inline void CommandBufferManager<T>::Init(T * apiInterface)
{
    PLOGD << "Command Buffer Manager Init";
    drawCommandBufferList = new DrawCommandBuffer<T>[MAX_COMMANDBUFFERS];
    //DispatchCommandBufferList; = new DispatchCommandBuffer<T>[MAX_COMMANDBUFFERS];
    //transferCommandBuffer = new TransferCommandBuffer<T>;
    this->apiInterface = apiInterface;
}

template<typename T>
inline void CommandBufferManager<T>::DeInit()
{
    PLOGD << "Command Buffer Manager DeInit";
    delete[] drawCommandBufferList;
}

template<typename T>
inline void CommandBufferManager<T>::Update()
{
}

template<typename T>
inline DrawCommandBuffer<T> * CommandBufferManager<T>::CreateDrawCommandBuffer(CommandBufferLevel* level, const uint32_t & poolId)
{
    //create draw command buffer
    ASSERT_MSG_DEBUG(drawCB_Counter <= MAX_COMMANDBUFFERS, "Command buffers exhausted");
    
    drawCommandBufferList[drawCB_Counter].Init(apiInterface, level, poolId);

    return &drawCommandBufferList[drawCB_Counter++];
}

template<typename T>
inline void CommandBufferManager<T>::DestroyCommandBuffer(uint32_t commandBufferId)
{

    //drawCommandBufferList[drawCB_Counter].Init(apiInterface, level);
}

template<typename T>
inline void CommandBufferManager<T>::DestroyDrawCommandBuffer(DrawCommandBuffer<T>* obj)
{
    obj->DeInit();
}

template<typename T>
inline void CommandBufferManager<T>::ResetCommandBuffer(uint32_t commandBufferId)
{
    DrawCommandBuffer<T> * buf = GetCommandBuffer(commandBufferId);
    apiInterface->ResetCommandBuffer(commandBufferId, buf->poolId);
}

template<typename T>
inline void CommandBufferManager<T>::ResetDrawCommandBuffer(DrawCommandBuffer<T>* obj)
{
    apiInterface->ResetCommandBuffer(obj->GetId(), obj->GetPoolId());
}

template<typename T>
inline void CommandBufferManager<T>::BeginDrawCommandBufferRecording(DrawCommandBuffer<T>* obj, CommandBufferUsage * usage, CommandBufferInheritanceInfo * inheritanceInfo)
{
    apiInterface->BeginCommandBufferRecording(obj->GetId(), usage, inheritanceInfo);
}

template<typename T>
inline void CommandBufferManager<T>::EndDrawCommandBufferRecording(DrawCommandBuffer<T>* obj)
{
    apiInterface->EndCommandBufferRecording(obj->GetId());
}

template<typename T>
inline uint32_t CommandBufferManager<T>::CreateCommandPool(PipelineType * poolType, CommandPoolProperty * poolProp)
{
    return apiInterface->CreateCommandPool(poolType, poolProp);
}

template<typename T>
inline void CommandBufferManager<T>::DestroyCommandPool(uint32_t commandPoolId)
{
    apiInterface->DestroyCommandPool(commandPoolId);
}

template<typename T>
inline DrawCommandBuffer<T>* CommandBufferManager<T>::GetCommandBuffer(uint32_t id)
{
    std::vector<DrawCommandBuffer<T>*>::iterator it;
    it = std::find_if(drawCommandBufferList.begin(), drawCommandBufferList.end(), [&](DrawCommandBuffer<T> * e) { return e->id == id; });

    ASSERT_MSG_DEBUG(it != drawCommandBufferList.end(), "Pool id not found");

    return it;
}

template<typename T>
inline CommandBufferManager<T>::~CommandBufferManager()
{
}

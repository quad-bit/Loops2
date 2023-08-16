#pragma once

#include "DrawGraphNode.h"
#include "CorePrecompiled.h"
#include "Settings.h"
#include "Graph.h"

#if (RENDERING_API == VULKAN)
#include "VulkanInterface.h"
typedef VulkanInterface ApiInterface;
#elif (RENDERING_API == DX)
#include "DxInterface.h"
typedef DxInterface ApiInterface
#endif

#include "ForwardDrawGraph.h"
#include "DeferredDrawGraph.h"

enum class RendererType
{
    Forward,
    Deferred
};

template <typename T>
class DrawCommandBuffer;

class DrawGraphManager
{
private:
    DrawGraphManager() {}
    DrawGraphManager(DrawGraphManager const &) {}
    DrawGraphManager const & operator= (DrawGraphManager const &) {}
    static DrawGraphManager* instance;
    
    ApiInterface * apiInterface;
    RendererType rendererType;
    
    ForwardGraph<ApiInterface> * fwdGraph = nullptr;
    DeferredGraph<ApiInterface> * dfrdGraph = nullptr;

    int maxDrawNodes = 90;

    std::vector<DrawGraphNode *> drawGraphNodeList;
    Graph<DrawGraphNode> * drawGraph;
    
public:
    void Init(RendererType rendererType);
    void DeInit();
    
    template <typename T>
    void Update(DrawCommandBuffer<T> * dcb);

    template <typename T>
    void Update(DrawCommandBuffer<T> * dcb, const RenderPassTag & passTag);
    
    static DrawGraphManager* GetInstance();
    ~DrawGraphManager();

    void AddNode(GraphNode<DrawGraphNode> * node);
    void CreateGraphEdges(GraphNode<DrawGraphNode> * src, GraphNode<DrawGraphNode> * dest);
};

template<typename T>
inline void DrawGraphManager::Update(DrawCommandBuffer<T>* dcb)
{
    if (rendererType == RendererType::Forward)
    {
        fwdGraph->Update(dcb, RenderPassTag::ColorPass);
    }
    else if (rendererType == RendererType::Deferred)
    {
        dfrdGraph->Update(dcb);
    }
}

template<typename T>
inline void DrawGraphManager::Update(DrawCommandBuffer<T>* dcb, const RenderPassTag & passTag)
{
    if (rendererType == RendererType::Forward)
    {
        fwdGraph->Update(dcb, passTag);
    }
    else if (rendererType == RendererType::Deferred)
    {
        dfrdGraph->Update(dcb);
    }
}

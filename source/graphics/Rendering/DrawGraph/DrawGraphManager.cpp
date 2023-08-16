#include "DrawGraphManager.h"

DrawGraphManager * DrawGraphManager::instance = nullptr;

void DrawGraphManager::Init(RendererType rendererType)
{
    drawGraph = new Graph<DrawGraphNode>(maxDrawNodes);

    this->rendererType = rendererType;
    if (rendererType == RendererType::Forward)
    {
        fwdGraph = new ForwardGraph<ApiInterface>();
        fwdGraph->Init(drawGraph);
    }
    else if (rendererType == RendererType::Deferred)
    {
        dfrdGraph = new DeferredGraph<ApiInterface>();
        dfrdGraph->Init(drawGraph);
    }
}

void DrawGraphManager::DeInit()
{
    switch (rendererType)
    {
    case RendererType::Forward:
        fwdGraph->DeInit();
        delete fwdGraph;

    case RendererType::Deferred:
        dfrdGraph->DeInit();
        delete dfrdGraph;
    }
}

DrawGraphManager * DrawGraphManager::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new DrawGraphManager();
    }
    return instance;
}

DrawGraphManager::~DrawGraphManager()
{
}

void DrawGraphManager::AddNode(GraphNode<DrawGraphNode> * node)
{
    switch (rendererType)
    {
    case RendererType::Forward:
        fwdGraph->AddNode(node);
        break;
    case RendererType::Deferred:
        dfrdGraph->AddNode(node);
        break;
    }
}

void DrawGraphManager::CreateGraphEdges(GraphNode<DrawGraphNode> * src, GraphNode<DrawGraphNode> * dest)
{
    drawGraph->AttachDirectedEdge(src, dest);
}

#pragma once

template <typename T>
class DeferredGraph
{
private:
    Graph<DrawGraphNode> * drawGraph;

public:
    void Init(Graph<DrawGraphNode> * drawGraph);
    void DeInit();
    void Update(DrawCommandBuffer<T> * dcb);
    ~DeferredGraph();

    void AddNode(GraphNode<DrawGraphNode> * node);
};

template<typename T>
inline void DeferredGraph<T>::Init(Graph<DrawGraphNode>* drawGraph)
{
    this->drawGraph = drawGraph;
}

template<typename T>
inline void DeferredGraph<T>::DeInit()
{
}

template<typename T>
inline void DeferredGraph<T>::Update(DrawCommandBuffer<T>* dcb)
{
}

template<typename T>
inline DeferredGraph<T>::~DeferredGraph()
{
}

template<typename T>
inline void DeferredGraph<T>::AddNode(GraphNode<DrawGraphNode> * node)
{
}

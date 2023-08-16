#pragma once
#include "Settings.h"
#include "RenderingWrapper.h"
#include <CorePrecompiled.h>
#include <vector>

#if (RENDERING_API == VULKAN)
#include "VulkanInterface.h"
typedef VulkanInterface ApiInterface;
#elif (RENDERING_API == DX)
#include "DxInterface.h"
typedef DxInterface ApiInterface
#endif

template <typename T>
class DrawCommandBuffer;

enum class DrawNodeTypes
{
    PIPELINE, // cmdBindPipeline
    CAMERA,
    LIGHT,
    LIGHT_GROUP,
    TEXTURE,
    SAMPLER,
    MATERIAL_SURFACE,
    MESH,
    TRANSFORM,
    DRAWING,
    NONE
};

namespace DrawGraphUtil
{
    extern uint32_t pipelineLayoutId;
    extern std::vector<uint32_t> descriptorIdList;
    extern uint32_t setOffset;
}

class DrawGraphNode
{
protected:

public:
    DrawNodeTypes drawNodeType;
    RenderPassTag tag = RenderPassTag::None;
    uint32_t setLevel;
    std::vector<SetWrapper*> setWrapperList;// pipeline node can have multiple sets wrappers
    std::vector<uint32_t> meshList;
    uint32_t numMeshes;
    virtual void Entry() = 0;
    virtual void Exit();
    
    static DrawCommandBuffer<ApiInterface> * dcb;
};

class TransformNode : public DrawGraphNode
{
public:
    std::vector<uint32_t> descriptorSetIds;
    virtual void Entry() override;
    //virtual void Exit() override;
    TransformNode()
    {
        numMeshes = 1;
        drawNodeType = DrawNodeTypes::TRANSFORM;
    }
};


class IndexedDrawNode : public DrawGraphNode
{
public:
    IndexedDrawInfo info;

    virtual void Entry() override;
    virtual void Exit() override;
    IndexedDrawNode()
    {
        numMeshes = 1;
        drawNodeType = DrawNodeTypes::DRAWING;
    }
};

class DrawArrayDrawNode : public DrawGraphNode
{
public:
    DrawArrayInfo info;

    virtual void Entry() override;
    virtual void Exit() override;
    DrawArrayDrawNode()
    {
        numMeshes = 1;
        drawNodeType = DrawNodeTypes::DRAWING;
    }
};

class MeshNode : public DrawGraphNode
{
public:
    std::vector<uint32_t> bufferIds;
    std::vector<size_t> pOffsets;
    uint32_t indexBufferId;
    bool isIndexed;
    virtual void Entry() override;
    virtual void Exit() override;
    MeshNode()
    {
        numMeshes = 1; //  for now vertex buffer and index buffer are not getting shared
                       // between multiple objects
        drawNodeType = DrawNodeTypes::MESH;
    }
};


class LightDrawNode : public DrawGraphNode
{
public:
    std::vector<uint32_t> descriptorIds;
    virtual void Entry() override;
    //virtual void Exit() override;
    LightDrawNode()
    {
        drawNodeType = DrawNodeTypes::LIGHT;
    }
};


class CameraDrawNode : public DrawGraphNode
{
public:
    std::vector<uint32_t> descriptorIds;
    virtual void Entry() override;
    //virtual void Exit() override;
    CameraDrawNode()
    {
        drawNodeType = DrawNodeTypes::CAMERA;
    }
};



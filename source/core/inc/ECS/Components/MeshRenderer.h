#pragma once

#include "ECS/Component.h"
#include "ECS/ECS_Helper.h"

class Mesh;
class Material;
class Transform;

class MeshRenderer : public Component<MeshRenderer>
{
public:
    Mesh * geometry;
    Material * material;
    Transform * transform;
    bool castShadows = true;
    bool receiveShadows = true;

    COMPONENT_TYPE componentType{COMPONENT_TYPE::MESH_RENDERER};

    MeshRenderer(Mesh * geo, Material * mat, Transform * tran)
    {
        geometry = geo;
        material = mat;
        transform = tran;
    }
};
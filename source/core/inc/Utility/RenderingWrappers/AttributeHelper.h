#pragma once

#include <glm\glm.hpp>
#include <map>
#include <vector>
#include <Cube.h>
#include <stdint.h>
#include "RenderingWrapper.h"

class Mesh;

enum class ATTRIBUTES
{
    POSITION = 0,
    COLOR = 1,
    NORMAL = 2,
    TANGENT = 3,
    UV = 4,
    UV1 = 5,
    UV2 = 6,
    UV3 = 7,
    UV4 = 8,
    NUM_ATTRIBUTES = 9
};

struct VertexMetaData
{
    uint32_t vertexDataStride, indexDataStride, attribCount;
    VertexInputAttributeInfo * attribInfoList;
};

struct PC
{
    glm::vec3 position;
    glm::vec4 color;
};

struct PCN
{
    glm::vec3 position;
    glm::vec4 color;
    glm::vec3 normal;
};


struct AttribStructBase
{
    void * vertexData;
    uint32_t vertexDataSize;
    VertexMetaData metaData;
    uint32_t vertexCount;

    void * indexData;
    uint32_t indexDataSize;
    uint32_t indexCount;

    AttribStructBase() {};
};


struct AttribPC : public AttribStructBase
{
    std::vector<PC> posColList;
    std::vector<uint32_t> indicies;
    AttribPC()
    {
        metaData.vertexDataStride = sizeof(PC);
        metaData.attribCount = 2;
        metaData.attribInfoList = new VertexInputAttributeInfo[metaData.attribCount];
        
        // position
        {
            VertexInputAttributeInfo info = {};
            info.binding = 0; // As a single vertex buffer is used per mesh
            info.format = Format::R32G32B32_SFLOAT; //vec3 position
            info.location = 0;
            info.offset = (uint32_t)offsetof(PC, PC::position);
            metaData.attribInfoList[0] = info;
        }

        //color
        {
            VertexInputAttributeInfo info = {};
            info.binding = 0; // As a single vertex buffer is used per mesh
            info.format = Format::R32G32B32A32_SFLOAT; //uvec4 color
            info.location = 1;
            info.offset = (uint32_t)offsetof(PC, PC::color);
            metaData.attribInfoList[1] = info;
        }
        metaData.indexDataStride = sizeof(uint32_t);
    }

    ~AttribPC()
    {
    }

    template<typename T>
    void FillData(Mesh * mesh);

    template<typename T>
    void FillData(Mesh * mesh, const glm::vec4 & color);
};

struct AttribPCN : public AttribStructBase
{
    std::vector<PCN> posColList;
    std::vector<uint32_t> indicies;
    AttribPCN()
    {
        metaData.vertexDataStride = sizeof(PCN);
        metaData.attribCount = 3;
        metaData.attribInfoList = new VertexInputAttributeInfo[metaData.attribCount];

        // position
        {
            VertexInputAttributeInfo info = {};
            info.binding = 0; // As a single vertex buffer is used per mesh
            info.format = Format::R32G32B32_SFLOAT; //vec3 position
            info.location = 0;
            info.offset = (uint32_t)offsetof(PCN, PCN::position);
            metaData.attribInfoList[0] = info;
        }

        //color
        {
            VertexInputAttributeInfo info = {};
            info.binding = 0; // As a single vertex buffer is used per mesh
            info.format = Format::R32G32B32A32_SFLOAT; //uvec4 color
            info.location = 1;
            info.offset = (uint32_t)offsetof(PCN, PCN::color);
            metaData.attribInfoList[1] = info;
        }

        //Normal
        {
            VertexInputAttributeInfo info = {};
            info.binding = 0; // As a single vertex buffer is used per mesh
            info.format = Format::R32G32B32_SFLOAT;  //uvec4 color
            info.location = 2;
            info.offset = (uint32_t)offsetof(PCN, PCN::normal);
            metaData.attribInfoList[2] = info;
        }

        indexCount = 0;
        metaData.indexDataStride = sizeof(uint32_t);
    }

    ~AttribPCN()
    {
        
    }

    template<typename T>
    void FillData(Mesh * mesh);

    template<typename T>
    void FillData(Mesh * mesh, const glm::vec4 & color);
};


#include <Mesh.h>

template<typename T>
inline void AttribPC::FillData(Mesh * mesh)
{
    T obj;

    size_t numVertices = obj.positions.size();
    posColList.resize(numVertices);

    mesh->positions.resize(numVertices);
    mesh->colors.resize(numVertices);
    
    for (uint32_t i = 0; i < numVertices; i++)
    {
        posColList[i].position = obj.positions[i];
        posColList[i].color = obj.color;
        
        mesh->positions[i] = &posColList[i].position;
        mesh->colors[i] = &posColList[i].color;
    }

    vertexData = posColList.data();
    vertexDataSize = (uint32_t)posColList.size() * sizeof(PC);
    vertexCount = (uint32_t)numVertices;

    size_t numIndicies = obj.indices.size();

    indicies.resize(numIndicies);
    mesh->indicies.resize(numIndicies);

    for (uint32_t i = 0; i < numIndicies; i++)
    {
        indicies[i] = obj.indices[i];
        mesh->indicies[i] = &indicies[i];
    }

    indexData = indicies.data();
    indexDataSize = (uint32_t)indicies.size() * sizeof( uint32_t );
    indexCount = (uint32_t)numIndicies;
}

template<typename T>
inline void AttribPC::FillData(Mesh * mesh, const glm::vec4 & color)
{
    T obj;

    size_t numVertices = obj.positions.size();
    posColList.resize(numVertices);

    mesh->positions.resize(numVertices);
    mesh->colors.resize(numVertices);

    for (uint32_t i = 0; i < numVertices; i++)
    {
        posColList[i].position = obj.positions[i];
        posColList[i].color = color;

        mesh->positions[i] = &posColList[i].position;
        mesh->colors[i] = &posColList[i].color;
    }

    vertexData = posColList.data();
    vertexDataSize = (uint32_t)posColList.size() * sizeof(PC);
    vertexCount = (uint32_t)numVertices;

    size_t numIndicies = obj.indices.size();

    indicies.resize(numIndicies);
    mesh->indicies.resize(numIndicies);

    for (uint32_t i = 0; i < numIndicies; i++)
    {
        indicies[i] = obj.indices[i];
        mesh->indicies[i] = &indicies[i];
    }

    indexData = indicies.data();
    indexDataSize = (uint32_t)indicies.size() * sizeof(uint32_t);
    indexCount = (uint32_t)numIndicies;
}

template<typename T>
inline void AttribPCN::FillData(Mesh * mesh)
{
    T obj;

    size_t numVertices = obj.indices.size();
    posColList.resize(numVertices);
    
    mesh->positions.resize(numVertices);
    mesh->colors.resize(numVertices);

    // position and color
    for (int i = 0; i < numVertices; i++)
    {
        posColList[i].position = obj.positions[obj.indices[i]];
        posColList[i].color = glm::vec4(0.6, 0.2, 0.5, 1.0);

        mesh->positions[i] = &posColList[i].position;
        mesh->colors[i] = &posColList[i].color;
    }
    
    // normals
    mesh->normals.resize(numVertices);
    for (int i = 0; i < numVertices; i++)
    {
        posColList[i].normal = obj.normals[obj.indices[i/6]];
        mesh->normals[i] = &posColList[i].normal;
    }

    vertexData = posColList.data();
    vertexDataSize = (uint32_t)posColList.size() * sizeof(PCN);
    vertexCount = (uint32_t)numVertices;

    // not going to use indicies as normals need to be present for each vertex, 
    // indexing won't allow it

    size_t numIndicies = 0;
    indicies.resize(numIndicies);
    mesh->indicies.resize(numIndicies);
}

template<typename T>
inline void AttribPCN::FillData(Mesh * mesh, const glm::vec4 & color)
{
    T obj;

    size_t numVertices = obj.indices.size();
    posColList.resize(numVertices);

    mesh->positions.resize(numVertices);
    mesh->colors.resize(numVertices);

    // position and color
    for (int i = 0; i < numVertices; i++)
    {
        posColList[i].position = obj.positions[obj.indices[i]];
        posColList[i].color = color;

        mesh->positions[i] = &posColList[i].position;
        mesh->colors[i] = &posColList[i].color;
    }

    // normals
    mesh->normals.resize(numVertices);
    for (int i = 0; i < numVertices; i++)
    {
        posColList[i].normal = obj.normals[obj.indices[i / 6]];
        mesh->normals[i] = &posColList[i].normal;
    }

    vertexData = posColList.data();
    vertexDataSize = (uint32_t)posColList.size() * sizeof(PCN);
    vertexCount = (uint32_t)numVertices;

    // not going to use indicies as normals need to be present for each vertex, 
    // indexing won't allow it

    size_t numIndicies = 0;
    indicies.resize(numIndicies);
    mesh->indicies.resize(numIndicies);
}

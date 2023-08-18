#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace Core
{
    namespace Math
    {
        struct CubeIndexed
        {
            glm::vec4 color = { 0.2, 0.5, 0.6, 0.8 };
            std::vector< glm::vec3 > positions =
            {
                glm::vec3(-1, -1, -1),
                glm::vec3(1, -1, -1),
                glm::vec3(1, 1, -1),
                glm::vec3(-1, 1, -1),
                glm::vec3(-1, -1, 1),
                glm::vec3(1, -1, 1),
                glm::vec3(1, 1, 1),
                glm::vec3(-1, 1, 1)
            };

            std::vector< glm::vec4 > colors =
            {
                glm::vec4(0.2f, 0.99f, 0.2f, 1.0f),
                glm::vec4(0.2f, 0.99f, 0.2f, 1.0f),
                glm::vec4(0.2f, 0.99f, 0.2f, 1.0f),
                glm::vec4(0.2f, 0.99f, 0.2f, 1.0f),
                glm::vec4(0.7f, 0.4f, 0.7f, 1.0f),
                glm::vec4(0.7f, 0.4f, 0.7f, 1.0f),
                glm::vec4(0.9f, 0.2f, 0.5f, 1.0f),
                glm::vec4(0.95f, 0.1f, 0.89f, 1.0f)
            };

            std::vector< glm::uvec2 > uv =
            {
                glm::uvec2(0, 0),
                glm::uvec2(1, 0),
                glm::uvec2(1, 1),
                glm::uvec2(0, 1)
            };

            std::vector<glm::vec3> normals =
            {
                glm::vec3(0, 0, 1),
                glm::vec3(1, 0, 0),
                glm::vec3(0, 0, -1),
                glm::vec3(-1, 0, 0),
                glm::vec3(0, 1, 0),
                glm::vec3(0, -1, 0)
            };

            std::vector<uint32_t> indices =
            {
                0, 1, 3, 3, 1, 2,
                1, 5, 2, 2, 5, 6,
                5, 4, 6, 6, 4, 7,
                4, 0, 7, 7, 0, 3,
                3, 2, 7, 7, 2, 6,
                4, 5, 0, 0, 5, 1
            };
        };
    }
}

/*

Vector3f vertices[8] =
{
Vector3f(-1, -1, -1),
Vector3f(1, -1, -1),
Vector3f(1, 1, -1),
Vector3f(-1, 1, -1),
Vector3f(-1, -1, 1),
Vector3f(1, -1, 1),
Vector3f(1, 1, 1),
Vector3f(-1, 1, 1)
};

Vector2f texCoords[4] =
{
Vector2f(0, 0),
Vector2f(1, 0),
Vector2f(1, 1),
Vector2f(0, 1)
};


Vector3f normals[6] =
{
Vector3f(0, 0, 1),
Vector3f(1, 0, 0),
Vector3f(0, 0, -1),
Vector3f(-1, 0, 0),
Vector3f(0, 1, 0),
Vector3f(0, -1, 0)
};

int indices[6 * 6] =
{
0, 1, 3, 3, 1, 2,
1, 5, 2, 2, 5, 6,
5, 4, 6, 6, 4, 7,
4, 0, 7, 7, 0, 3,
3, 2, 7, 7, 2, 6,
4, 5, 0, 0, 5, 1
};

int texInds[6] = { 0, 1, 3, 3, 1, 2 };

float vertexBuffer[18 * 6];
for (int i = 0; i < 36; i++) {
vertexBuffer[i * 3 + 0] = vertices[indices[i]].x;
vertexBuffer[i * 3 + 1] = vertices[indices[i]].y;
vertexBuffer[i * 3 + 2] = vertices[indices[i]].z;
}

float textureBuffer[12 * 6];
for (int i = 0; i < 36; i++) {
textureBuffer[i * 2 + 0] = texCoords[texInds[i % 4]].x;
textureBuffer[i * 2 + 1] = texCoords[texInds[i % 4]].y;
}

float normalBuffer[18 * 6];
for (int i = 0; i < 36; i++) {
normalBuffer[i * 3 + 0] = normals[indices[i / 6]].x;
normalBuffer[i * 3 + 1] = normals[indices[i / 6]].y;
normalBuffer[i * 3 + 2] = normals[indices[i / 6]].z;
}

*/
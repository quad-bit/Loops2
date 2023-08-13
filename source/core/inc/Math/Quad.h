#pragma once
#include <glm\glm.hpp>
#include <vector>

struct QuadIndexed
{
    glm::vec4 color = { 0.2, 0.5, 0.6, 0.8 };
    std::vector< glm::vec3 > positions =
    {
        glm::vec3(-1, -1, -1),
        glm::vec3(1, -1, -1),
        glm::vec3(1, 1, -1),
        glm::vec3(-1, 1, -1)
    };

    std::vector< glm::vec4 > colors =
    {
        glm::vec4(0.2f, 0.99f, 0.2f, 1.0f),
        glm::vec4(0.2f, 0.99f, 0.2f, 1.0f),
        glm::vec4(0.2f, 0.99f, 0.2f, 1.0f),
        glm::vec4(0.2f, 0.99f, 0.2f, 1.0f)
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
        glm::vec3(0, 0, -1),
    };

    std::vector<uint32_t> indices=
    {
        0, 1, 3, 3, 1, 2
    };

};

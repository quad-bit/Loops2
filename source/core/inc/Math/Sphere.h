#pragma once
#include <Math/MathUtil.h>

//https://www.songho.ca/opengl/gl_sphere.html

namespace Core
{
    namespace Math
    {
        uint32_t sectorCount = 36;
        uint32_t stackCount = 16;
        float PI = 3.14f;

        struct Sphere
        {
            std::vector<glm::vec3> m_vertices;
            std::vector<glm::vec3> m_normals;
            std::vector<glm::vec2> m_texCoords;
            std::vector<uint32_t> m_indices;
            std::vector<uint32_t> m_lineIndices;

            explicit Sphere(float radius)
            {
                float x, y, z, xy;                              // vertex position
                float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
                float s, t;                                     // vertex texCoord

                float sectorStep = 2 * PI / sectorCount;
                float stackStep = PI / stackCount;
                float sectorAngle, stackAngle;

                for (int i = 0; i <= stackCount; ++i)
                {
                    stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
                    xy = radius * cosf(stackAngle);             // r * cos(u)
                    z = radius * sinf(stackAngle);              // r * sin(u)

                    // add (sectorCount+1) vertices per stack
                    // first and last vertices have same position and normal, but different tex coords
                    for (int j = 0; j <= sectorCount; ++j)
                    {
                        sectorAngle = j * sectorStep;           // starting from 0 to 2pi

                        // vertex position (x, y, z)
                        x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
                        y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
                        m_vertices.push_back(glm::vec3(x, y, z));

                        // normalized vertex normal (nx, ny, nz)
                        nx = x * lengthInv;
                        ny = y * lengthInv;
                        nz = z * lengthInv;
                        m_normals.push_back(glm::vec3(nx, ny, nz));

                        // vertex tex coord (s, t) range between [0, 1]
                        s = (float)j / sectorCount;
                        t = (float)i / stackCount;
                        m_texCoords.push_back(glm::vec2(s, t));
                    }
                }

                int k1, k2;
                for (int i = 0; i < stackCount; ++i)
                {
                    k1 = i * (sectorCount + 1);     // beginning of current stack
                    k2 = k1 + sectorCount + 1;      // beginning of next stack

                    for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
                    {
                        // 2 triangles per sector excluding first and last stacks
                        // k1 => k2 => k1+1
                        if (i != 0)
                        {
                            m_indices.push_back(k1);
                            m_indices.push_back(k2);
                            m_indices.push_back(k1 + 1);
                        }

                        // k1+1 => k2 => k2+1
                        if (i != (stackCount - 1))
                        {
                            m_indices.push_back(k1 + 1);
                            m_indices.push_back(k2);
                            m_indices.push_back(k2 + 1);
                        }

                        // store indices for lines
                        // vertical lines for all stacks, k1 => k2
                        m_lineIndices.push_back(k1);
                        m_lineIndices.push_back(k2);
                        if (i != 0)  // horizontal lines except 1st stack, k1 => k+1
                        {
                            m_lineIndices.push_back(k1);
                            m_lineIndices.push_back(k1 + 1);
                        }
                    }
                }
            }
        };
    }
}

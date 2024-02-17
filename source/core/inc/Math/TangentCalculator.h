#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

// https://web.archive.org/web/20110708081637/http://www.terathon.com/code/tangent.html

namespace Core
{
    namespace Math
    {
        struct Triangle
        {
            unsigned short index[3];
        };

        void CalculateTangentArray(long vertexCount, const glm::vec3* vertex, const glm::vec3* normal,
            const glm::vec2* texcoord, long triangleCount, const Triangle* triangle, glm::vec4* tangent)
        {
            glm::vec3* tan1 = new glm::vec3[vertexCount * 2];
            glm::vec3* tan2 = tan1 + vertexCount;
            //ZeroMemory(tan1, vertexCount * sizeof(glm::vec3) * 2);

            for (long a = 0; a < triangleCount; a++)
            {
                long i1 = triangle->index[0];
                long i2 = triangle->index[1];
                long i3 = triangle->index[2];

                const glm::vec3& v1 = vertex[i1];
                const glm::vec3& v2 = vertex[i2];
                const glm::vec3& v3 = vertex[i3];

                const glm::vec2& w1 = texcoord[i1];
                const glm::vec2& w2 = texcoord[i2];
                const glm::vec2& w3 = texcoord[i3];

                float x1 = v2.x - v1.x;
                float x2 = v3.x - v1.x;
                float y1 = v2.y - v1.y;
                float y2 = v3.y - v1.y;
                float z1 = v2.z - v1.z;
                float z2 = v3.z - v1.z;

                float s1 = w2.x - w1.x;
                float s2 = w3.x - w1.x;
                float t1 = w2.y - w1.y;
                float t2 = w3.y - w1.y;

                float r = 1.0F / (s1 * t2 - s2 * t1);
                glm::vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
                    (t2 * z1 - t1 * z2) * r);
                glm::vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
                    (s1 * z2 - s2 * z1) * r);

                tan1[i1] += sdir;
                tan1[i2] += sdir;
                tan1[i3] += sdir;

                tan2[i1] += tdir;
                tan2[i2] += tdir;
                tan2[i3] += tdir;

                triangle++;
            }

            for (long a = 0; a < vertexCount; a++)
            {
                const glm::vec3& n = normal[a];
                const glm::vec3& t = tan1[a];

                // Gram-Schmidt orthogonalize
                auto temp = glm::normalize(t - n * glm::dot(n, t));
                tangent[a] = glm::vec4(temp, 0.0f);

                // Calculate handedness
                tangent[a].w = (glm::dot(glm::cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
            }

            delete[] tan1;
        }

        void CalculateTangentArray(long vertexCount, const glm::vec3* vertex, const glm::vec3* normal,
            const glm::vec2* texcoord, unsigned int* indicies, uint32_t indexCount, glm::vec4* tangent)
        {
            glm::vec3* tan1 = new glm::vec3[vertexCount * 2];
            glm::vec3* tan2 = tan1 + vertexCount;
            //ZeroMemory(tan1, vertexCount * sizeof(glm::vec3) * 2);

            auto triangleCount = indexCount / 3;
            for (long a = 0; a < triangleCount; a++)
            {
                long i1 = indicies[a * 3];
                long i2 = indicies[a * 3 + 1];
                long i3 = indicies[a * 3 + 2];

                const glm::vec3& v1 = vertex[i1];
                const glm::vec3& v2 = vertex[i2];
                const glm::vec3& v3 = vertex[i3];

                const glm::vec2& w1 = texcoord[i1];
                const glm::vec2& w2 = texcoord[i2];
                const glm::vec2& w3 = texcoord[i3];

                float x1 = v2.x - v1.x;
                float x2 = v3.x - v1.x;
                float y1 = v2.y - v1.y;
                float y2 = v3.y - v1.y;
                float z1 = v2.z - v1.z;
                float z2 = v3.z - v1.z;

                float s1 = w2.x - w1.x;
                float s2 = w3.x - w1.x;
                float t1 = w2.y - w1.y;
                float t2 = w3.y - w1.y;

                float r = 1.0F / (s1 * t2 - s2 * t1);
                glm::vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
                    (t2 * z1 - t1 * z2) * r);
                glm::vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
                    (s1 * z2 - s2 * z1) * r);

                tan1[i1] += sdir;
                tan1[i2] += sdir;
                tan1[i3] += sdir;

                tan2[i1] += tdir;
                tan2[i2] += tdir;
                tan2[i3] += tdir;
            }

            for (long a = 0; a < vertexCount; a++)
            {
                const glm::vec3& n = normal[a];
                const glm::vec3& t = tan1[a];

                // Gram-Schmidt orthogonalize
                auto temp = glm::normalize(t - n * glm::dot(n, t));
                tangent[a] = glm::vec4(temp, 0.0f);

                // Calculate handedness
                tangent[a].w = (glm::dot(glm::cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
            }

            delete[] tan1;
        }
    }
}
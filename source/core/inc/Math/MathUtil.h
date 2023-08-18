#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace Core
{
    namespace Math
    {

#define Vec4ToVec3(a) glm::vec3(a.x, a.y, a.z)
#define Vec3ToVec4_1(a) glm::vec4(a.x, a.y, a.z, 1.0f)
#define Vec3ToVec4_0(a) glm::vec4(a.x, a.y, a.z, 0.0f)

#define Vec3ToFloat3(a, b) \
    b[0] = a.x; \
    b[1] = a.y; \
    b[2] = a.z; \

        float lerp(float x, float y, float t);

        template <typename T>
        T RangeConversion(T val, T newMin, T newMax)
        {
            return val * (newMax - newMin) - newMax;
        }
    }
}
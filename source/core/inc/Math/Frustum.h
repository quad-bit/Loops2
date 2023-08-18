#pragma once

#include "Utility/Container/Vector.h"
#include "Math/Plane.h"
#include <glm\glm.hpp>

namespace Core
{
    namespace Math
    {
        class Frustum
        {
        private:
            Core::Containers::Vector<Plane> frustum;

        public:
            Frustum();
            ~Frustum();

            void CalculateFrustum(float angle, float ratio, float near,
                float far, glm::vec3& camPos, glm::vec3& lookAtPos, glm::vec3& up);

            void AddPlane(Plane& plane);
            bool IsPointVisible(glm::vec3 point);
            bool IsSphereVisible(glm::vec3 center, float rad);
            bool IsBoxVisible(glm::vec3 min, glm::vec3 max);
            int GetTotalPlanes();
        };
    }
}
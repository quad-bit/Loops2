#pragma once

#include "Utility/Container/Vector.h"
#include "Math/Plane.h"
#include <glm\glm.hpp>


class Frustum
{
private:
    Vector<Plane> frustum;

public:
    Frustum();
    ~Frustum();

    void CalculateFrustum(float angle, float ratio, float near,
    					    float far, vec3 &camPos, vec3 & lookAtPos, vec3 & up);

    void AddPlane(Plane& plane);
    bool IsPointVisible(vec3 point);
    bool IsSphereVisible(vec3 center, float rad);
    bool IsBoxVisible(vec3 min, vec3 max);
    int GetTotalPlanes();
};

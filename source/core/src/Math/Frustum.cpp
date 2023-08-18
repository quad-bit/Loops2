#include "Math/Frustum.h"


Core::Math::Frustum::Frustum()
{
}

Core::Math::Frustum::~Frustum()
{
}

void Core::Math::Frustum::CalculateFrustum(float angle, float ratio, float near, float far, glm::vec3 & camPos, glm::vec3 & lookAtPos, glm::vec3 & up)
{
    glm::vec3 xVec, yVec, zVec, vecN, vecF;
    glm::vec3 nearTopLeft, nearTopRight, nearBottomLeft, nearBottomRight;
    glm::vec3 farTopLeft, farTopRight, farBottomRight, farBottomLeft;
    float radians = (float)glm::tan(glm::degrees(angle) * 0.5f);
    float nearH = near * radians;
    float nearW = nearH * ratio;
    float farH = far * radians;
    float farW = farH * ratio;

    zVec = camPos - lookAtPos;
    zVec = glm::normalize(zVec);

    xVec = glm::cross(up, zVec);
    xVec = glm::normalize(xVec);

    yVec = glm::cross(zVec, xVec);

    vecN = camPos - zVec * near;
    vecF = camPos - zVec * far;

    nearTopLeft = vecN + yVec * nearH - xVec * nearW;
    nearTopRight = vecN + yVec * nearH + xVec * nearW;
    nearBottomLeft = vecN - yVec * nearH - xVec * nearW;
    nearTopRight = vecN - yVec * nearH + xVec * nearW;

    farTopLeft = vecF + yVec * farH - xVec * farW;
    farTopRight = vecF + yVec * farH + xVec * farW;
    farBottomLeft = vecF - yVec * farH - xVec * farW;
    farTopRight = vecF - yVec * farH + xVec * farW;

    frustum.Clear();

    Plane planeObj;

    planeObj.Create(nearTopRight, nearTopLeft, farTopLeft);
    AddPlane(planeObj);

    planeObj.Create(nearBottomLeft, nearBottomRight, farBottomRight);
    AddPlane(planeObj);

    planeObj.Create(nearTopLeft, nearBottomLeft, farBottomLeft);
    AddPlane(planeObj);

    planeObj.Create(nearBottomRight, nearTopRight, farBottomRight);
    AddPlane(planeObj);

    planeObj.Create(nearTopLeft, nearTopRight, nearBottomRight);
    AddPlane(planeObj);

    planeObj.Create(farTopRight, farTopLeft, farBottomLeft);
    AddPlane(planeObj);
}

void Core::Math::Frustum::AddPlane(Plane & plane)
{
    frustum.Push(plane);
}

bool Core::Math::Frustum::IsPointVisible(glm::vec3 point)
{
    for (int i = 0; i < frustum.GetSize(); i++)
    {
        if (frustum[i].GetDistance(point) < 0)
        {
            return false;
        }
    }
    return true;
}

bool Core::Math::Frustum::IsSphereVisible(glm::vec3 center, float rad)
{
    float distance = 0;

    for (int i = 0; i < frustum.GetSize(); i++)
    {
        distance = frustum[i].GetDistance(center);
        if (distance < -rad)
            return false;
    }
    return true;
}

bool Core::Math::Frustum::IsBoxVisible(glm::vec3 min, glm::vec3 max)
{
    if (IsPointVisible(min)) return true;
    if (IsPointVisible(glm::vec3(max.x, min.y, min.z))) return true;
    if (IsPointVisible(glm::vec3(min.x, max.y, min.z))) return true;
    if (IsPointVisible(glm::vec3(max.x, max.y, min.z))) return true;
    if (IsPointVisible(glm::vec3(min.x, min.y, max.z))) return true;
    if (IsPointVisible(glm::vec3(max.x, min.y, max.z))) return true;
    if (IsPointVisible(glm::vec3(min.x, max.y, max.z))) return true;
    if (IsPointVisible(max)) return true;

    return false;
}

int Core::Math::Frustum::GetTotalPlanes()
{
    return frustum.GetSize();
}

#include "Math/BoundingBox.h"


Core::Math::BoundingBox::BoundingBox()
{
}

Core::Math::BoundingBox::BoundingBox(const Core::Math::BoundingBox & AABB)
{
    min = AABB.min;
    max = AABB.max;
    center = AABB.center;
}

Core::Math::BoundingBox::~BoundingBox()
{
}

void Core::Math::BoundingBox::CalculateBB(glm::vec3 * vertices, int numVertices)
{
    if (vertices == NULL)
    {
        return;
    }

    for (int i = 0; i < numVertices; i++)
    {
        if (vertices[i].x < min.x)    min.x = vertices[i].x;
        if (vertices[i].x > max.x)    max.x = vertices[i].x;

        if (vertices[i].y < min.y)    min.y = vertices[i].y;
        if (vertices[i].y > max.y)    max.y = vertices[i].y;

        if (vertices[i].z < min.z)    min.z = vertices[i].z;
        if (vertices[i].z > max.z)    max.z = vertices[i].z;
    }

    center.x = (min.x + max.x) / 2.0f;
    center.y = (min.y + max.y) / 2.0f;
    center.z = (min.z + max.z) / 2.0f;
}

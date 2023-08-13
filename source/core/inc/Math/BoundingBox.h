#pragma once

#include <glm\glm.hpp>

//using namespace glm;

class BoundingBox
{
private:
    glm::vec3 min, max, center;

public:
    BoundingBox();
    BoundingBox(const BoundingBox& AABB);
    virtual ~BoundingBox();

    void CalculateBB(glm::vec3 * vertices, int numVertices);
};

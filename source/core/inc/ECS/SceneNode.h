#pragma once
#include <vector>
#include <glm\glm.hpp>
#include <string>
enum class NODE_TYPE
{
    TRANSFORM,
    //GEOMETRY,
    //MATERIAL,
    NONE
};

namespace SceneTreeUtil
{
    extern std::vector<glm::mat4> matrixList;
    extern std::vector<std::string> nameList;
}

class SceneNode
{
public:
    NODE_TYPE nodeType;
    //SceneNode * parentNode;

    //virtual void Execute() = 0;
    virtual void Entry() = 0;
    virtual void Exit() {}

    SceneNode(const NODE_TYPE & type) : nodeType(type) {}
    SceneNode() { nodeType = NODE_TYPE::NONE; }
};

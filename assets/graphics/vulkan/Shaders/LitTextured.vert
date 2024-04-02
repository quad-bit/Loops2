#version 450

// std430 + UBO 
#extension GL_EXT_scalar_block_layout : enable 
// for including headers
#extension GL_GOOGLE_include_directive : enable

/*
layout (std140, set = 0, binding = 0) uniform View
{
    mat4 view;
    mat4 projection;
    vec3 cameraPos;
} view;
*/
layout (std430, set = 0, binding = 0) uniform Scene
{
    mat4 view;
    mat4 projection;
    vec3 cameraPos;
    float near;
    float far;
    uint screenWidth;
    uint screenHeight;
} scene;

layout(std140, set = 4, binding = 0) uniform Transform
{
    mat4 model;
}transform;

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inTexCoord;
layout (location = 4) in vec4 inTangent;

struct OutVertex
{
    vec4 color;
    vec2 fragTexCoord;
    vec3 normal;
    vec4 tangent;
    vec3 viewSpacePos;
    vec4 worldSpacePos;
};
layout (location = 0) out OutVertex outVertex;

struct OutScene
{
    vec4 cameraPos;
    // x = near, y= far, z=screenHeight, w=screenWidth
    vec4 pack;
    mat4 inverseProjection;
};
layout (location = 6) out OutScene outScene;

void main()
{
    mat4 clip = mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.5f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    mat3 normalMat = mat3(transpose(inverse(transform.model))); // needs to be calculated on cpu side, normal Mat
    vec4 viewPos = scene.view * transform.model * vec4(pos.xyz, 1.0);

    outVertex.color = inColor;
    gl_Position = scene.projection * scene.view * transform.model * vec4(pos.xyz, 1.0);
    outVertex.fragTexCoord = inTexCoord;
    outVertex.normal = normalMat * inNormal;
    outVertex.tangent = inTangent;
    outVertex.worldSpacePos = transform.model * vec4(pos.xyz, 1.0);
    outVertex.viewSpacePos = viewPos.xyz;

    outScene.cameraPos = vec4(scene.cameraPos.xyz, viewPos.z);
    outScene.inverseProjection = inverse(scene.projection);
    outScene.pack.x = scene.near;
    outScene.pack.y = scene.far;
    outScene.pack.z = scene.screenHeight;
    outScene.pack.w = scene.screenWidth;
}

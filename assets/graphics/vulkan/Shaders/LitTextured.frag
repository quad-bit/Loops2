#version 450 
//#extension GL_ARB_separate_shader_objects : enable 

// std430 + UBO 
#extension GL_EXT_scalar_block_layout : enable 
// for including headers
#extension GL_GOOGLE_include_directive : enable

#include "LightCullSettings.h"

struct InVertex
{
    vec4 color;
    vec2 fragTexCoord;
    vec3 normal;
    vec3 cameraPos;
    vec4 tangent;
    vec4 worldSpacePos;
};

layout (location = 0) in InVertex inVertex;

struct InScene
{
    // x = near, y= far, z=screenHeight, w=screenWidth
    vec4 pack;
    mat4 inverseProjection;
};
layout (location = 6) in flat InScene scene;

layout (location = 0) out vec4 outColor;

struct PointLight
{
    vec3 lightPos;
    float lightRadius;
    vec4 lightColor;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

const int NUM_LIGHTS = 5;

layout(set = 2, binding = 0) uniform lightUniform
{
    PointLight pointLights[NUM_LIGHTS];
};

struct Cluster
{
    vec4 minPos;
    vec4 maxPos;
    uint lightIds[MAX_LIGHTS_PER_CLUSTER];
    uint numLights;
};

layout(std430, set = 2, binding = 1) uniform ClusterInfo
{
    Cluster cluster[CLUSTER_X * CLUSTER_Y * CLUSTER_Z];
}clusterInfo;

layout(set = 3, binding = 0) uniform sampler2D diffuseSampler;
layout(set = 3, binding = 1) uniform sampler2D normalSampler;

struct Surface
{
    vec3 specular; // will be replaced by specular map
    float shininess;
}surface;

vec4 clipToView(vec4 clip)
{
    //View space transform
    vec4 viewSpacePos = (scene.inverseProjection) * clip;

    //Perspective projection
    viewSpacePos = viewSpacePos / viewSpacePos.w;
    return viewSpacePos;
}

vec4 ScreenToView(vec4 screen)
{
    //Convert to NDC
    vec2 texCoord = vec2(screen.x, screen.y) / vec2(scene.pack.w, scene.pack.z);

    //Convert to clipSpace.. Vulkan has its origin at top left corner
    vec4 clip = vec4(vec2(texCoord.x, 1.0 - texCoord.y)* 2.0 - 1.0, -1.0f, screen.w);
    //vec4 clip = vec4(vec2(texCoord.x, texCoord.y)* 2.0 - 1.0, screen.z, screen.w);

    return clipToView(clip);
}

int FindCluster(vec4 viewSpacePos)
{
    // top left corner of cluster 0
    vec3 topLeftClusterPos = vec3(clusterInfo.cluster[0].minPos.x, clusterInfo.cluster[0].maxPos.y, clusterInfo.cluster[0].minPos.z);

    uint topRightIndex = CLUSTER_X - 1;
    // top right corner of CLUSTER_X - 1'th cluster
    vec3 topRightClusterPos = vec3(clusterInfo.cluster[topRightIndex].maxPos.x, clusterInfo.cluster[topRightIndex].maxPos.y, clusterInfo.cluster[topRightIndex].minPos.z);

    uint bottomLeftIndex = CLUSTER_X * (CLUSTER_Y - 1);
    vec3 bottomLeftClusterPos = vec3(clusterInfo.cluster[bottomLeftIndex].minPos.x, clusterInfo.cluster[bottomLeftIndex].minPos.y, clusterInfo.cluster[bottomLeftIndex].minPos.z);

    float clusterWidth = abs(topRightClusterPos.x - topLeftClusterPos.x)/CLUSTER_X;
    float clusterHeight = abs(bottomLeftClusterPos.y - topLeftClusterPos.y)/CLUSTER_Y;

    int clusterXIndex = 0, clusterYIndex = 0;
    // you cant divide by cluster width as the range is from -a to +a and not from 0 to a
    if(viewSpacePos.x < 0)
    {
        clusterXIndex = int(viewSpacePos.x/clusterWidth) + int(floor(CLUSTER_X/2)) - 1;
    }
    else
    {
        clusterXIndex = int(viewSpacePos.x/clusterWidth) + int(ceil(CLUSTER_X/2));
    }

    // the range for y starts from + to - unlike above x's
    if(viewSpacePos.y < 0)
    {
        clusterYIndex = -int(viewSpacePos.y/clusterHeight) + int(floor(CLUSTER_Y/2));
    }
    else
    {
        clusterYIndex = int(viewSpacePos.y/clusterHeight) + int(ceil(CLUSTER_Y/2)) - 1;
    }

    int clusterZIndex = 0;

    int clusterIndex = clusterZIndex * CLUSTER_X * CLUSTER_Y +
          clusterYIndex * CLUSTER_X + 
          clusterXIndex;

    return clusterIndex;
}

void main() 
{
    surface.specular = vec3(1.0);
    surface.shininess = 32;

    uint num = clusterInfo.cluster[0].numLights;
    vec4 color = texture(diffuseSampler, inVertex.fragTexCoord) * inVertex.color;

    vec3 viewDir = normalize(inVertex.cameraPos - inVertex.worldSpacePos.xyz);

    vec3 N = normalize(inVertex.normal);
    vec3 T = normalize(inVertex.tangent.xyz);
    vec3 B = cross(inVertex.normal, inVertex.tangent.xyz) * inVertex.tangent.w;
    mat3 TBN = mat3(T, B, N);
    N = TBN * normalize(texture(normalSampler, inVertex.fragTexCoord).xyz * 2.0 - vec3(1.0));

    vec3 V = normalize(viewDir);

    vec3 diffuse = vec3(0.0);
    vec3 ambient = vec3(0.0);
    vec3 specular = vec3(0.0);

    vec4 viewPos = ScreenToView(vec4(gl_FragCoord.xyz, 1.0));
    uint clusterIndex = FindCluster(viewPos);

    for(int i = 0; i < NUM_LIGHTS; i++)
    {
        vec3 lightAmbient = pointLights[i].ambient.xyz;
        vec3 lightDiffuse = pointLights[i].diffuse.xyz; 
        vec3 lightSpecular = pointLights[i].specular.xyz; 
        vec3 lightPos = pointLights[i].lightPos.xyz; 

        vec3 lightDir = normalize(lightPos.xyz - inVertex.worldSpacePos.xyz);
        vec3 L = normalize(lightDir);
        vec3 R = reflect(-L, N);

        float distanceVal = distance(lightPos.xyz, inVertex.worldSpacePos.xyz);
        bool testRadius = true;
        if(testRadius)
        {
            if(distanceVal < pointLights[i].lightRadius)
            {
                diffuse += lightDiffuse * max(dot(N, L), 0.0);// * 1.0f/(distanceVal);
                specular += pow(max(dot(R, V), 0.0), surface.shininess) * lightSpecular * surface.specular;// * 1.0f/(distanceVal);
            }
        }
        else
        {
            diffuse += lightDiffuse * max(dot(N, L), 0.0);
            specular += pow(max(dot(R, V), 0.0), surface.shininess) * lightSpecular * surface.specular;
        }
        ambient += lightAmbient;
    }

    outColor = vec4((diffuse + specular + ambient) * color.rgb, color.a);
}



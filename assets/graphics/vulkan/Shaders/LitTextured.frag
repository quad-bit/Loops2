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
    vec4 tangent;
    vec3 viewSpacePos;
    vec4 worldSpacePos;
};

layout (location = 0) in InVertex inVertex;

struct InScene
{
    vec4 cameraPos;
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

layout(std430, set = 2, binding = 1) readonly buffer ClusterInfo
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
    //vec4 clip = vec4(vec2(texCoord.x, 1.0 - texCoord.y)* 2.0 - 1.0, -1.0f, screen.w);
    vec4 clip = vec4(vec2(texCoord.x, 1.0 - texCoord.y)* 2.0 - 1.0, screen.z, screen.w);

    return clipToView(clip);
}

uint FindZTile(float screenZ)
{
    // -1 to 1
    float depthNDC = 2.0 * screenZ - 1.0;
    float zNear = scene.pack.x;
    float zFar = scene.pack.y;
    float linearDepth = -2.0 * zNear * zFar / (zFar + zNear - depthNDC * (zFar - zNear));
    float actualDepth = scene.cameraPos.w;

    uint tile = uint(CLUSTER_Z * log(abs(linearDepth/zNear))/log(zFar/zNear));
    return tile;
}

int FindCluster(vec4 viewSpacePos, float screenZ)
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
    if(CLUSTER_X > 1)
    {
        if(viewSpacePos.x < 0)
        {
            clusterXIndex = int(viewSpacePos.x/clusterWidth) + int(floor(CLUSTER_X/2)) - 1;
        }
        else
        {
            clusterXIndex = int(viewSpacePos.x/clusterWidth) + int(ceil(CLUSTER_X/2));
        }
    }

    // the range for y starts from + to - unlike above x's
    if(CLUSTER_Y > 1)
    {
        if(viewSpacePos.y < 0)
        {
            clusterYIndex = -int(viewSpacePos.y/clusterHeight) + int(floor(CLUSTER_Y/2));
        }
        else
        {
            clusterYIndex = int(viewSpacePos.y/clusterHeight) + int(ceil(CLUSTER_Y/2)) - 1;
        }
    }
    //int clusterZIndex = int(FindZTile(screenZ));
    float zNear = scene.pack.x;
    float zFar = scene.pack.y;
    int clusterZIndex = int(CLUSTER_Z * log(abs(viewSpacePos.z/zNear))/log(zFar/zNear));

    int clusterIndex = clusterZIndex * CLUSTER_X * CLUSTER_Y +
          clusterYIndex * CLUSTER_X + 
          clusterXIndex;

    return clusterIndex;
}

int FindCluster2(vec4 viewSpacePos)
{
    float zNear = scene.pack.x;
    float zFar = scene.pack.y;
    int clusterZIndex = int(CLUSTER_Z * log(abs(viewSpacePos.z/zNear))/log(zFar/zNear));

    // top left corner of cluster the first cluster in the z slice
    uint topLeftClusterIndex = clusterZIndex * CLUSTER_X * CLUSTER_Y;
    vec3 topLeftClusterPos = vec3(clusterInfo.cluster[topLeftClusterIndex].minPos.x,
     clusterInfo.cluster[topLeftClusterIndex].maxPos.y, clusterInfo.cluster[topLeftClusterIndex].minPos.z);

    uint topRightIndex = topLeftClusterIndex + CLUSTER_X - 1;
    // top right corner of CLUSTER_X - 1'th cluster
    vec3 topRightClusterPos = vec3(clusterInfo.cluster[topRightIndex].maxPos.x, clusterInfo.cluster[topRightIndex].maxPos.y, clusterInfo.cluster[topRightIndex].minPos.z);

    uint bottomLeftIndex = topLeftClusterIndex + CLUSTER_X * (CLUSTER_Y - 1);
    vec3 bottomLeftClusterPos = vec3(clusterInfo.cluster[bottomLeftIndex].minPos.x, clusterInfo.cluster[bottomLeftIndex].minPos.y, clusterInfo.cluster[bottomLeftIndex].minPos.z);

    float clusterWidth = abs(topRightClusterPos.x - topLeftClusterPos.x)/CLUSTER_X;
    float clusterHeight = abs(bottomLeftClusterPos.y - topLeftClusterPos.y)/CLUSTER_Y;

    int clusterXIndex = 0, clusterYIndex = 0;
    // you cant divide by cluster width as the range is from -a to +a and not from 0 to a
    if(CLUSTER_X > 1)
    {
        if(viewSpacePos.x < 0)
        {
            clusterXIndex = int(viewSpacePos.x/clusterWidth) + int(floor(CLUSTER_X/2)) - 1;
        }
        else
        {
            clusterXIndex = int(viewSpacePos.x/clusterWidth) + int(ceil(CLUSTER_X/2));
        }
    }

    // the range for y starts from + to - unlike above x's
    if(CLUSTER_Y > 1)
    {
        if(viewSpacePos.y < 0)
        {
            clusterYIndex = -int(viewSpacePos.y/clusterHeight) + int(floor(CLUSTER_Y/2));
        }
        else
        {
            clusterYIndex = int(viewSpacePos.y/clusterHeight) + int(ceil(CLUSTER_Y/2)) - 1;
        }
    }

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

    vec3 viewDir = normalize(scene.cameraPos.xyz - inVertex.worldSpacePos.xyz);

    vec3 N = normalize(inVertex.normal);
    vec3 T = normalize(inVertex.tangent.xyz);
    vec3 B = cross(inVertex.normal, inVertex.tangent.xyz) * inVertex.tangent.w;
    mat3 TBN = mat3(T, B, N);
    N = TBN * normalize(texture(normalSampler, inVertex.fragTexCoord).xyz * 2.0 - vec3(1.0));

    vec3 V = normalize(viewDir);

    vec3 diffuse = vec3(0.0);
    vec3 ambient = vec3(0.3f);
    vec3 specular = vec3(0.0);

    vec3 fragCoord = gl_FragCoord.xyz;
    vec4 viewPos = ScreenToView(vec4(fragCoord.xyz, 1.0));
    uint clusterIndex = FindCluster2(viewPos);
    //vec4 viewPos = vec4(inVertex.viewSpacePos.xyz, 1.0);
    //uint clusterIndex = FindCluster(viewPos, viewPos.z);

    for(int j = 0; j < clusterInfo.cluster[clusterIndex].numLights; j++)
    {
        uint lightIndex = clusterInfo.cluster[clusterIndex].lightIds[j];
        //vec3 lightAmbient = pointLights[lightIndex].ambient.xyz;
        vec3 lightDiffuse = pointLights[lightIndex].diffuse.xyz; 
        vec3 lightSpecular = pointLights[lightIndex].specular.xyz; 
        vec3 lightPos = pointLights[lightIndex].lightPos.xyz; 

        vec3 lightDir = normalize(lightPos.xyz - inVertex.worldSpacePos.xyz);
        vec3 L = normalize(lightDir);
        vec3 R = reflect(-L, N);

        float distanceVal = distance(lightPos.xyz, inVertex.worldSpacePos.xyz);
        bool testRadius = true;
        if(testRadius)
        {
            if(distanceVal < pointLights[lightIndex].lightRadius)
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
        //ambient += lightAmbient;
    }

    outColor = vec4((diffuse + specular + ambient) * color.rgb, color.a);
}



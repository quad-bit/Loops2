
#include "windowing/WindowManager.h"
#include <Platform/Assertion.h>


#if defined(GLFW_ENABLED)

// a simple glfw logo
const char* const logo[] =
{
    "................",
    "..00............",
    "..00.000.000....",
    "..00.0.0.0.0....",
    "..00.000.000....",
    "..00............",
    "..00000000000...",
    "..00000000000...",
    "................",
    "..00000.00000...",
    "..00..0.00......",
    "..00000.00000...",
    "..00.......00...",
    "..00000000000...",
    "..00000000000...",
    "................"
};

const unsigned char icon_colors[5][4] =
{
    { 0,   0,   0, 255 }, // black
    { 255,   0,   0, 255 }, // red
    { 0, 255,   0, 255 }, // green
    { 0,   0, 255, 255 }, // blue
    { 255, 255, 255, 255 }  // white
};

static int cur_icon_color = 0;

static void set_icon(GLFWwindow* window, int icon_color)
{
    int x, y;
    unsigned char pixels[16 * 16 * 4];
    unsigned char* target = pixels;
    GLFWimage img = { 16, 16, pixels };

    for (y = 0; y < img.width; y++)
    {
        for (x = 0; x < img.height; x++)
        {
            if (logo[y][x] == '0')
                memcpy(target, icon_colors[icon_color], 4);
            else
                memset(target, 0, 4);

            target += 4;
        }
    }

    glfwSetWindowIcon(window, 1, &img);
}


void Renderer::Windowing::WindowManager::InitOSWindow()
{
    glfwInit();
    if (glfwVulkanSupported() == GLFW_FALSE) 
    {
        ASSERT_MSG_DEBUG(0, " GLFW Failed to initialize with Vulkan.");
        std::exit(-1);
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindow = glfwCreateWindow(m_windowSettings.m_windowWidth, m_windowSettings.m_windowHeight, m_windowSettings.m_windowName.c_str(), nullptr, nullptr);
    //glfwGetFramebufferSize(glfwWindow, (int*)&m_windo, (int*)&Settings::windowHeight);
    set_icon(glfwWindow, cur_icon_color);
}

void Renderer::Windowing::WindowManager::DeInitOSWindow()
{
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}

void Renderer::Windowing::WindowManager::UpdateOSWindow()
{
    glfwPollEvents();

    if (glfwWindowShouldClose(glfwWindow)) 
    {
        Close();
    }
}

#endif 
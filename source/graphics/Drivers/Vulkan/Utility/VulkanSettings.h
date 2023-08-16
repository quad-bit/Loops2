#pragma once
#include <Settings.h>


#if (RENDERING_API == VULKAN)
        
    #define BUILD_ENABLE_VULKAN_DEBUG                 1
    #define BUILD_ENABLE_VULKAN_RUNTIME_DEBUG         1
        
    #if defined( GLFW_ENABLED )

        // Define as a build option
        #define USE_FRAMEWORK_GLFW 1

        #define GLFW_INCLUDE_VULKAN
        #include <GLFW/glfw3.h>
        // For Windows Message Box
        /*#if defined( _WIN32 )
        #undef APIENTRY
        #include <windows.h>
        #endif*/

    // WINDOWS
    #elif (CURRENT_OS == WINDOWS_OS)
        // this is always defined on windows platform

        #define PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_WIN32_SURFACE_EXTENSION_NAME
        #define VK_USE_PLATFORM_WIN32_KHR 1

        #include <windows.h>

    // LINUX ( Via XCB library )
    #elif (OS == LINUX_OS)
        // there are other ways to create windows on linux,
        // requirements might change based on the available libraries and distro used
        // for example: xlib, wayland and mir
        // xcb seems like a popular and well supported option on X11, until wayland and mir take over

        #define VK_USE_PLATFORM_XCB_KHR 1
        #include <xcb/xcb.h>

    #else
        // platform not yet supported
    #error Platform not yet supported
    #endif

#endif

include("${CMAKE_SOURCE_DIR}/cmake/Util.cmake")
GROUP_SOURCE_FILES(${CMAKE_CURRENT_SOURCE_DIR})

set(WINDOWING_FILES
inc/windowing/InputEvents.h
inc/windowing/InputManager.h
inc/windowing/MouseInputManager.h
inc/windowing/WindowManager.h

src/windowing/InputManager.cpp
src/windowing/InputManagerGLFW.cpp
src/windowing/MouseInputManager.cpp
src/windowing/Window_glfw.cpp
src/windowing/WindowManager.cpp
)

set(RENDER_GRAPH_FILES
inc/renderGraph/Effect.h
inc/renderGraph/Pipeline.h
inc/renderGraph/RenderPass.h
inc/renderGraph/Technique.h
inc/renderGraph/TechniqueNode.h

# src/windowing/InputManager.cpp
# src/windowing/InputManagerGLFW.cpp
# src/windowing/MouseInputManager.cpp
# src/windowing/Window_glfw.cpp
# src/windowing/WindowManager.cpp
)

set(SHADER_RESOURCE_FILES
inc/shaderResource/UniformFactory.h
src/shaderResource/UniformFactory.cpp
)

set(GRAPHIC_FILES
# inc/DxInterface.h
inc/GraphicsManager.h
inc/RenderingManager.h
inc/RenderingPrecompiled.h
inc/VulkanInterface.h
# inc/ForwardInterface.h
# inc/DeferredInterface.h

# src/DxInterface.cpp
src/GraphicsManager.cpp
src/RenderingManager.cpp
src/RenderingPrecompiled.cpp
src/VulkanInterface.cpp
)


#create library
set(TARGET_NAME "renderer")
add_library(${TARGET_NAME} 
    ${WINDOWING_FILES} 
    ${GRAPHIC_FILES} 
    # ${RENDER_GRAPH_FILES}
    ${SHADER_RESOURCE_FILES}
)
add_library(Loops::${TARGET_NAME} ALIAS ${TARGET_NAME})

# if (MSVC)
#     # For precompiled header.
#     # Set
#     # "Precompiled Header" to "Use (/Yu)"
#     # "Precompiled Header File" to "stdafx.h"
#     set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /YuRenderingPrecompiled.h /FIRenderingPrecompiled.h")
# endif()

# set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/Rendering/RenderingPrecompiled.cpp"
#     PROPERTIES
#     COMPILE_FLAGS "/YcRenderingPrecompiled.h"
# )

target_link_libraries(${TARGET_NAME} PRIVATE glfw Loops::core Loops::gfxVulkan)

target_include_directories(${TARGET_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:inc>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

IF( APPLE )
    MESSAGE( STATUS "Running under MacOS X" )
    # Watch out, for this check also is TRUE under MacOS X because it
    # falls under the category of Unix-like.
ELSEIF( UNIX )
    MESSAGE( STATUS "Running under Unix or a Unix-like OS" )
    # Despite what you might think given this name, the variable is also
    # true for 64bit versions of Windows.
ELSEIF( WIN32 )
    MESSAGE( STATUS "Running under Windows (either 32bit or 64bit), setting character set to unicode" )
    add_definitions(-DUNICODE -D_UNICODE)
ENDIF()

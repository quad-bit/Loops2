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
inc/renderGraph/Task.h
inc/renderGraph/RenderGraphManager.h
inc/renderGraph/utility/Utils.h
inc/renderGraph/Graph.h

inc/renderGraph/effects/Opaque.h
inc/renderGraph/effects/SkyboxPass.h
inc/renderGraph/effects/DepthPass.h
inc/renderGraph/effects/BoundPass.h
inc/renderGraph/effects/LightCullPass.h

inc/renderGraph/pipelines/DepthViewPipeline.h
inc/renderGraph/pipelines/LowEndPipeline.h

inc/renderGraph/techniques/LambertianLit.h
inc/renderGraph/techniques/OpaqueUnlit.h
inc/renderGraph/techniques/OpaqueTexturedUnlit.h
inc/renderGraph/techniques/OpaqueTexturedLit.h
inc/renderGraph/techniques/DepthTechnique.h
inc/renderGraph/techniques/Skybox.h
inc/renderGraph/techniques/BoundRenderer.h
inc/renderGraph/techniques/LightCull.h

inc/renderGraph/tasks/RenderTask.h
inc/renderGraph/tasks/ComputeTask.h
inc/renderGraph/tasks/TransferTask.h
inc/renderGraph/tasks/DownloadTask.h
inc/renderGraph/tasks/PresentationTask.h

src/renderGraph/RenderGraphManager.cpp
src/renderGraph/Pipeline.cpp
src/renderGraph/Technique.cpp
src/renderGraph/utility/Utils.cpp

src/renderGraph/effects/Opaque.cpp
src/renderGraph/effects/DepthPass.cpp
src/renderGraph/effects/SkyboxPass.cpp
src/renderGraph/effects/BoundPass.cpp
src/renderGraph/effects/LightCullPass.cpp


src/renderGraph/pipelines/DepthViewPipeline.cpp
src/renderGraph/pipelines/LowEndPipeline.cpp

src/renderGraph/techniques/LambertianLit.cpp
src/renderGraph/techniques/DepthTechnique.cpp
src/renderGraph/techniques/OpaqueUnlit.cpp
src/renderGraph/techniques/OpaqueTexturedUnlit.cpp
src/renderGraph/techniques/OpaqueTexturedLit.cpp
src/renderGraph/techniques/Skybox.cpp
src/renderGraph/techniques/BoundRenderer.cpp
src/renderGraph/techniques/LightCull.cpp

src/renderGraph/tasks/RenderTask.cpp
src/renderGraph/tasks/ComputeTask.cpp
src/renderGraph/tasks/TransferTask.cpp
src/renderGraph/tasks/DownloadTask.cpp
)

set(SHADER_RESOURCE_FILES
inc/resourceManagement/UniformFactory.h
inc/resourceManagement/MaterialFactory.h
inc/resourceManagement/MeshFactory.h
inc/resourceManagement/BoundFactory.h
inc/resourceManagement/Resource.h

src/resourceManagement/UniformFactory.cpp
src/resourceManagement/MaterialFactory.cpp
src/resourceManagement/MeshFactory.cpp
src/resourceManagement/BoundFactory.cpp
src/resourceManagement/Resource.cpp
)

set(GRAPHIC_FILES
# inc/DxInterface.h
inc/GraphicsManager.h
inc/RenderingManager.h
inc/RenderingPrecompiled.h
inc/VulkanInterface.h
inc/CommandReader.h
inc/RendererSettings.h
# inc/ForwardInterface.h
# inc/DeferredInterface.h

# src/DxInterface.cpp
src/GraphicsManager.cpp
src/RenderingManager.cpp
src/RenderingPrecompiled.cpp
src/VulkanInterface.cpp
src/CommandReader.cpp
)

#create library
set(TARGET_NAME "renderer")
add_library(${TARGET_NAME} 
    ${WINDOWING_FILES} 
    ${GRAPHIC_FILES} 
    ${RENDER_GRAPH_FILES}
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

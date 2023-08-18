include("${CMAKE_SOURCE_DIR}/cmake/Util.cmake")
GROUP_SOURCE_FILES(${CMAKE_CURRENT_SOURCE_DIR})

set(SourceList
    inc/EngineManager.h
    inc/SceneManager.h
    inc/ECS/ECS_Manager.h
    inc/ECS/Systems/CameraSystem.h
    inc/ECS/Systems/LightSystem.h
    inc/ECS/Systems/MeshRendererSystem.h
    inc/ECS/Systems/ScriptableSystem.h
    inc/ECS/Systems/TransformSystem.h
    src/EngineManager.cpp
    src/SceneManager.cpp
    src/ECS/ECS_Manager.cpp
    src/ECS/Systems/CameraSystem.cpp
    src/ECS/Systems/LightSystem.cpp
    src/ECS/Systems/MeshRendererSystem.cpp
    src/ECS/Systems/ScriptableSystem.cpp
    src/ECS/Systems/TransformSystem.cpp
)

#create library
set(TARGET_NAME "Engine")
add_library(${TARGET_NAME} ${SourceList})
add_library(Loops::${TARGET_NAME} ALIAS ${TARGET_NAME})

target_link_libraries(${TARGET_NAME} PRIVATE Core)
# target_link_libraries(EngineInterface PUBLIC Renderer)

target_include_directories(${TARGET_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:inc>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if (MSVC)
    # For precompiled header.
    # Set
    # "Precompiled Header" to "Use (/Yu)"
    # "Precompiled Header File" to "stdafx.h"

endif()

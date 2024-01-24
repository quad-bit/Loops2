include("${CMAKE_SOURCE_DIR}/cmake/Util.cmake")
GROUP_SOURCE_FILES(${CMAKE_CURRENT_SOURCE_DIR})

set(SourceList
    inc/EngineManager.h
    inc/ECS/SceneManager.h
    inc/ECS/SceneGraphManager.h
    inc/ECS/ECS_Manager.h
    inc/ECS/Systems/CameraSystem.h
    inc/ECS/Systems/LightSystem.h
    inc/ECS/Systems/MeshRendererSystem.h
    inc/ECS/Systems/ScriptableSystem.h
    inc/ECS/Systems/TransformSystem.h
    inc/SceneLoader.h

    src/EngineManager.cpp
    src/ECS/SceneManager.cpp
    src/ECS/SceneGraphManager.cpp
    src/ECS/ECS_Manager.cpp
    src/ECS/Systems/CameraSystem.cpp
    src/ECS/Systems/LightSystem.cpp
    src/ECS/Systems/MeshRendererSystem.cpp
    src/ECS/Systems/ScriptableSystem.cpp
    src/ECS/Systems/TransformSystem.cpp
    src/SceneLoader.cpp
)

#create library
set(TARGET_NAME "engine")
add_library(${TARGET_NAME} ${SourceList})
add_library(Loops::${TARGET_NAME} ALIAS ${TARGET_NAME})

target_link_libraries(${TARGET_NAME} PRIVATE Loops::core)
target_link_libraries(${TARGET_NAME} PRIVATE Loops::renderer)

target_include_directories(${TARGET_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:inc>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${TINYGLTF_INCLUDE_DIRS}
)

if (MSVC)
    # For precompiled header.
    # Set
    # "Precompiled Header" to "Use (/Yu)"
    # "Precompiled Header File" to "stdafx.h"

endif()

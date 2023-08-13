
include("${CMAKE_SOURCE_DIR}/cmake/Util.cmake")
GROUP_SOURCE_FILES(${CMAKE_CURRENT_SOURCE_DIR})

set(CORE_FILES
    inc/CoreManager.h
    inc/CorePrecompiled.h
    inc/PlatformSettings.h
    inc/RendererSettings.h
    inc/Settings.h

    src/CoreManager.cpp
    src/CorePrecompiled.cpp
    src/RendererSettings.cpp
    src/Settings.cpp
)

set(ECS_COMPONENT_FILES
    inc/ECS/Components/Camera.h
    inc/ECS/Components/Light.h
    inc/ECS/Components/Material.h
    inc/ECS/Components/Mesh.h
    inc/ECS/Components/MeshRenderer.h
    inc/ECS/Components/Scriptable.h
    inc/ECS/Components/Shader.h
    inc/ECS/Components/Texture.h
    inc/ECS/Components/Transform.h

    src/ECS/Components/Camera.cpp
    src/ECS/Components/SceneNode.cpp
    src/ECS/Components/Transform.cpp
)

set(ECS_EVENTS_FILES
    inc/ECS/Events/ComponentAdditionEvent.h
    inc/ECS/Events/Event.h
    inc/ECS/Events/EventBus.h
    inc/ECS/Events/MeshAdditionEvent.h
    inc/ECS/Events/SceneChangeEvent.h

    src/ECS/EventBus.cpp 
)

set(ECS_FILES
    inc/ECS/Component.h
    inc/ECS/ComponentHandle.h
    inc/ECS/ComponentManager.h
    inc/ECS/ComponentMask.h
    inc/ECS/ECS_Helper.h
    inc/ECS/ECS_Setting.h
    inc/ECS/Entity.h
    inc/ECS/EntityHandle.h
    inc/ECS/EntityManager.h
    inc/ECS/EntityMap.h
    inc/ECS/SceneNode.h
    inc/ECS/System.h
    inc/ECS/UnitTest_ECS.h
    inc/ECS/World.h

    src/ECS/Component.cpp
    src/ECS/ComponentMask.cpp
    src/ECS/ECS_Setting.cpp
    src/ECS/EntityManager.cpp
    src/ECS/EventBus.cpp
    src/ECS/System.cpp
    src/ECS/UnitTest_ECS.cpp
    src/ECS/World.cpp
)

set(IO_FILES
    inc/IO/FileReader.h
    inc/IO/ReadMe.txt

    src/IO/FileReader.cpp
)

set(MATH_FILES
    inc/Math/BoundingBox.h
    inc/Math/Cube.h
    inc/Math/Frustum.h
    inc/Math/MathUtil.h
    inc/Math/Plane.h
    inc/Math/Quad.h
    inc/Math/ReadMe.txt

    src/Math/BoundingBox.cpp
    src/Math/Frustum.cpp
    src/Math/MathUtil.cpp
    src/Math/Plane.cpp
)

set(PLATFORM_FILES
    inc/Platform/Assertion.h
    inc/Platform/PlatformManager.h
    inc/Platform/ReadMe.txt

    src/Platform/Assertion.cpp
    src/Platform/PlatformManager.cpp
)

set(UTILITY_FILES
    inc/Utility/HashHelper.h
    inc/Utility/HashManager.h
    inc/Utility/PipelineStateWrappers.h
    inc/Utility/ReadMe.txt
    inc/Utility/ResourceAllocationHelper.h
    inc/Utility/ShaderResourceDescription.h
    inc/Utility/Timer.h
    inc/Utility/Container/BinaryTree.h
    inc/Utility/Container/BitArray.h
    inc/Utility/Container/DoublyLinkedList.h
    inc/Utility/Container/Graph.h
    inc/Utility/Container/LinkedList.h
    inc/Utility/Container/Queue.h
    inc/Utility/Container/Stack.h
    inc/Utility/Container/Tree.h
    inc/Utility/Container/Util.h
    inc/Utility/Container/Vector.h
    inc/Utility/RenderingWrappers/AttributeHelper.h
    inc/Utility/RenderingWrappers/RenderingWrapper.h

    src/Utility/HashManager.cpp
    src/Utility/PipelineStateWrappers.cpp
    src/Utility/ResourceAllocationHelper.cpp
    src/Utility/Timer.cpp
    src/Utility/Container/BitArray.cpp
)

find_package(glm REQUIRED)
IF( glm_FOUND )
    message (" found glm : ${glm_INCLUDE_DIR} ")
ENDIF()

find_package(plog REQUIRED)
IF( plog_FOUND )
    message ("found plog : ${plog_INCLUDE_DIR}")
ENDIF()

#find glfw
find_package(glfw3 3.3)
IF( glfw3_FOUND )
    message (" found glfw ")
ENDIF()

#create library

set(TARGET_NAME "Core")
add_library(${TARGET_NAME} 
    ${CORE_FILES} 
    ${ECS_COMPONENT_FILES} 
    ${ECS_EVENTS_FILES} 
    ${ECS_FILES} 
    ${IO_FILES} 
    ${PLATFORM_FILES}
    ${UTILITY_FILES}
    ${MATH_FILES}
)
add_library(Loops::${TARGET_NAME} ALIAS ${TARGET_NAME})

target_link_libraries(${TARGET_NAME} PUBLIC glfw glm::glm plog)
# target_link_libraries(${TARGET_NAME} PRIVATE glm::glm)
# target_link_libraries(${TARGET_NAME} PRIVATE plog)


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
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /YuCorePrecompiled.h /FICorePrecompiled.h")
endif()

set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/src/CorePrecompiled.cpp"
    PROPERTIES
    COMPILE_FLAGS "/YcCorePrecompiled.h"
)

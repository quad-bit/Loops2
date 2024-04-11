include("${CMAKE_SOURCE_DIR}/cmake/Util.cmake")
GROUP_SOURCE_FILES(${CMAKE_CURRENT_SOURCE_DIR})

set(SourceList
inc/ApplicationController.h
inc/CameraController.h
inc/PlayerHandlerScript.h
inc/SceneManagerScript.h
src/ApplicationController.cpp
src/CameraController.cpp
src/main.cpp
src/PlayerHandlerScript.cpp
src/SceneManagerScript.cpp
)

#create library
set(TARGET_NAME "app_beautifulGame")
add_Executable(${TARGET_NAME} ${SourceList})

find_package(plog REQUIRED)

target_link_libraries(${TARGET_NAME} PRIVATE Loops::core)
# target_link_libraries(${TARGET_NAME} PRIVATE plog)
target_link_libraries(${TARGET_NAME} PUBLIC Loops::renderer)
target_link_libraries(${TARGET_NAME} PRIVATE Loops::engine)

target_include_directories(${TARGET_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:inc>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<BUILD_INTERFACE:${plog_INCLUDE_DIR}>
)

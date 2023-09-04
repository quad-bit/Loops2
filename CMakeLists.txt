cmake_minimum_required(VERSION 3.15)

include(${CMAKE_SOURCE_DIR}/cmake/vcpkg.cmake)

project (Loops)

message ("using c++ 20")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(EFFECTS_PATH "${CMAKE_SOURCE_DIR}/assets/graphics/effects")
set(RENDERING_API "VULKAN")

find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
find_package(plog REQUIRED)

message ("\nConfiguring Core ..\n")
add_subdirectory(source/core projects/core)

message ("\nConfiguring Vulkan Driver ..\n")
add_subdirectory(source/graphicsApi/vulkan projects/vulkan)

message ("\nGraphics ..\n")
add_subdirectory(Source/Graphics Projects/Graphics)

message ("\nConfiguring EngineInterface ..\n")
add_subdirectory(source/engine projects/engine)

message ("\nApp ..")
add_subdirectory(source/application projects/application)

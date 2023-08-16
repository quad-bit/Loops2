
##  file list creation Begin
#
# MACRO(SUBDIRLIST result curdir)
#   FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
#   SET(dirlist "")
#   FOREACH(child ${children})
#     IF(IS_DIRECTORY ${curdir}/${child})
#       LIST(APPEND dirlist ${child})
#     ENDIF()
#   ENDFOREACH()
#   SET(${result} ${dirlist})
# ENDMACRO()
#
# SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

include("${CMAKE_CURRENT_SOURCE_DIR}/../../../../Util.cmake")

HEADER_DIRECTORIES(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

Set ( SourceList "")

FOREACH(subdir ${SUBDIRS})
    #message(${subdir})

    file(GLOB fileListInDir
        "${subdir}/*.h"
        "${subdir}/*.cpp"
        "${subdir}/*.txt"
    )
    LIST(APPEND SourceList ${fileListInDir})

    source_group(${subdir} FILES ${fileListInDir})

ENDFOREACH()

message("\n")

#get all the source files
file(GLOB filesInParentDir
    "*.h"
    "*.cpp"
    "*.txt"
)

list(LENGTH filesInParentDir size)
IF(NOT ${size} EQUAL 0)
    LIST(APPEND SourceList ${filesInParentDir})
    source_group("" FILES ${filesInParentDir})
    LIST(APPEND SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})
ENDIF()

#adding data files
Set (VulkanDataPath "${CMAKE_SOURCE_DIR}/Assets/Graphics/Vulkan")

SUBDIRLIST(DATASUBDIRS ${VulkanDataPath})

Set ( DataList "")
FOREACH(subdir ${DATASUBDIRS})
    #message (${subdir})
    file(GLOB DataList
        "${VulkanDataPath}/${subdir}/*.frag"
        "${VulkanDataPath}/${subdir}/*.vert"
        "${VulkanDataPath}/${subdir}/*.spv"
        "${VulkanDataPath}/${subdir}/*.refl"
    )
    LIST(APPEND SourceList ${DataList})

    source_group(${subdir} FILES ${DataList})

ENDFOREACH()

##  file list creation ends

SET (PROJECT_NAME "Vulkan")

#find VULKAN
find_package(Vulkan REQUIRED)


#find glm
IF( DEFINED ENV{GLM_PATH} )
  SET( glm_DIR "$ENV{GLM_PATH}" )
ENDIF()

find_package(glm REQUIRED HINTS "${glm_DIR}")

IF( glm_FOUND )
  message (" found glm ")
ELSE()
  SET( glm_DIR "$ENV{GLM_PATH}" )
  SET ( glm_INCLUDE_DIR "${glm_DIR}")
  message (" glm include directory set to : ${glm_INCLUDE_DIR}")
ENDIF()

# #find glfw
# IF( DEFINED ENV{GLFW_PATH} )
#   SET( glfw_DIR "$ENV{GLFW_PATH}" )
# ENDIF()

# #find_package(glfw HINTS "${glfw_DIR}")
# find_package(glfw3 3.3)

# IF( glfw3_FOUND )
#   message (" found glfw ")
# ELSE()
#   message (" not found glfw ")
#   SET ( glfw3_INCLUDE_DIR "${glfw_DIR}/include")
#   SET ( glfw3_LIBRARY_DIR "${glfw_DIR}/Build/src/Debug")
#   SET ( glfw3_LIBRARIES "${glfw3_LIBRARY_DIR}/glfw3.lib")
# ENDIF()

#create library
add_library(Vulkan ${SourceList})

add_compile_definitions(VULKAN_ASSETS_PATH="${VulkanDataPath}")
add_compile_definitions(EFFECTS_PATH="${EffectsPath}")

# forces anyone who uses this lib to include the below mentioned source dir into their project props
target_include_directories(Vulkan PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(Vulkan PUBLIC ${SUBDIRS})

SET( Json_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/ThirdParty/RapidJson" )
message ("json include directory set to : ${Json_INCLUDE_DIR}")
target_include_directories(Vulkan PUBLIC ${Json_INCLUDE_DIR})

#include directories
message ("include directories ${Vulkan_INCLUDE_DIR} ${glm_INCLUDE_DIR}")
target_include_directories(Vulkan PUBLIC
    ${Vulkan_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)


#library directories
link_directories(${glfw3_LIBRARY_DIR} ${vulkan_LIBRARY_DIR})

#trying to add the vulkan lib to this lib : NOT WORKING
message ("vulkan library ${Vulkan_LIBRARIES}")
message ("glfw3 library ${glfw3_LIBRARIES}")
target_link_libraries(Vulkan PUBLIC ${Vulkan_LIBRARIES} Core)
target_link_libraries(Vulkan PRIVATE glm::glm)

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

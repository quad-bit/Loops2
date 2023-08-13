
##  file list creation Begin

include("${CMAKE_CURRENT_SOURCE_DIR}/../../Util.cmake")

HEADER_DIRECTORIES(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

Set ( SourceList "")

FOREACH(subdir ${SUBDIRS})
    #message(${subdir})

    string(FIND ${subdir} "Drivers" result)

    IF(NOT result EQUAL -1 )
      CONTINUE()
    ENDIF()

    file(GLOB fileListInDir
        "${subdir}/*.h"
        "${subdir}/*.cpp"
        "${subdir}/*.txt"
    )

    LIST(APPEND SourceList ${fileListInDir})

    source_group(${subdir} FILES ${fileListInDir})

ENDFOREACH()

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

message("\n")

#create library
add_library(Renderer ${SourceList})

FOREACH(subdir ${SUBDIRS})
    # forces anyone who uses this lib to include the below mentioned source dir into their project props
    string(FIND ${subdir} "Drivers" result)

    IF(NOT result EQUAL -1 )
      CONTINUE()
    ENDIF()

    target_include_directories(Renderer PUBLIC "${subdir}")
ENDFOREACH()

#target_precompile_headers(Renderer PRIVATE RenderingPrecompiled.pch)

if (MSVC)
    # For precompiled header.
    # Set
    # "Precompiled Header" to "Use (/Yu)"
    # "Precompiled Header File" to "stdafx.h"
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /YuRenderingPrecompiled.h /FIRenderingPrecompiled.h")
endif()

set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/Rendering/RenderingPrecompiled.cpp"
    PROPERTIES
    COMPILE_FLAGS "/YcRenderingPrecompiled.h"
)

find_package(glfw3 REQUIRED)

# ADD_GLFW(includeDir libDir libraries)
# target_include_directories(Renderer PUBLIC includeDir)
# link_directories(${libDir})
target_link_libraries(Renderer PUBLIC Core Vulkan glfw)

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


##  file list creation Begin

include("${CMAKE_CURRENT_SOURCE_DIR}/../../Util.cmake")

HEADER_DIRECTORIES(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

Set ( SourceList "")

FOREACH(subdir ${SUBDIRS})
    #message(${subdir})
    #file(RELATIVE_PATH test ${CMAKE_CURRENT_SOURCE_DIR} ${subdir})
    #message(${test})

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

#create library
add_library(EngineInterface ${SourceList})

# forces anyone who uses this lib to include the below mentioned source dir into their project props
target_include_directories(EngineInterface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(EngineInterface PUBLIC ${SUBDIRS})
#target_include_directories(EngineInterface PUBLIC ${plog_INCLUDE_DIR})

target_link_libraries(EngineInterface PUBLIC Core)
target_link_libraries(EngineInterface PUBLIC Renderer)

if (MSVC)
    # For precompiled header.
    # Set
    # "Precompiled Header" to "Use (/Yu)"
    # "Precompiled Header File" to "stdafx.h"

endif()

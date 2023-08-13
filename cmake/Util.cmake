
# Gives all the subdirectories recursively
MACRO(HEADER_DIRECTORIES return_list curDir)

    get_filename_component(ABSOLUTE_PATH ${curDir} ABSOLUTE)
    #message(${curDir})

    FILE(GLOB_RECURSE new_list "${curDir}/*.h" "${curDir}/*.cpp" "${curDir}/*.txt" )
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} DIRECTORY)
        file(RELATIVE_PATH dir_path ${CMAKE_CURRENT_SOURCE_DIR} ${dir_path})
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()

    list(LENGTH dir_list size)
    IF(NOT ${size} EQUAL 0)
        LIST(REMOVE_DUPLICATES dir_list)
    ENDIF()

    SET(${return_list} ${dir_list})
ENDMACRO()

# Gives all the subdirectories (one level)... actually not that effective.
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    message(${child})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()

  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

MACRO(GET_SOURCE_FILES result curDir)
    HEADER_DIRECTORIES(SUBDIRS ${curDir})
    Set (SourceList "")
    FOREACH(subdir ${SUBDIRS})
        file(GLOB fileListInDir
            "${subdir}/*.h"
            "${subdir}/*.cpp"
            "${subdir}/*.txt"
        )
        LIST(APPEND SourceList ${fileListInDir})
        source_group(${subdir} FILES ${fileListInDir})
    ENDFOREACH()

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
    SET(${result} ${SourceList})
ENDMACRO()

MACRO(GROUP_SOURCE_FILES curDir)
    HEADER_DIRECTORIES(SUBDIRS ${curDir})
    Set (SourceList "")
    FOREACH(subdir ${SUBDIRS})
        file(GLOB fileListInDir
            "${subdir}/*.h"
            "${subdir}/*.cpp"
            "${subdir}/*.txt"
        )
        LIST(APPEND SourceList ${fileListInDir})
        source_group(${subdir} FILES ${fileListInDir})
    ENDFOREACH()

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
ENDMACRO()



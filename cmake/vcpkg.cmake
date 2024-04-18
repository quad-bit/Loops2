set(VCPKG_BOOTSTRAP OFF)
set(VCPKG_SCRIPT_RELATIVE_PATH "scripts/buildsystems/vcpkg.cmake")

set(VCPKG_SUBMODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../Libraries/vcpkg")
set(VCPKG_SUBMODULE_TOOLCHAIN_FILE "${VCPKG_SUBMODULE_PATH}/${VCPKG_SCRIPT_RELATIVE_PATH}")

if(NOT DEFINED VCPKG_TOOLCHAIN_FILE AND EXISTS ${VCPKG_SUBMODULE_TOOLCHAIN_FILE})
    set(VCPKG_BOOTSTRAP ON)
    set(VCPKG_TOOLCHAIN_FILE ${VCPKG_SUBMODULE_TOOLCHAIN_FILE})
    get_filename_component(VCPKG_TOOLCHAIN_FILE ${VCPKG_TOOLCHAIN_FILE} ABSOLUTE)
endif()

if(${VCPKG_BOOTSTRAP})
    if(WIN32 AND NOT EXISTS ${VCPKG_SUBMODULE_PATH}/vcpkg.exe)
        message("vcpkg bootstrap")
        execute_process(COMMAND cmd /c ${VCPKG_SUBMODULE_PATH}/bootstrap-vcpkg.bat -disableMetrics)
    elseif(UNIX AND NOT EXISTS ${VCPKG_SUBMODULE_PATH}/vcpkg)
        message("vcpkg bootstrap")
        execute_process(COMMAND ${VCPKG_SUBMODULE_PATH}/bootstrap-vcpkg.sh -disableMetrics)
    endif()
endif()

if(DEFINED VCPKG_TOOLCHAIN_FILE)
    message("Using vcpkg from ${VCPKG_TOOLCHAIN_FILE}")
else()
    message(FATAL_ERROR "Toolchain file not found")
endif()

if(NOT DEFINED VCPKG_MANIFEST_DIR)
    set(VCPKG_MANIFEST_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
endif()

set (CMAKE_TOOLCHAIN_FILE "${VCPKG_TOOLCHAIN_FILE}")
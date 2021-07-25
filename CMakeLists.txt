cmake_minimum_required(VERSION 3.16)

# Fix .dll loading problme after version 3.16
set(CMAKE_FIND_LIBRARY_SUFFIXES .dll ${CMAKE_FIND_LIBRARY_SUFFIXES})

# Load external cmake modules
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(JSONParser)

# Load configurations
sbeClearJson(config)
file(READ upp.json conf)
sbeParseJson(config conf)

project(
    "${config.namespace}${config.name}"
    VERSION "${config.version}"
    DESCRIPTION "${config.description}"
    LANGUAGES CXX
    )

# Load modules
unset (SUBMODULES_NAME)
unset (SUBMODULES_PATH)
unset (SUBMODULES_TAG)
foreach(var ${config.modules})
    message("Adding module: ${config.modules_${var}.name}")
    list (APPEND SUBMODULES_NAME "${config.modules_${var}.name}")
    list (APPEND SUBMODULES_PATH "${config.modules_${var}.path}")
    list (APPEND SUBMODULES_TAG "${config.modules_${var}.tag}")
endforeach()

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_SH "CMAKE_SH-NOTFOUND")
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    include(CTest)
    set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
endif()

# Fetch the content of the submodules
include(FetchContent)
if(SUBMODULES_NAME)
    list(LENGTH SUBMODULES_NAME len1)
    math(EXPR len2 "${len1} - 1")
    foreach(val RANGE ${len2})
        list(GET SUBMODULES_NAME ${val} Name)
        list(GET SUBMODULES_PATH ${val} Path)
        list(GET SUBMODULES_TAG ${val} Tag)
        FetchContent_Declare(
            ${Name}
            GIT_REPOSITORY ${Path}
            GIT_TAG        ${Tag}
            USES_TERMINAL_DOWNLOAD YES
            USES_TERMINAL_UPDATE   YES
        )
        FetchContent_MakeAvailable(${Name})
    endforeach()
endif()

# Make cmake aware of the presence of other CMakeLists files in core and gui folders
add_subdirectory(src)

if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME) AND BUILD_TESTING)
  add_subdirectory(apps)
  add_subdirectory(tests)
endif()



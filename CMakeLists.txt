#
# abstract_astar_search cmake 
#
cmake_minimum_required(VERSION 2.8.7)
project(abstract_astar_search C)

#
# abstract_astar_search version
#
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 1)

#
# Version file :)
#
configure_file(
    "${PROJECT_SOURCE_DIR}/include/version.h.in"
    "${PROJECT_SOURCE_DIR}/include/version.h"
    )

#
# Avoid source tree pollution
#
if(${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(WARNING "In-source builds are not recommended. "
        "Consider building in a separate directory, "
        "to avoid polluting the source tree.")
endif()

#
# Per compiler flags
# 
set(GCC_COMPILER_FLAGS_DEBUG    "-Wall -g")
set(GCC_COMPILER_FLAGS_RELEASE  "-Wall -DNO_LINENOS -DNDEBUG")
set(CLANG_COMPILER_FLAGS_DEBUG  "-Wall -g")
set(CLANG_COMPILER_FLAGS_RELEASE"-Wall -DNO_LINENOS -DNDEBUG")
set(MSVC_COMPILER_FLAGS_DEBUG   "/W3 /D_CRT_SECURE_NO_WARNINGS /wd4005 /wd4996 /wd4820 /nologo")
set(MSVC_COMPILER_FLAGS_RELEASE "/DNO_LINENOS /DNDEBUG /W3 /D_CRT_SECURE_NO_WARNINGS /wd4005 /wd4996 /wd4820 /nologo")

#
# Set more "sane" output folders 
#
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#
# Compiler specific flags
#
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    message(STATUS "Setting clang flags.")
    set(CMAKE_C_FLAGS_DEBUG
        "${CMAKE_C_FLAGS_DEBUG} ${CLANG_COMPILER_FLAGS_DEBUG}")
    set(CMAKE_C_FLAGS_RELEASE 
        "${CMAKE_C_FLAGS_RELEASE} ${CLANG_COMPILER_FLAGS_RELEASE}")
    add_definitions("-std=gnu99")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    message(STATUS "Setting gcc flags.")
    set(CMAKE_C_FLAGS_DEBUG 
        "${CMAKE_C_FLAGS_DEBUG} ${GCC_COMPILER_FLAGS_DEBUG}")
    set(CMAKE_C_FLAGS_RELEASE 
        "${CMAKE_C_FLAGS_RELEASE} ${GCC_COMPILER_FLAGS_RELEASE}")
    add_definitions("-std=gnu99")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
    message(STATUS "Setting MSVC flags.")
    set(CMAKE_C_FLAGS_DEBUG 
        "${CMAKE_C_FLAGS_DEBUG} ${MSVC_COMPILER_FLAGS_DEBUG}")
    set(CMAKE_C_FLAGS_RELEASE 
        "${CMAKE_C_FLAGS_RELEASE} ${MSVC_COMPILER_FLAGS_RELEASE}")
endif()

message(STATUS "Set debug flags to ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "Set release flags to ${CMAKE_C_FLAGS_RELEASE}")

#
# Cmake Modules that define how to find any external libs will be placed here
#
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake-modules)

#
# Add external libs that are required for abstract_astar_search to build here as sub
# directories such that we can build and link against them as static libraries
#
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/bstring)

#
# Header files required by abstract_astar_search 
#
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/deps/bstring/include)

#
# Add the abstract_astar_search src and tests to the build
#
add_subdirectory(src)
add_subdirectory(test)
#add_subdirectory(docs)
add_subdirectory(data)

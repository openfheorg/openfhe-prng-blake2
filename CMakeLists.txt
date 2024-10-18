# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.10)

set(LIBRARY_NAME "PRNGengine")
project(${LIBRARY_NAME} VERSION 1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT UNIX OR NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # OpenFHE may use an external shared object provided by user for PRNG and linked with g++ on Linux.
    message(FATAL_ERROR "This library must be linked using g++ on Linux only")
endif()

option(BUILD_SHARED_LIBS "Build shared library" ON)
message( STATUS "BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
message("***** INSTALL IS AT ${CMAKE_INSTALL_PREFIX}; to change, run cmake with -DCMAKE_INSTALL_PREFIX=/your/path")

# Add the library target
set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/src/lib")
file(GLOB SOURCE_FILES CONFIGURE_DEPENDS ${SOURCE_DIR}/*.cpp ${SOURCE_DIR}/*.c)

if(BUILD_SHARED_LIBS)
    add_library(${LIBRARY_NAME} SHARED ${SOURCE_FILES})
else()
    add_library(${LIBRARY_NAME} STATIC ${SOURCE_FILES})
endif()

target_compile_options(${LIBRARY_NAME} PRIVATE
                       -Wall -Wextra -Wpedantic -Werror -O3
                       -Wno-unknown-pragmas -Wno-parentheses
                      )

set_target_properties(${LIBRARY_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(${LIBRARY_NAME} PROPERTIES
                      VERSION ${PROJECT_VERSION}
                      SOVERSION 1
                     )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/include)

# install the library
install(TARGETS ${LIBRARY_NAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
       )

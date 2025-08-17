cmake_minimum_required(VERSION 3.16)
project(ekf_soc LANGUAGES CXX)

# Options for developers
option(ENABLE_DEV_WARNINGS "Turn on extra warnings" OFF)

# Global C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Library target for your EKF implementation
add_library(ekf_soc STATIC
    src/ekf_soc.cpp
)

# Public headers
target_include_directories(ekf_soc
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Warnings across compilers (opt-in)
if(ENABLE_DEV_WARNINGS)
    if(MSVC)
        target_compile_options(ekf_soc PRIVATE /W4 /permissive-)
    else()
        target_compile_options(ekf_soc PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

# Executable
add_executable(ekf_soc_app src/main.cpp)

target_link_libraries(ekf_soc_app PRIVATE ekf_soc)

# Group outputs in build tree (nice QoL)
set_target_properties(ekf_soc_app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
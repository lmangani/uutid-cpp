cmake_minimum_required(VERSION 3.14)
project(tsid VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Option to enable/disable tests
option(BUILD_TESTING "Build tests" ON)

# Add the header-only library target
add_library(tsid INTERFACE)
target_include_directories(tsid 
    INTERFACE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Testing configuration
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

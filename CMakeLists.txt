# script to run this file:
# cmake -S . -B build
# cd build && make

cmake_minimum_required(VERSION 4.3.2)

project(gbemu)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include your headers
include_directories(include)

# Find SFML (installed via Homebrew)
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

# Find Native File Dialog (manual since pkg-config was annoying)
find_path(NFD_INCLUDE_DIR nfd.h PATHS /opt/homebrew/include)
find_library(NFD_LIBRARY nfd PATHS /opt/homebrew/lib)

# Find Catch2 (for testing)
find_package(Catch2 3 REQUIRED)
# These tests can use the Catch2-provided main
add_executable(tests 
    test/test.cpp
    src/cpu.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

# nlohmann json package
find_package(nlohmann_json REQUIRED)


# Source files
file(GLOB SRC_FILES src/*.cpp)


add_executable(${PROJECT_NAME} ${SRC_FILES} )

# Link libraries
target_include_directories(gbemu PRIVATE ${NFD_INCLUDE_DIR})

target_link_libraries(gbemu
    SFML::Graphics
    SFML::Window
    SFML::System
    ${NFD_LIBRARY}
    nlohmann_json::nlohmann_json
    "-framework AppKit"
)
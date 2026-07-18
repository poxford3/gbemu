# script to run this file:
# cmake -S . -B build
# cd build && make
# or cmake --build build

cmake_minimum_required(VERSION 4.3.2)

project(gbemu)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include header files
include_directories(include)


# Find SDL2 (installed via Homebrew)
find_package(SDL2 REQUIRED)
find_path(SDL2_IMAGE_INCLUDE_DIR SDL_image.h PATHS /opt/homebrew/include/SDL2)
find_library(SDL2_IMAGE_LIBRARY SDL2_image PATHS /opt/homebrew/lib)
find_path(SDL2_TTF_INCLUDE_DIR SDL_ttf.h PATHS /opt/homebrew/include/SDL2)
find_library(SDL2_TTF_LIBRARY SDL2_ttf PATHS /opt/homebrew/lib)

# Find Native File Dialog (manual since pkg-config was annoying)
find_path(NFD_INCLUDE_DIR nfd.h PATHS /opt/homebrew/include)
find_library(NFD_LIBRARY nfd PATHS /opt/homebrew/lib)

# Find Catch2 (for testing)
find_package(Catch2 3 REQUIRED)
# These tests can use the Catch2-provided main
add_executable(tests 
    test/test.cpp
    src/cpu.cpp
    src/mmu.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

# nlohmann json package
find_package(nlohmann_json REQUIRED)


# Source files
file(GLOB SRC_FILES src/*.cpp)

# dear imgui
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    # Optional while developing
    ${IMGUI_DIR}/imgui_demo.cpp

    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)

add_executable(${PROJECT_NAME}
    ${SRC_FILES}
    ${IMGUI_SOURCES}
)
# ---
target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
# ---

# Link libraries
target_include_directories(${PROJECT_NAME} PRIVATE ${NFD_INCLUDE_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIR}
    ${SDL2_TTF_INCLUDE_DIR}
    ${NFD_INCLUDE_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(${PROJECT_NAME}
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARY}
    ${SDL2_TTF_LIBRARY}
    ${NFD_LIBRARY}
    nlohmann_json::nlohmann_json
    "-framework AppKit"
)
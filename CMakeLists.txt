cmake_minimum_required(VERSION 3.10)
project(ZH_Base)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Dependencies ---
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)

# --- Include directories ---
include_directories(
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    includes
    .
)

# --- Source files ---
# Root .cpp files
file(GLOB ROOT_SOURCES
    "*.cpp"
)

# Sources inside includes/
file(GLOB INCLUDE_SOURCES
    "includes/*.cpp"
)

# ImGui
file(GLOB IMGUI_SOURCES
    "imgui/*.cpp"
    "imgui/backends/imgui_impl_sdl2.cpp"
    "imgui/backends/imgui_impl_opengl3.cpp"
)

add_executable(ZH_Base
    ${ROOT_SOURCES}
    ${INCLUDE_SOURCES}
    ${IMGUI_SOURCES}
)
target_compile_definitions(ZH_Base PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLEW)

# --- Libraries ---
target_link_libraries(ZH_Base
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
)

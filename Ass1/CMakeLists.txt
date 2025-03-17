cmake_minimum_required(VERSION 3.10)
project(MarbleSolitaire)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${OPENGL_INCLUDE_DIR})

# Copy shader files to build directory
file(COPY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# Add imgui as a library
add_library(imgui
    external/imgui/imgui.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)

# Source files
set(SOURCES
    src/main.cpp
    src/game.cpp
    src/renderer.cpp
    src/shader.cpp
)

# Create executable
add_executable(marble_solitaire ${SOURCES})

# Link libraries
target_link_libraries(marble_solitaire
    ${OPENGL_LIBRARIES}
    GLEW::GLEW
    glfw
    imgui
)
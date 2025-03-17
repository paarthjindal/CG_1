// Define this to tell ImGui not to use its built-in loader
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM

// Include GLEW before the ImGui implementation
#include <GL/glew.h>

// Now include the ImGui implementation file
#include "imgui_impl_opengl3.cpp"
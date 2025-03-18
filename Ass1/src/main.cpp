#include <GL/glew.h>
#include <GLFW/glfw3.h>
// #include <../external/imgui/imgui_impl_glfw.h>
// #include <imgui.h>
#include <../external/imgui/imgui.h>
#include <../external/imgui/backends/imgui_impl_glfw.h>
#include <../external/imgui/backends/imgui_impl_opengl3.h>
#include <iostream>
#include <string>

#include "../include/game.h"
#include "../include/renderer.h"

// Window dimensions and title
const char *WINDOW_TITLE = "Marble Solitaire";
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

// Global objects
MarbleSolitaire *game = nullptr;
Renderer *renderer = nullptr;
GLFWwindow *window = nullptr;

// Function prototypes
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void initializeGLFW();
void initializeImGui();
void mainLoop();
void cleanup();

int main()
{
    // Initialize GLFW and create window
    initializeGLFW();
    if (!window)
    {
        return -1;
    }

    // Initialize ImGui
    initializeImGui();

    // Initialize game and renderer
    game = new MarbleSolitaire(7);
    game->startTimer();

    renderer = new Renderer(WINDOW_WIDTH, WINDOW_HEIGHT);
    renderer->init();

    // Main game loop
    mainLoop();

    // Cleanup
    cleanup();

    return 0;
}

void initializeGLFW()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Set callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return;
    }

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
}

void initializeImGui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
}

void mainLoop()
{
    game->startTimer();
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events
        glfwPollEvents();

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Clear the framebuffer
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render game
        renderer->renderGame(*game);

        // Create ImGui interface
        renderer->renderUI(*game);

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window);
    }
}

void cleanup()
{
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Delete game and renderer
    delete renderer;
    delete game;

    // Terminate GLFW
    glfwTerminate();
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // Get cursor position
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // Convert screen coordinates to board position
        Position boardPos = renderer->getBoardPosition(static_cast<int>(xpos), static_cast<int>(ypos), *game);

        // Process the click
        if (boardPos.isValid()) {
            // If no piece is selected, try to select one
            if (!game->getSelectedPosition().isValid()) {
                game->selectPosition(boardPos.row, boardPos.col);
            } else {
                // If a piece is already selected, try to move it
                game->tryMove(boardPos.row, boardPos.col);
            }
        }
    }
}
// Add this function if it doesn't exist
// void refreshWindow()
// {
//     // Force window refresh after game state changes
//     glfwPollEvents();
//     renderer->renderGame(*game);
//     glfwSwapBuffers(window);
// }
// // Modify your mouse callback to use it
// // Modify the mouse callback to use the correct window size variables
// void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
// {
//     if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
//     {
//         // Get cursor position
//         double xpos, ypos;
//         glfwGetCursorPos(window, &xpos, &ypos);

//         // Convert screen coordinates to game coordinates
//         int boardSize = game->getBoardSize();
//         float cellSize = 1.6f / boardSize;

//         // Use WINDOW_WIDTH and WINDOW_HEIGHT instead of windowWidth and windowHeight
//         int col = (int)((xpos / WINDOW_WIDTH + 0.8f) / cellSize);
//         int row = (int)((-ypos / WINDOW_HEIGHT + 0.8f) / cellSize);

//         // Process click
//         bool stateChanged = game->processClick(row, col);
//         if (stateChanged)
//         {
//             std::cout << "Game state updated, forcing refresh" << std::endl;
//             refreshWindow(); // Force window update immediately
//         }
//     }
// }
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_U:
            game->undoMove();
            break;
        case GLFW_KEY_R:
            game->redoMove();
            break;
        case GLFW_KEY_N:
            game->reset();
            game->startTimer();
            break;
        }
    }
}
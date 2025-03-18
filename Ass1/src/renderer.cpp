#include "../include/renderer.h"
#include <../external/imgui/imgui.h>
#include <cmath>
#include <string>
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer(int width, int height) : windowWidth(width), windowHeight(height) {
    // Initialize member variables
}

Renderer::~Renderer() {
    // Clean up OpenGL resources
    glDeleteVertexArrays(1, &squareVAO);
    glDeleteBuffers(1, &squareVBO);

    glDeleteVertexArrays(1, &circleVAO);
    glDeleteBuffers(1, &circleVBO);
}
#include <unistd.h>
void Renderer::init() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        std::cout << "Current working directory: " << cwd << std::endl;
    }

    // Try to load shaders with absolute paths
    std::string basePath = std::string(cwd) + "/assets/shaders/";

    std::cout << "Loading shaders from: " << basePath << std::endl;

    squareShader.loadFromFile((basePath + "square.vs").c_str(),
                             (basePath + "square.fs").c_str());

    circleShader.loadFromFile((basePath + "circle.vs").c_str(),
                             (basePath + "circle.fs").c_str());

    // Check for shader compilation errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error after shader loading: " << err << std::endl;
    }

    // Create geometry
    createSquare();
    createCircle();
}
void Renderer::createSquare() {
    // Define square vertices (using a simple quad)
    float vertices[] = {
        -0.5f, -0.5f,  // Bottom left
         0.5f, -0.5f,  // Bottom right
         0.5f,  0.5f,  // Top right
        -0.5f,  0.5f   // Top left
    };

    // Create and configure VAO for square
    glGenVertexArrays(1, &squareVAO);
    glBindVertexArray(squareVAO);

    // Create and configure VBO
    glGenBuffers(1, &squareVBO);
    glBindBuffer(GL_ARRAY_BUFFER, squareVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Configure vertex attributes
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::createCircle() {
    std::cout << "Creating circle geometry..." << std::endl;

    // Define vertices for a square
    float vertices[] = {
        -0.5f, -0.5f,  // Bottom left
         0.5f, -0.5f,  // Bottom right
         0.5f,  0.5f,  // Top right
        -0.5f, -0.5f,  // Bottom left (repeated)
         0.5f,  0.5f,  // Top right (repeated)
        -0.5f,  0.5f   // Top left
    };

    // Generate VAO and VBO
    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &circleVBO);

    // Bind VAO first
    glBindVertexArray(circleVAO);

    // Bind and setup VBO
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Check for OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "OpenGL error when creating circle: " << error << std::endl;
    } else {
        std::cout << "Circle geometry created successfully" << std::endl;
    }
}
void Renderer::renderGame(const MarbleSolitaire& game) {
    // Debug output - only print occasionally to avoid spam
    static int frameCounter = 0;
    if (frameCounter++ % 60 == 0) {  // Print every 60 frames
        std::cout << "Rendering game with " << game.getRemainingMarbles() << " marbles" << std::endl;
    }

    // Clear the screen
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    renderBoard(game);
    renderMarbles(game);
    renderSelection(game);
    renderGameInfo(game);
}

void Renderer::renderBoard(const MarbleSolitaire& game) {
    // Use orthographic projection for 2D rendering
    glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);

    // Calculate cell size based on board size
    float cellSize = 1.6f / game.getBoardSize();

    // Bind square shader for drawing the board squares
    squareShader.use();
    squareShader.setMat4("projection", projection);

    // Render board cells
    for (int row = 0; row < game.getBoardSize(); row++) {
        for (int col = 0; col < game.getBoardSize(); col++) {
            // Skip invalid positions
            if (game.getCell(row, col) == INVALID)
                continue;

            // Calculate position
            float x = -0.8f + cellSize * col + cellSize * 0.5f;
            float y = 0.8f - cellSize * row - cellSize * 0.5f;

            // Set transformation
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(x, y, 0.0f));
            model = glm::scale(model, glm::vec3(cellSize * 0.9f, cellSize * 0.9f, 1.0f));
            squareShader.setMat4("transform", model);

            // Set color based on cell type
            squareShader.setVec4("color", glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)); // Board color

            // Draw square
            glBindVertexArray(squareVAO);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
    }
}
void Renderer::renderMarbles(const MarbleSolitaire& game) {
     // Debug: Count marbles to render
     int marbleCount = 0;
     for (int row = 0; row < game.getBoardSize(); row++) {
         for (int col = 0; col < game.getBoardSize(); col++) {
             if (game.getCell(row, col) == MARBLE) {
                 marbleCount++;
             }
         }
     }
     static int lastRenderedCount = -1;
     if (marbleCount != lastRenderedCount) {
         std::cout << "About to render " << marbleCount << " marbles" << std::endl;
         lastRenderedCount = marbleCount;
     }
    // Use orthographic projection for 2D rendering
    glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);

    // Calculate cell size based on board size
    float cellSize = 1.6f / game.getBoardSize();
    float marbleSize = cellSize * 0.7f; // Slightly smaller than cells

    // Bind circle shader for drawing marbles
    circleShader.use();
    circleShader.setMat4("projection", projection);

    // Enable blending for better-looking circles
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Render marbles
    for (int row = 0; row < game.getBoardSize(); row++) {
        for (int col = 0; col < game.getBoardSize(); col++) {
            if (game.getCell(row, col) == MARBLE) {
                // Calculate position
                float x = -0.8f + cellSize * col + cellSize * 0.5f;
                float y = 0.8f - cellSize * row - cellSize * 0.5f;

                // Set transformation
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(x, y, 0.0f));
                model = glm::scale(model, glm::vec3(marbleSize, marbleSize, 1.0f));
                circleShader.setMat4("transform", model);

                // Set marble color to bright red for visibility
                circleShader.setVec4("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

                // Draw circle
                glBindVertexArray(circleVAO);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            }
        }
    }

    // Disable blending
    glDisable(GL_BLEND);
}
void Renderer::renderSelection(const MarbleSolitaire& game) {
    // Get selected position
    Position selected = game.getSelectedPosition();

    // If no selection, just return
    if (!selected.isValid()) {
        return;
    }

    // Remove debug output that's flooding your console
    // std::cout << "Rendering selection at position: (" << selected.row << ", " << selected.col << ")" << std::endl;

    // Use orthographic projection for 2D rendering
    glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);

    // Calculate cell size based on board size
    float cellSize = 1.6f / game.getBoardSize();

    // Calculate position
    float x = -0.8f + cellSize * selected.col + cellSize * 0.5f;
    float y = 0.8f - cellSize * selected.row - cellSize * 0.5f;

    // Use square shader for drawing selection highlight
    squareShader.use();
    squareShader.setMat4("projection", projection);

    // Set transformation
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(cellSize * 0.95f, cellSize * 0.95f, 1.0f));
    squareShader.setMat4("transform", model);

    // Set highlight color - bright yellow with high opacity
    squareShader.setVec4("color", glm::vec4(1.0f, 1.0f, 0.0f, 0.7f));

    // Draw highlight with blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(squareVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisable(GL_BLEND);
}

void Renderer::renderGameInfo(const MarbleSolitaire& game) {
    // Game info is rendered via ImGui in renderUI
}

void Renderer::renderUI(const MarbleSolitaire& game) {
    // Time display (top left)
    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::SetNextWindowSize(ImVec2(150, 50));
    ImGui::SetNextWindowBgAlpha(0.7f);
    ImGui::Begin("Time", nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Time: %d seconds", game.getElapsedSeconds());
    ImGui::End();

    // Marbles count (top right)
    ImGui::SetNextWindowPos(ImVec2(windowWidth - 160, 10));
    ImGui::SetNextWindowSize(ImVec2(150, 50));
    ImGui::SetNextWindowBgAlpha(0.7f);
    ImGui::Begin("Marbles", nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Marbles left: %d", game.getRemainingMarbles());
    ImGui::End();

    // Controls panel (bottom)
    ImGui::SetNextWindowPos(ImVec2(windowWidth / 2 - 150, windowHeight - 60));
    ImGui::SetNextWindowSize(ImVec2(300, 50));
    ImGui::Begin("Controls", nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize);

    // Control buttons
    if (ImGui::Button("Undo")) {
        const_cast<MarbleSolitaire&>(game).undoMove();
    }
    ImGui::SameLine();
    if (ImGui::Button("Redo")) {
        const_cast<MarbleSolitaire&>(game).redoMove();
    }
    ImGui::SameLine();
    if (ImGui::Button("New Game")) {
        const_cast<MarbleSolitaire&>(game).reset();
        const_cast<MarbleSolitaire&>(game).startTimer();
    }

    ImGui::End();

    // Game over notification
    if (game.gameOver()) {
        ImGui::SetNextWindowPos(ImVec2(windowWidth / 2 - 150, windowHeight / 2 - 50));
        ImGui::SetNextWindowSize(ImVec2(300, 100));
        ImGui::Begin("Game Over", nullptr,
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse);

        if (game.hasWon()) {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "You Won!");
            ImGui::Text("Congratulations! You solved the puzzle!");
        } else {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Game Over!");
            ImGui::Text("No more moves possible. Try again!");
        }

        if (ImGui::Button("New Game")) {
            const_cast<MarbleSolitaire&>(game).reset();
            const_cast<MarbleSolitaire&>(game).startTimer();
        }

        ImGui::End();
    }
}

glm::vec2 Renderer::windowToBoard(int x, int y, const MarbleSolitaire& game) {
    // Convert window coordinates (in pixels) to normalized device coordinates (-1 to 1)
    float ndcX = (2.0f * x) / windowWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * y) / windowHeight;

    // Scale to our board coordinate system
    float boardSize = 1.6f;
    float boardX = (ndcX + 1.0f) * 0.5f * boardSize - 0.8f;
    float boardY = (ndcY + 1.0f) * 0.5f * boardSize - 0.8f;

    return glm::vec2(boardX, boardY);
}

Position Renderer::getBoardPosition(int x, int y, const MarbleSolitaire& game) {
    // Get board coordinates
    glm::vec2 boardCoord = windowToBoard(x, y, game);

    // Calculate cell size
    float cellSize = 1.6f / game.getBoardSize();

    // Calculate row and column
    int col = static_cast<int>((boardCoord.x + 0.8f) / cellSize);
    int row = static_cast<int>((0.8f - boardCoord.y) / cellSize);

    // Check if in valid range
    if (row >= 0 && row < game.getBoardSize() && col >= 0 && col < game.getBoardSize() &&
        game.getCell(row, col) != INVALID) {
        return Position(row, col);
    }

    // Return invalid position if out of range
    return Position(-1, -1);
}
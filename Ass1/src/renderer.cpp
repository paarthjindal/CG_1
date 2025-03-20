#include "../include/renderer.h"
#include <../external/imgui/imgui.h>
#include <cmath>
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include <../include/renderer.h>
#include <../include/theme.h>
Renderer::Renderer(int width, int height) : windowWidth(width), windowHeight(height)
{
    // Initialize member variables
}

Renderer::~Renderer()
{
    // Clean up OpenGL resources
    glDeleteVertexArrays(1, &squareVAO);
    glDeleteBuffers(1, &squareVBO);

    glDeleteVertexArrays(1, &circleVAO);
    glDeleteBuffers(1, &circleVBO);
}
#include <unistd.h>


void Renderer::setTheme(const Theme& theme) {
    currentTheme = theme;
}

void Renderer::init()
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        std::cout << "Current working directory: " << cwd << std::endl;
    }

    // Try to load shaders with absolute paths
    std::string basePath = std::string(cwd) + "/assets/shaders/";

    std::cout << "Loading shaders from: " << basePath << std::endl;

    squareShader.loadFromFile((basePath + "square.vs").c_str(),
                              (basePath + "square.fs").c_str());

    circleShader.loadFromFile((basePath + "circle.vs").c_str(),
                              (basePath + "circle.fs").c_str());

                               // Add this line to load the highlight shader
    highlightShader.loadFromFile((basePath + "square.vs").c_str(),
    (basePath + "square.fs").c_str());  // We can reuse square shaders

    // Add this after shader loading in init():
    // if (!circleShader.isValid())
    // {
    //     std::cerr << "Circle shader failed to compile!" << std::endl;
    // }

    // Check for shader compilation errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error after shader loading: " << err << std::endl;
    }

    // Create geometry
    createSquare();
    createCircle();
    createHighlight();  // Make sure this method exists

}
void Renderer::createSquare()
{
    // Define square vertices (using a simple quad)
    float vertices[] = {
        -0.5f, -0.5f, // Bottom left
        0.5f, -0.5f,  // Bottom right
        0.5f, 0.5f,   // Top right
        -0.5f, 0.5f   // Top left
    };

    // Create and configure VAO for square
    glGenVertexArrays(1, &squareVAO);
    glBindVertexArray(squareVAO);

    // Create and configure VBO
    glGenBuffers(1, &squareVBO);
    glBindBuffer(GL_ARRAY_BUFFER, squareVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Configure vertex attributes
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::createCircle()
{
    std::cout << "Creating circle geometry..." << std::endl;

    // Define vertices for a square with texture coordinates
    float vertices[] = {
        // Positions        // Texture Coords
        -0.5f, -0.5f,       0.0f, 0.0f,   // Bottom left
         0.5f, -0.5f,       1.0f, 0.0f,   // Bottom right
         0.5f,  0.5f,       1.0f, 1.0f,   // Top right
        -0.5f,  0.5f,       0.0f, 1.0f    // Top left
    };

    // Generate and bind VAO
    glGenVertexArrays(1, &circleVAO);
    glBindVertexArray(circleVAO);

    // Generate and bind VBO
    glGenBuffers(1, &circleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute - stride is now 4 floats (position + texcoord)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    std::cout << "Circle geometry created successfully" << std::endl;
}

void Renderer::createHighlight()
{
    // We can reuse squareVAO for the highlight, but if you want a dedicated VAO:
    float vertices[] = {
        -0.5f, -0.5f,  // Bottom left
         0.5f, -0.5f,  // Bottom right
         0.5f,  0.5f,  // Top right
        -0.5f,  0.5f   // Top left
    };

    glGenVertexArrays(1, &highlightVAO);
    glBindVertexArray(highlightVAO);

    glGenBuffers(1, &highlightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, highlightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::renderGame(const MarbleSolitaire &game)
{
    // Debug output - only print occasionally to avoid spam
    static int frameCounter = 0;
    if (frameCounter++ % 60 == 0)
    { // Print every 60 frames
        std::cout << "Rendering game with " << game.getRemainingMarbles() << " marbles" << std::endl;
    }

    // Clear the screen with the theme background color
    glClearColor(
        currentTheme.BACKGROUND_COLOR.r,
        currentTheme.BACKGROUND_COLOR.g,
        currentTheme.BACKGROUND_COLOR.b,
        currentTheme.BACKGROUND_COLOR.a
    );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderBoard(game);
    renderMarbles(game);
    renderSelection(game);
    renderGameInfo(game);
}

void Renderer::renderBoard(const MarbleSolitaire& game) {
    // Use orthographic projection for 2D rendering
    glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);

    // Set up shader
    squareShader.use();
    squareShader.setMat4("projection", projection);

    // Calculate cell size based on board size and theme parameters
    float cellSize = currentTheme.BOARD_WIDTH / game.getBoardSize();

    // Render each cell of the board
    for (int row = 0; row < game.getBoardSize(); row++) {
        for (int col = 0; col < game.getBoardSize(); col++) {
            if (game.getCell(row, col) == INVALID)
                continue;

            // Calculate position using theme constants
            float x = currentTheme.BOARD_ORIGIN_X + cellSize * col + cellSize * 0.5f;
            float y = currentTheme.BOARD_ORIGIN_Y - cellSize * row - cellSize * 0.5f;

            // Set transformation
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(x, y, 0.0f));
            model = glm::scale(model, glm::vec3(cellSize * currentTheme.CELL_SCALE_FACTOR,
                                                cellSize * currentTheme.CELL_SCALE_FACTOR, 1.0f));
            squareShader.setMat4("transform", model);

            // Set color from theme
            squareShader.setVec4("color", currentTheme.BOARD_COLOR);

            // Draw square
            glBindVertexArray(squareVAO);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
    }
}



void Renderer::renderMarbles(const MarbleSolitaire &game)
{
    // Debug: Count marbles to render
    int marbleCount = 0;
    for (int row = 0; row < game.getBoardSize(); row++)
    {
        for (int col = 0; col < game.getBoardSize(); col++)
        {
            if (game.getCell(row, col) == MARBLE)
            {
                marbleCount++;
            }
        }
    }
    static int lastRenderedCount = -1;
    if (marbleCount != lastRenderedCount)
    {
        std::cout << "About to render " << marbleCount << " marbles" << std::endl;
        lastRenderedCount = marbleCount;
    }
    // // Use orthographic projection for 2D rendering
    glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
    // squareShader.use();
    // squareShader.setMat4("projection", projection);

    // // Create a bright green square in the center of screen
    // glm::mat4 testModel = glm::mat4(1.0f);
    // testModel = glm::translate(testModel, glm::vec3(0.0f, 0.0f, 0.0f)); // Center of screen
    // testModel = glm::scale(testModel, glm::vec3(0.5f, 0.5f, 1.0f)); // Large square
    // squareShader.setMat4("transform", testModel);
    // squareShader.setVec4("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Bright green

    // // Draw the test square
    // glBindVertexArray(squareVAO);
    // glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // // Check for OpenGL errors
    // GLenum err = glGetError();
    // if (err != GL_NO_ERROR) {
    //     std::cerr << "OpenGL error during test square rendering: " << err << std::endl;
    // }
    // Calculate cell size based on board size
    // Calculate cell size based on board size and theme parameters
    float cellSize = currentTheme.BOARD_WIDTH / game.getBoardSize();
    float marbleSize = cellSize * currentTheme.MARBLE_SCALE_FACTOR;
    // Enable depth testing to ensure proper ordering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // Enable blending for better-looking circles
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Bind circle shader for drawing marbles
    circleShader.use();
    circleShader.setMat4("projection", projection);
    // Try using the square shader first (for debugging)
    // squareShader.use();
    // squareShader.setMat4("projection", projection);

    // Debug output - print the first marble position
    // static bool first_log_done = false;
    int debugCount = 0;
    // Render marbles
    for (int row = 0; row < game.getBoardSize(); row++)
    {
        for (int col = 0; col < game.getBoardSize(); col++)
        {
            if (game.getCell(row, col) == MARBLE)
            {
                // Calculate position
                // Calculate position using theme constants
                float x = currentTheme.BOARD_ORIGIN_X + cellSize * col + cellSize * 0.5f;
                float y = currentTheme.BOARD_ORIGIN_Y - cellSize * row - cellSize * 0.5f;
                // // Print first few marble positions for debugging
                // if (debugCount < 3) {
                //     std::cout << "Marble " << debugCount << " at position (" << row << "," << col
                //               << ") rendering at screen position (" << x << "," << y << ")" << std::endl;
                //     debugCount++;
                // }

                // Set transformation
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(x, y, currentTheme.MARBLE_Z_POSITION));
                model = glm::scale(model, glm::vec3(marbleSize, marbleSize, 1.0f));
                // squareShader.setMat4("transform", model);
                // // Alternate colors for debugging
                // float r = ((row + col) % 2 == 0) ? 1.0f : 0.8f;
                // float b = ((row + col) % 2 == 0) ? 0.0f : 0.8f;
                // // Set marble color to bright RED (for visibility)
                // squareShader.setVec4("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

                // // Draw square (instead of circle) for testing
                // glBindVertexArray(squareVAO);
                // glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                circleShader.setMat4("transform", model);

                // // Set marble color to bright blue (different color than squares)
                circleShader.setVec4("color", currentTheme.MARBLE_COLOR);

                // // Draw circle
                glBindVertexArray(circleVAO);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                // glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, 0);
            }
        }
    }

    // Disable blending
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
}
void Renderer::renderSelection(const MarbleSolitaire &game)
{
    Position selected = game.getSelectedPosition();

    // If no selection, just return
    if (!selected.isValid()) {
        return;
    }

    // Use orthographic projection
    glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);

    // Calculate cell size based on board size
    float cellSize = currentTheme.BOARD_WIDTH / game.getBoardSize();

    // Calculate position
    float x = currentTheme.BOARD_ORIGIN_X + cellSize * selected.col + cellSize * 0.5f;
    float y = currentTheme.BOARD_ORIGIN_Y - cellSize * selected.row - cellSize * 0.5f;

    // Enable blending for transparent highlight
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Use the highlight shader
    highlightShader.use();
    highlightShader.setMat4("projection", projection);

    // Make highlight slightly larger than the cell
    float highlightScale = cellSize * (currentTheme.CELL_SCALE_FACTOR + 0.05f);

    // Position highlight slightly above the board but below marbles
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.05f));
    model = glm::scale(model, glm::vec3(highlightScale, highlightScale, 1.0f));
    highlightShader.setMat4("transform", model);

    // Set a bright, semi-transparent highlight color
    // If the theme color is too subtle, override it with a brighter one
    glm::vec4 highlightColor = currentTheme.HIGHLIGHT_COLOR;
    if (highlightColor.a < 0.4f) highlightColor.a = 0.4f;  // Ensure minimum opacity
    highlightShader.setVec4("color", highlightColor);

    // Draw the highlight using either dedicated VAO or square VAO
    glBindVertexArray(highlightVAO != 0 ? highlightVAO : squareVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Disable blending
    glDisable(GL_BLEND);
}
void Renderer::renderGameInfo(const MarbleSolitaire &game)
{
    // Game info is rendered via ImGui in renderUI
}

void Renderer::renderUI(const MarbleSolitaire &game)
{
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
    if (ImGui::Button("Undo"))
    {
        const_cast<MarbleSolitaire &>(game).undoMove();
    }
    ImGui::SameLine();
    if (ImGui::Button("Redo"))
    {
        const_cast<MarbleSolitaire &>(game).redoMove();
    }
    ImGui::SameLine();
    if (ImGui::Button("New Game"))
    {
        const_cast<MarbleSolitaire &>(game).reset();
        const_cast<MarbleSolitaire &>(game).startTimer();
    }

    ImGui::End();

    // Game over notification
    if (game.gameOver())
    {
        ImGui::SetNextWindowPos(ImVec2(windowWidth / 2 - 150, windowHeight / 2 - 50));
        ImGui::SetNextWindowSize(ImVec2(300, 100));
        ImGui::Begin("Game Over", nullptr,
                     ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoCollapse);

        if (game.hasWon())
        {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "You Won!");
            ImGui::Text("Congratulations! You solved the puzzle!");
        }
        else
        {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Game Over!");
            ImGui::Text("No more moves possible. Try again!");
        }

        if (ImGui::Button("New Game"))
        {
            const_cast<MarbleSolitaire &>(game).reset();
            const_cast<MarbleSolitaire &>(game).startTimer();
        }

        ImGui::End();
    }
}

glm::vec2 Renderer::windowToBoard(int x, int y, const MarbleSolitaire &game)
{
    // Convert window coordinates (in pixels) to normalized device coordinates (-1 to 1)
    float ndcX = (2.0f * x) / windowWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * y) / windowHeight;

    // Scale to our board coordinate system
    float boardSize = 1.6f;
    float boardX = (ndcX + 1.0f) * 0.5f * boardSize - 0.8f;
    float boardY = (ndcY + 1.0f) * 0.5f * boardSize - 0.8f;

    return glm::vec2(boardX, boardY);
}

Position Renderer::getBoardPosition(int x, int y, const MarbleSolitaire &game)
{
    // Get board coordinates
    glm::vec2 boardCoord = windowToBoard(x, y, game);

    // Calculate cell size
    float cellSize = 1.6f / game.getBoardSize();

    // Calculate row and column
    int col = static_cast<int>((boardCoord.x + 0.8f) / cellSize);
    int row = static_cast<int>((0.8f - boardCoord.y) / cellSize);

    // Check if in valid range
    if (row >= 0 && row < game.getBoardSize() && col >= 0 && col < game.getBoardSize() &&
        game.getCell(row, col) != INVALID)
    {
        return Position(row, col);
    }

    // Return invalid position if out of range
    return Position(-1, -1);
}
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "game.h"
#include "shader.h"

class Renderer {
public:
    Renderer(int windowWidth, int windowHeight);
    ~Renderer();

    // Initialization
    void init();

    // Main rendering functions
    void renderGame(const MarbleSolitaire& game);
    void renderUI(const MarbleSolitaire& game);

    // Helper functions
    glm::vec2 windowToBoard(int x, int y, const MarbleSolitaire& game);
    Position getBoardPosition(int x, int y, const MarbleSolitaire& game);

private:
    int windowWidth, windowHeight;

    // OpenGL objects
    GLuint squareVAO, squareVBO;
    GLuint circleVAO, circleVBO;

    // Shaders
    Shader squareShader;
    Shader circleShader;

    // Initialize geometry
    void createSquare();
    void createCircle();

    // Rendering helpers
    void renderBoard(const MarbleSolitaire& game);
    void renderMarbles(const MarbleSolitaire& game);
    void renderSelection(const MarbleSolitaire& game);
    void renderGameInfo(const MarbleSolitaire& game);
};
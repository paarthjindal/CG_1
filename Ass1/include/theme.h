#ifndef THEME_H
#define THEME_H

#include <glm/glm.hpp>
#include <string>

// Theme class to store all visual styling parameters
class Theme {
public:
    // Board layout constants
    float BOARD_WIDTH = 1.6f;
    float BOARD_ORIGIN_X = -0.8f;
    float BOARD_ORIGIN_Y = 0.8f;

    // Sizing constants
    float CELL_SCALE_FACTOR = 0.9f;
    float MARBLE_SCALE_FACTOR = 0.7f;
    float MARBLE_Z_POSITION = 0.1f;

    // Colors
    glm::vec4 BACKGROUND_COLOR = glm::vec4(0.2f, 0.3f, 0.3f, 1.0f);
    glm::vec4 BOARD_COLOR = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    glm::vec4 MARBLE_COLOR = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);  // Default red
    glm::vec4 HIGHLIGHT_COLOR = glm::vec4(1.0f, 1.0f, 0.0f, 0.5f);
    glm::vec4 TEXT_COLOR = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    // Preset themes
    static Theme classicWood() {
        Theme theme;
        theme.BACKGROUND_COLOR = glm::vec4(0.12f, 0.08f, 0.05f, 1.0f);  // Dark wood
        theme.BOARD_COLOR = glm::vec4(0.76f, 0.6f, 0.42f, 1.0f);       // Light wood
        theme.MARBLE_COLOR = glm::vec4(0.2f, 0.5f, 0.9f, 1.0f);        // Blue glass
        theme.HIGHLIGHT_COLOR = glm::vec4(0.9f, 0.8f, 0.3f, 0.6f);     // Golden highlight
        return theme;
    }

    static Theme modern() {
        Theme theme;
        theme.BACKGROUND_COLOR = glm::vec4(0.12f, 0.15f, 0.18f, 1.0f); // Dark slate
        theme.BOARD_COLOR = glm::vec4(0.25f, 0.28f, 0.3f, 1.0f);       // Soft gray
        theme.MARBLE_COLOR = glm::vec4(0.18f, 0.8f, 0.7f, 1.0f);       // Teal
        theme.HIGHLIGHT_COLOR = glm::vec4(1.0f, 0.95f, 0.4f, 0.5f);    // Yellow highlight
        return theme;
    }

    static Theme royal() {
        Theme theme;
        theme.BACKGROUND_COLOR = glm::vec4(0.05f, 0.1f, 0.2f, 1.0f);   // Navy
        theme.BOARD_COLOR = glm::vec4(0.25f, 0.18f, 0.35f, 1.0f);      // Purple
        theme.MARBLE_COLOR = glm::vec4(0.9f, 0.75f, 0.1f, 1.0f);       // Gold
        theme.HIGHLIGHT_COLOR = glm::vec4(1.0f, 0.5f, 0.0f, 0.5f);     // Orange highlight
        return theme;
    }
};

// Global theme instance
extern Theme currentTheme;

#endif // THEME_H
# Marble Solitaire

A simple implementation of the Marble Solitaire game using OpenGL and ImGui.

## Game Rules
Marble Solitaire (also known as Peg Solitaire) is a board game for one player. The game starts with marbles placed on all board positions except for the center. The objective is to remove marbles by jumping over them with other marbles, leaving only one marble at the end.

- A marble can jump over an adjacent marble if there is an empty space on the other side.
- The jumped marble is removed from the board.
- The goal is to have only one marble left at the end.

## Controls
- Left-click to select a marble.
- Left-click on a valid destination to move the selected marble.
- Press 'U' to undo a move.
- Press 'R' to redo a move.
- Press 'N' to start a new game.
- Press 'ESC' to exit the game.

## Building and Running

### Using CMake
```bash
mkdir build && cd build
cmake ..
make
./marble_solitaire
```

### Using Make directly
```bash
make
./marble_solitaire
```

## Dependencies
- OpenGL
- GLEW
- GLFW3
- Dear ImGui

## Implementation Details
The game uses vertex shaders to render the board and marbles. ImGui is used for the user interface elements like buttons and text display.
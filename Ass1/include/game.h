#pragma once

#include <vector>
#include <stack>
#include <chrono>

enum CellState {
    INVALID = -1,  // Position not part of the board (corners in traditional game)
    EMPTY = 0,     // Valid position but no marble
    MARBLE = 1     // Position with a marble
};

struct Position {
    int row;
    int col;

    Position(int r = -1, int c = -1) : row(r), col(c) {}

    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }

    bool isValid() const {
        return row >= 0 && col >= 0;
    }
};

struct Move {
    Position from;
    Position to;
    Position jumped;  // The position of the jumped marble

    Move(Position f = Position(), Position t = Position(), Position j = Position())
        : from(f), to(t), jumped(j) {}
};

class MarbleSolitaire {
public:
    MarbleSolitaire(int boardSize = 7);
    ~MarbleSolitaire();

    // Game initialization
    void reset();

    // Game state
    CellState getCell(int row, int col) const;
    int getBoardSize() const { return boardSize; }
    int getRemainingMarbles() const { return remainingMarbles; }

    // Game time tracking
    void startTimer();
    int getElapsedSeconds() const;

    // Selection and movement
    void selectPosition(int row, int col);
    Position getSelectedPosition() const { return selectedPosition; }
    bool isValidSelection(int row, int col) const;
    bool isValidMove(const Position& from, const Position& to) const;
    bool makeMove(const Position& from, const Position& to);
    bool tryMove(int row, int col);  // Try to move selected marble to position
    // Process a move from one position to another
    bool makeMove(int fromRow, int fromCol, int toRow, int toCol);

    // Handle user click at position (row, col)
    bool processClick(int row, int col);

    // Undo/Redo
    bool canUndo() const { return !moveHistory.empty(); }
    bool canRedo() const { return !redoStack.empty(); }
    bool undoMove();
    bool redoMove();

    // Game state checks
    bool gameOver() const;
    bool hasWon() const;
    // Debug methods
    int countMarbles() const;
    void printBoard() const;
    void debugState() const;
    bool isValidPosition(int row, int col) const;
    bool hasValidMovesFrom(int row, int col) const;
    std::vector<Position> getValidMovesForSelected() const;

private:
    int boardSize;
    int remainingMarbles;
    std::vector<std::vector<CellState>> board;
    Position selectedPosition;
    std::stack<Move> moveHistory;
    std::stack<Move> redoStack;
    std::chrono::time_point<std::chrono::system_clock> startTime;

    bool isValidPosition(const Position& pos) const;
    Position getJumpedPosition(const Position& from, const Position& to) const;
    bool hasValidMoves() const;
    void initializeBoard();
};
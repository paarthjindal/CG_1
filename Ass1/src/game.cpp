#include "game.h"
#include <iostream>

MarbleSolitaire::MarbleSolitaire(int size) : boardSize(size), remainingMarbles(0), selectedPosition(-1, -1) {
    // Initialize board
    board.resize(boardSize, std::vector<CellState>(boardSize, INVALID));
    reset();
}

MarbleSolitaire::~MarbleSolitaire() {
    // Nothing to clean up
}

void MarbleSolitaire::reset() {
    // Clear move history and selection
    while (!moveHistory.empty()) moveHistory.pop();
    while (!redoStack.empty()) redoStack.pop();
    selectedPosition = Position(-1, -1);

    // Initialize board
    initializeBoard();

    // Calculate initial marble count
    remainingMarbles = 0;
    for (int row = 0; row < boardSize; row++) {
        for (int col = 0; col < boardSize; col++) {
            if (board[row][col] == MARBLE) {
                remainingMarbles++;
            }
        }
    }
}

void MarbleSolitaire::initializeBoard() {
    // Set up the typical cross-shaped board
    // For a standard 7x7 peg solitaire board

    // First, mark all positions as invalid
    for (int row = 0; row < boardSize; row++) {
        for (int col = 0; col < boardSize; col++) {
            board[row][col] = INVALID;
        }
    }

    // Define the valid regions (for the English-style board)
    int midPoint = boardSize / 2;

    // Top two rows
    for (int col = midPoint - 1; col <= midPoint + 1; col++) {
        for (int row = 0; row <= 1; row++) {
            board[row][col] = MARBLE;
        }
    }

    // Middle three rows
    for (int col = 0; col < boardSize; col++) {
        for (int row = midPoint - 1; row <= midPoint + 1; row++) {
            board[row][col] = MARBLE;
        }
    }

    // Bottom two rows
    for (int col = midPoint - 1; col <= midPoint + 1; col++) {
        for (int row = boardSize - 2; row <= boardSize - 1; row++) {
            board[row][col] = MARBLE;
        }
    }

    // Set center position to empty
    board[midPoint][midPoint] = EMPTY;

    // Debug output to verify board state
    std::cout << "Board initialized with " << countMarbles() << " marbles" << std::endl;
    printBoard();
}

// Add these helper functions to debug
int MarbleSolitaire::countMarbles() const {
    int count = 0;
    for (int row = 0; row < boardSize; row++) {
        for (int col = 0; col < boardSize; col++) {
            if (board[row][col] == MARBLE) {
                count++;
            }
        }
    }
    return count;
}

void MarbleSolitaire::printBoard() const {
    std::cout << "Board state:\n";
    for (int row = 0; row < boardSize; row++) {
        for (int col = 0; col < boardSize; col++) {
            if (board[row][col] == INVALID) std::cout << "X ";
            else if (board[row][col] == MARBLE) std::cout << "O ";
            else std::cout << ". ";
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}

void MarbleSolitaire::startTimer() {
    startTime = std::chrono::system_clock::now();
}

int MarbleSolitaire::getElapsedSeconds() const {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
}

CellState MarbleSolitaire::getCell(int row, int col) const {
    if (row < 0 || row >= boardSize || col < 0 || col >= boardSize) {
        return INVALID;
    }
    return board[row][col];
}

void MarbleSolitaire::selectPosition(int row, int col) {
    // Can only select positions with marbles
    if (isValidSelection(row, col)) {
        selectedPosition = Position(row, col);
    } else {
        selectedPosition = Position(-1, -1);
    }
}

bool MarbleSolitaire::isValidSelection(int row, int col) const {
    // Can only select positions within bounds and containing a marble
    return row >= 0 && row < boardSize && col >= 0 && col < boardSize && board[row][col] == MARBLE;
}

bool MarbleSolitaire::isValidPosition(const Position& pos) const {
    return pos.row >= 0 && pos.row < boardSize && pos.col >= 0 && pos.col < boardSize &&
           board[pos.row][pos.col] != INVALID;
}

bool MarbleSolitaire::isValidMove(const Position& from, const Position& to) const {
    // Check if positions are valid
    if (!isValidPosition(from) || !isValidPosition(to)) {
        return false;
    }

    // From position must have a marble
    if (board[from.row][from.col] != MARBLE) {
        return false;
    }

    // To position must be empty
    if (board[to.row][to.col] != EMPTY) {
        return false;
    }

    // Calculate distance between positions
    int rowDiff = to.row - from.row;
    int colDiff = to.col - from.col;

    // Move must be either horizontal or vertical and must jump over exactly one marble
    bool isHorizontalMove = (rowDiff == 0 && abs(colDiff) == 2);
    bool isVerticalMove = (colDiff == 0 && abs(rowDiff) == 2);

    if (!isHorizontalMove && !isVerticalMove) {
        return false;
    }

    // Check if there's a marble to jump over
    Position jumped = getJumpedPosition(from, to);

    return board[jumped.row][jumped.col] == MARBLE;
}

Position MarbleSolitaire::getJumpedPosition(const Position& from, const Position& to) const {
    // Calculate the position of the jumped marble
    return Position((from.row + to.row) / 2, (from.col + to.col) / 2);
}

bool MarbleSolitaire::makeMove(int fromRow, int fromCol, int toRow, int toCol) {
    // Debug message at the start
    std::cout << "Attempting move from (" << fromRow << "," << fromCol << ") to ("
              << toRow << "," << toCol << ")" << std::endl;

    // Validate positions
    if (!isValidPosition(fromRow, fromCol) || !isValidPosition(toRow, toCol)) {
        std::cout << "Invalid position in move" << std::endl;
        return false;
    }

    // Check if the move is valid (from has a marble, to is empty, and they're two cells apart)
    if (board[fromRow][fromCol] != MARBLE || board[toRow][toCol] != EMPTY) {
        std::cout << "Invalid move: source must have marble, destination must be empty" << std::endl;
        return false;
    }

    // Calculate the middle position
    int midRow = (fromRow + toRow) / 2;
    int midCol = (fromCol + toCol) / 2;

    // Check that we're jumping over a marble
    if (board[midRow][midCol] != MARBLE) {
        std::cout << "Invalid move: must jump over a marble" << std::endl;
        return false;
    }

    // Check that the positions are 2 cells apart in a straight line
    if (!((abs(fromRow - toRow) == 2 && fromCol == toCol) ||
          (abs(fromCol - toCol) == 2 && fromRow == toRow))) {
        std::cout << "Invalid move: must move exactly 2 spaces in a straight line" << std::endl;
        return false;
    }

    // Make the move
    board[fromRow][fromCol] = EMPTY;       // Remove marble from start
    board[midRow][midCol] = EMPTY;         // Remove jumped marble
    board[toRow][toCol] = MARBLE;          // Place marble at destination

    // Store the move in history for undo
    Move move;
    move.from = Position(fromRow, fromCol);
    move.to = Position(toRow, toCol);
    move.jumped = Position(midRow, midCol);
    moveHistory.push(move);

    // Clear redo stack since we made a new move
    while (!redoStack.empty()) {
        redoStack.pop();
    }

    // Reset selection
    selectedPosition = Position(-1, -1);

    // Decrement remaining marbles count
    remainingMarbles--;

    // Debug: print board after move
    std::cout << "Move completed successfully! Updated board:" << std::endl;
    printBoard();
    std::cout << "Remaining marbles: " << remainingMarbles << std::endl;

    return true;
}
// Add to the MarbleSolitaire class implementation
void MarbleSolitaire::debugState() const {
    std::cout << "===== GAME STATE DEBUG =====" << std::endl;
    std::cout << "Board size: " << boardSize << std::endl;
    std::cout << "Remaining marbles: " << remainingMarbles << std::endl;
    std::cout << "Selected position: ";
    if (selectedPosition.isValid()) {
        std::cout << "(" << selectedPosition.row << ", " << selectedPosition.col << ")";
    } else {
        std::cout << "None";
    }
    std::cout << std::endl;
    std::cout << "Moves in history: " << moveHistory.size() << std::endl;
    std::cout << "Redo stack size: " << redoStack.size() << std::endl;
    printBoard();
    std::cout << "==========================" << std::endl;
}



bool MarbleSolitaire::isValidPosition(int row, int col) const {
    return row >= 0 && row < boardSize && col >= 0 && col < boardSize &&
           board[row][col] != INVALID;
}

// Helper to check if there are valid moves for a specific position
bool MarbleSolitaire::hasValidMovesFrom(int row, int col) const {
    if (board[row][col] != MARBLE) return false;

    // Check all four directions
    const int dr[] = {-2, 2, 0, 0};
    const int dc[] = {0, 0, -2, 2};

    for (int i = 0; i < 4; i++) {
        int newRow = row + dr[i];
        int newCol = col + dc[i];
        Position from(row, col);
        Position to(newRow, newCol);

        if (isValidMove(from, to)) {
            return true;
        }
    }
    return false;
}

// Highlight valid moves for the selected marble
std::vector<Position> MarbleSolitaire::getValidMovesForSelected() const {
    std::vector<Position> validMoves;

    if (!selectedPosition.isValid()) {
        return validMoves;  // No marble selected
    }

    int row = selectedPosition.row;
    int col = selectedPosition.col;

    // Check all four directions
    const int dr[] = {-2, 2, 0, 0};
    const int dc[] = {0, 0, -2, 2};

    for (int i = 0; i < 4; i++) {
        int newRow = row + dr[i];
        int newCol = col + dc[i];
        Position to(newRow, newCol);

        if (isValidMove(selectedPosition, to)) {
            validMoves.push_back(to);
        }
    }

    return validMoves;
}
// Also update the processClick function
bool MarbleSolitaire::processClick(int row, int col) {
    std::cout << "Processing click at position (" << row << "," << col << ")" << std::endl;

    // Check if clicked on an invalid position
    if (!isValidPosition(row, col)) {
        std::cout << "Invalid position clicked" << std::endl;
        return false;
    }

    // If no marble is selected yet
    if (!selectedPosition.isValid()) {
        // Can only select positions with marbles
        if (board[row][col] == MARBLE) {
            selectedPosition = Position(row, col);
            std::cout << "Selected marble at (" << row << "," << col << ")" << std::endl;
            return true;
        } else {
            std::cout << "Cannot select empty or invalid position" << std::endl;
            return false;
        }
    }
    // If a marble is already selected
    else {
        // If clicked on the same marble, deselect it
        if (row == selectedPosition.row && col == selectedPosition.col) {
            selectedPosition = Position(-1, -1);
            std::cout << "Deselected marble" << std::endl;
            return true;
        }
        // If clicked on another marble, select that one instead
        else if (board[row][col] == MARBLE) {
            selectedPosition = Position(row, col);
            std::cout << "Selected new marble at (" << row << "," << col << ")" << std::endl;
            return true;
        }
        // If clicked on an empty space, attempt to move there
        else if (board[row][col] == EMPTY) {
            bool moveSuccessful = makeMove(selectedPosition.row, selectedPosition.col, row, col);
            if (moveSuccessful) {
                std::cout << "Move successful" << std::endl;
            } else {
                std::cout << "Invalid move attempted" << std::endl;
            }
            return moveSuccessful;
        }
    }

    return false;
}
bool MarbleSolitaire::makeMove(const Position& from, const Position& to) {
    return makeMove(from.row, from.col, to.row, to.col);
    // if (!isValidMove(from, to)) {
    //     return false;
    // }

    // // Get jumped position
    // Position jumped = getJumpedPosition(from, to);

    // // Move the marble
    // board[from.row][from.col] = EMPTY;
    // board[to.row][to.col] = MARBLE;
    // board[jumped.row][jumped.col] = EMPTY;

    // // Create move record
    // Move move(from, to, jumped);

    // // Update move history
    // moveHistory.push(move);

    // // Clear redo stack when a new move is made
    // while (!redoStack.empty()) {
    //     redoStack.pop();
    // }

    // // Update remaining marbles
    // remainingMarbles--;

    // // Clear selection
    // selectedPosition = Position(-1, -1);

    // return true;
}

bool MarbleSolitaire::tryMove(int row, int col) {
    // If no marble is selected, can't move
    if (!selectedPosition.isValid()) {
        return false;
    }

    Position target(row, col);
    return makeMove(selectedPosition, target);
}

bool MarbleSolitaire::undoMove() {
    if (moveHistory.empty()) {
        std::cout << "No moves to undo" << std::endl;
        return false;
    }

    // Get the last move
    Move lastMove = moveHistory.top();
    moveHistory.pop();

    // Restore the board state
    board[lastMove.from.row][lastMove.from.col] = MARBLE;       // Put marble back at start
    board[lastMove.to.row][lastMove.to.col] = EMPTY;            // Remove marble from destination
    board[lastMove.jumped.row][lastMove.jumped.col] = MARBLE;   // Restore jumped marble

    // Add to redo stack
    redoStack.push(lastMove);

    // Update remaining marbles
    remainingMarbles++;

    // Clear selection
    selectedPosition = Position(-1, -1);

    // Print board after undo - ADD THIS
    std::cout << "\n=== BOARD AFTER UNDO ===\n";
    printBoard();
    std::cout << "Remaining marbles: " << remainingMarbles << std::endl;
    std::cout << "======================\n" << std::endl;

    return true;
}

bool MarbleSolitaire::redoMove() {
    if (redoStack.empty()) {
        std::cout << "No moves to redo" << std::endl;
        return false;
    }

    // Get the move to redo
    Move redoMove = redoStack.top();
    redoStack.pop();

    // Apply the move again
    board[redoMove.from.row][redoMove.from.col] = EMPTY;        // Remove marble from start
    board[redoMove.to.row][redoMove.to.col] = MARBLE;           // Put marble at destination
    board[redoMove.jumped.row][redoMove.jumped.col] = EMPTY;    // Remove jumped marble

    // Add to history
    moveHistory.push(redoMove);

    // Update remaining marbles
    remainingMarbles--;

    // Clear selection
    selectedPosition = Position(-1, -1);

    // Print board after redo - ADD THIS
    std::cout << "\n=== BOARD AFTER REDO ===\n";
    printBoard();
    std::cout << "Remaining marbles: " << remainingMarbles << std::endl;
    std::cout << "======================\n" << std::endl;

    return true;
}
bool MarbleSolitaire::gameOver() const {
    // Game is over if there are no valid moves left
    return !hasValidMoves();
}

bool MarbleSolitaire::hasValidMoves() const {
    // Check if any marble can make a valid move
    for (int row = 0; row < boardSize; row++) {
        for (int col = 0; col < boardSize; col++) {
            if (board[row][col] == MARBLE) {
                Position from(row, col);

                // Check all four possible jump directions
                Position directions[4] = {
                    Position(-2, 0),  // Up
                    Position(2, 0),   // Down
                    Position(0, -2),  // Left
                    Position(0, 2)    // Right
                };

                for (const auto& dir : directions) {
                    Position to(row + dir.row, col + dir.col);
                    if (isValidMove(from, to)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool MarbleSolitaire::hasWon() const {
    return remainingMarbles == 1;
}
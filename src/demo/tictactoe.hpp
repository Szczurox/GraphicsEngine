// Tic-tac-toe vs AI demo based on https://github.com/Szczurox/TicTacToe-Minimax
// Interact with the board by clicking on the free spaces
// Press "R" to restart
// Press "F11" to toggle the fullscreen mode
// Press "Esc" to exit the demo

#ifndef TICTACTOE_DEMO
#define TICTACTOE_DEMO

#include "../GraphicsEngine.hpp"

// Global variables
bool running = true;
bool restart = false;
GraphicsEngine e;

// Move value structure
struct MoveValue {
	// Move score
	int score;
	// Start and end points of the line marking the winning condition
	vec2<int> lineBegin;
	vec2<int> lineEnd;

	// MoveValue constructor for non-winning condition
	MoveValue(int score) : score(score), lineBegin(vec2<int>()), lineEnd(vec2<int>()) {}
	// MoveValue constructor used in the final winning condition
	MoveValue(int score, vec2<int> winSquareFirst, vec2<int> winSquareLast) : score(score) {
		lineBegin = vec2<int>(150 + winSquareFirst.y * 300, 150 + winSquareFirst.x * 300);
		lineEnd = vec2<int>(150 + winSquareLast.y * 300, 150 + winSquareLast.x * 300);
	}
};

// Move structure
// Used to pack the row, the column and the move value together
struct Move {
	int row, col;
	MoveValue moveVal;
	Move() : row(-1), col(-1), moveVal(-1000) {}
};

// Checks if there are any free spaces left on the board
bool isMovesLeft(int board[3][3]) {
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			if (board[i][j] == 0)
				return true;
	return false;
}

// Evaluates the current board state
// Returns player type * 10 (-10 for O and 10 for X) if someone won
// Else returns 0
// Also returns on which line on the board player has won
MoveValue evaluate(int b[3][3]) {
	// One of the players won horizontally
	for (short row = 0; row < 3; row++)
		if (b[row][0] == b[row][1] && b[row][1] == b[row][2])
			if (b[row][0] != 0)
				return MoveValue(b[row][0] * 10, vec2<int>(row, 0), vec2<int>(row, 2));

	// One of the players won vertically
	for (short col = 0; col < 3; col++)
		if (b[0][col] == b[1][col] && b[1][col] == b[2][col])
			if (b[0][col] != 0)
				return MoveValue(b[0][col] * 10, vec2<int>(0, col), vec2<int>(2, col));

	// One of the players won diagonally
	if (b[0][2] == b[1][1] && b[1][1] == b[2][0])
		if (b[1][1] != 0)
			return MoveValue(b[1][1] * 10, vec2<int>(0, 2), vec2<int>(2, 0));

	if (b[0][0] == b[1][1] && b[1][1] == b[2][2])
		if (b[1][1] != 0)
			return MoveValue(b[1][1] * 10, vec2<int>(0, 0), vec2<int>(2, 2));

	// Nobody won
	return MoveValue(0);
}

// Goes through all the cells, makes a move, checks the results
// With condition passed as a function
template <typename Proc>
int traverseCells(int board[3][3], bool isMax, Proc check) {
	int best = -1000;
	int move = 1; // Move type of the board matrix (1 == X, -1 == O)
	if (!isMax) {
		best = 1000;
		move = -1;
	}

	// Iterate through all the cells
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			// Check if the cell is empty
			if (board[i][j] == 0) {
				// Make the move
				board[i][j] = move;

				// Check results of the move using the passed function
				best = check(best);

				// Undo the move
				board[i][j] = 0;
			}

	return best;
}

// Minimax algorythm
int minimax(int board[3][3], int depth, bool isMax) {
	int score = evaluate(board).score;

	// If one of the players has won the game
	if (score == 10) {
		return score -= depth;
	}
	else if (score == -10) {
		return score += depth;
	}

	// If there are no moves left and no winner
	if (isMovesLeft(board) == false)
		return 0;

	// Maximizer's move
	if (isMax)
		return traverseCells(board, isMax, [board, depth, isMax](int best) {
		// Chose the greater from the current best and the new move
		int result = minimax(board, depth + 1, false);
		if (best > result)
			return best;
		return result;
			});
	// Minimizer's score
	return traverseCells(board, isMax, [board, depth, isMax](int best) {
		// Chose the smaller from the current best and the new move
		int result = minimax(board, depth + 1, true);
		if (best < result)
			return best;
		return result;
		});
}


// Returns the best possible move
Move findBestMove(int board[3][3], bool& isMax) {
	Move bestMove; // The best move
	bestMove.row = -1;
	bestMove.col = -1;
	int move = 1;   // Move type of the board matrix (1 == X, -1 == O)
	if (!isMax) {
		bestMove.moveVal = 1000;
		move = -1;
	}

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			// Check if the cell is empty
			if (board[i][j] == 0) {
				// Make a move
				board[i][j] = move;

				// Compute evaluation function for the move
				int moveVal = minimax(board, 0, !isMax);

				// Undo the move
				board[i][j] = 0;

				// Check if new move is better than the previous best move
				if ((isMax && moveVal > bestMove.moveVal.score) || (!isMax && moveVal < bestMove.moveVal.score)) {
					// Update best move
					bestMove.row = i;
					bestMove.col = j;
					bestMove.moveVal = moveVal;
				}
			}

	bestMove.moveVal = evaluate(board);

	// Set the turn to the other player
	isMax = !isMax;
	return bestMove;
}

// Gets move from player and returns it as a Move
Move getPlayerMove(int board[3][3], Rect squares[3][3], bool& playerTurn) {
	Move playerMove;

	// Check hitbox for each square
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			if (squares[i][j].isPointInside(vec2<int>(e.mouseX, e.mouseY)) && e.lbClick)
				if (board[i][j] == 0) {
					playerMove.row = i;
					playerMove.col = j;
					playerTurn = !playerTurn;
					break;
				}

	playerMove.moveVal = evaluate(board);

	return playerMove;
}

// Draws a cross
void drawCross(vec2<int> origin, int width, UINT32 color, unsigned short thickness = 1) {
	int halfWidth = width / 2;
	e.drawLine(vec2<int>(origin.x - halfWidth, origin.y - halfWidth),
		vec2<int>(origin.x + halfWidth, origin.y + halfWidth), color, thickness);
	e.drawLine(vec2<int>(origin.x - halfWidth, origin.y + halfWidth),
		vec2<int>(origin.x + halfWidth, origin.y - halfWidth), color, thickness);
}

void drawBoard(int board[3][3]) {
	// Clear screen
	e.clearScreen(0x333333);
	// Board
	e.drawLine(vec2<int>(300, 0), vec2<int>(300, 900), BLACK, 10);
	e.drawLine(vec2<int>(600, 0), vec2<int>(600, 900), BLACK, 10);
	e.drawLine(vec2<int>(0, 300), vec2<int>(900, 300), BLACK, 10);
	e.drawLine(vec2<int>(0, 600), vec2<int>(900, 600), BLACK, 10);

	// For each row
	for (int i = 0; i < 3; i++) {
		// For each column
		for (int j = 0; j < 3; j++) {
			// Draw X for 1, O for -1 and empty space for 0
			if (board[i][j] == 1)
				drawCross(vec2<int>(150 + j * 300, 150 + i * 300), 240, BLACK, 5);
			else if (board[i][j] == -1)
				e.drawEmptyCircle(vec2<int>(150 + j * 300, 150 + i * 300), 120, BLACK, 5);
		}
	}
}


// Main window function
int TicTacToeDemoMain(_In_ HINSTANCE curInst, _In_opt_ HINSTANCE prevInst, _In_ PSTR cmdLine, _In_ INT cmdCount) {
	// Game variables
	bool playerTurn = true;
	bool gameOver = false;
	MoveValue boardValue = 0;

	// Board matrix (0 == empty, 1 == X, -1 == O)
	int board[3][3] = {
		{ 0, 0, 0 },
		{ 0, 0, 0 },
		{ 0, 0, 0 }
	};

	e.createWindow(curInst, 900, 900);

	// Array of hitboxes of the board spaces
	Rect squares[3][3];

	// Generate hitbox for each space
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			squares[i][j] = Rect(vec2<int>(10 + j * 300, 10 + i * 300), vec2<int>(290 + j * 300, 290 + i * 300));

	// Main program loop
	while (running) {
		e.handleMessages();

		if (restart) {
			// Reset the board matrix
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
					board[i][j] = 0;

			// Revert game variables back to default
			boardValue = 0;
			playerTurn = true;
			gameOver = false;
			// Finish restart
			restart = false;
		}

		// Main game loop
		if (!gameOver) {
			drawBoard(board);

			// Check if any of the players won or game ended in a draw
			if (boardValue.score == 10 || boardValue.score == -10) {
				// Draw a line that marks the winner
				e.drawLine(boardValue.lineBegin, boardValue.lineEnd, RED, 10);
				gameOver = true;
			}
			else if (!isMovesLeft(board)) {
				gameOver = true;
			}

			// Player
			if (playerTurn) {
				// Get the move, set the move, set the new board value
				Move move = getPlayerMove(board, squares, playerTurn);
				board[move.row][move.col] = 1;
				boardValue = move.moveVal;
			}
			// AI
			else {
				// Get the move, set the move, set the new board value
				Move move = findBestMove(board, playerTurn);
				board[move.row][move.col] = -1;
				boardValue = move.moveVal;
			}
		}

		e.mainLoopEndEvents();
	}

	return 0;
}

// Processes the messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		case VK_F11:
			e.toggleFullscreen();
			break;
		case 0x52:
			restart = true;
			break;
		}
		break;
	}
	return e.processMessage(hwnd, msg, wParam, lParam, running);
}

#endif
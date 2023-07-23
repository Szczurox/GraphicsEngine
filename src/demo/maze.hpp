// DEMO IS NOT YET FINISHED

#ifndef MAZE_DEMO
#define MAZE_DEMO

#include "../GraphicsEngine.hpp"
#include<stack>

bool running = true;
GraphicsEngine e;

enum {
	PATH_N = 0x01,
	PATH_E = 0x02,
	PATH_S = 0x04,
	PATH_W = 0x08,
	VISITED = 0x10,
};

int MazeDemoMain(_In_ HINSTANCE curInst, _In_opt_ HINSTANCE prevInst, _In_ PSTR cmdLine, _In_ INT cmdCount) {
	unsigned int windowWidth = 900;
	unsigned int windowHeight = 900;
	unsigned int mazeWidth = 18;
	unsigned int mazeHeight = 18;
	unsigned int coordMultW = windowWidth / mazeWidth;
	unsigned int coordMultH = windowHeight / mazeHeight;
	int *maze = new int[mazeWidth * mazeHeight];
	int visitedCells;
	std::stack<std::pair<int, int>> stack;

	memset(maze, 0x00, mazeWidth * mazeHeight);

	stack.push(std::make_pair(0, 0));
	maze[0] = VISITED;
	visitedCells = 1;

	e.createWindow(curInst, 900, 900);

	// Clear screen
	e.clearScreen(0x333333);

	for (int x = 0; x < mazeWidth; x++) {
		for (int y = 0; y < mazeHeight; y++) {
			COLOR curColor;

			if (maze[y * mazeWidth + x] & VISITED)
				curColor = WHITE;
			else 
				curColor = GREY;

			if (maze[y * mazeWidth + x] & PATH_S)
				e.drawRectangle(vec2<int>(x * coordMultW, y * coordMultH), 8 * coordMultW / 10, coordMultH, curColor);
			if (maze[y * mazeWidth + x] & PATH_E)
				e.drawRectangle(vec2<int>(x * coordMultW, y * coordMultH), coordMultW, 8 * coordMultH / 10, curColor);
			else 
				e.drawRectangle(vec2<int>(x * coordMultW, y * coordMultH), 8 * coordMultW / 10, 8 * coordMultH / 10, curColor);
		}
	}

	// Main program loop
	while (running) {
		e.handleMessages();

		e.mainLoopEndEvents();
	}

	return 0;
}

// Processes the messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	return e.processMessage(hwnd, msg, wParam, lParam);
}

#endif
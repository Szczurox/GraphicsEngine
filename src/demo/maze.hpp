#ifndef MAZE_DEMO
#define MAZE_DEMO

#include "../GraphicsEngine.hpp"

bool running = true;
GraphicsEngine e;

struct Tile {
	Rect rect;
	bool isOn;

	Tile() : rect(Rect()), isOn(false) {}
	Tile(Rect rect) : rect(rect), isOn(false) {}
	Tile(Rect rect, bool isOn) : rect(rect), isOn(isOn) {}
};

int MazeDemoMain(_In_ HINSTANCE curInst, _In_opt_ HINSTANCE prevInst, _In_ PSTR cmdLine, _In_ INT cmdCount) {
	// It will crash if window ratio is different from tiles ratio
	const int windowWidth = 900;
	const int windowHeight = 900;
	const int width = 10;
	const int height = 10;

	const int ratioW = windowWidth / width;
	const int ratioH = windowHeight / height;

	e.createWindow(curInst, windowWidth, windowHeight);

	Tile tiles[height][width];

	// Generate hitbox for each space
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			vec2<int> min = vec2<int>(2 + j * ratioW, 2 + i * ratioH);
			vec2<int> max = vec2<int>((ratioW - 2) + j * ratioW, (ratioW - 2) + i * ratioH);
			tiles[i][j] = Rect(min, max);
		};
	

	// Clear screen
	e.clearScreen(BLACK);

	for (int h = 0; h < height; h++)
		for (int w = 0; w < width; w++)
			e.drawRectangle(tiles[h][w].rect, 0x333333);

	// Main program loop
	while (running) {
		e.handleMessages();

		if (e.rbClick) {
			for (int h = 0; h < height; h++)
				for (int w = 0; w < width; w++)
					if (tiles[h][w].rect.isPointInside(vec2<int>(e.mouseX, e.mouseY))) {
						Tile& tile = tiles[h][w];
						if (!tile.isOn) {
							e.drawRectangle(tiles[h][w].rect, RED);
							tile.isOn = true;
						}
						else { 
							e.drawRectangle(tiles[h][w].rect, 0x333333);
							tile.isOn = false;
						}
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
		}
	}

	return e.processMessage(hwnd, msg, wParam, lParam, running);
}

#endif
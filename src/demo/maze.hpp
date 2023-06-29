// A-Star path finding

#ifndef MAZE_DEMO
#define MAZE_DEMO

#include "../GraphicsEngine.hpp"
#include<vector>

bool running = true;
GraphicsEngine e;

struct Tile {
	Rect rect;
	bool isObstacle = false;
	bool isVisited = false;
	float globalGoal = 0;
	float localGoal = 0;
	vec2<int> coords = vec2<int>();
	std::vector<Tile*> neighbours = {};
	Tile* parent = nullptr;

	Tile() : rect(Rect()) {}
	Tile(Rect rect) : rect(rect) {}
	Tile(Rect rect, vec2<int> coords) : rect(rect), coords(coords) {}
};

int MazeDemoMain(_In_ HINSTANCE curInst, _In_opt_ HINSTANCE prevInst, _In_ PSTR cmdLine, _In_ INT cmdCount) {
	// It will crash if window ratio is different from tiles ratio
	const int windowWidth = 900;
	const int windowHeight = 900;
	const int tilesWidth = 16;
	const int tilesHeight = 16;

	const int ratioW = windowWidth / tilesWidth;
	const int ratioH = windowHeight / tilesHeight;

	const int gap = 3;  // Gap between tiles (in pixels)

	e.createWindow(curInst, windowWidth, windowHeight);

	Tile tiles[tilesHeight][tilesWidth];

	Tile* tileStart;
	Tile* tileEnd;

	// Generate tiles
	for (int i = 0; i < tilesHeight; i++)
		for (int j = 0; j < tilesWidth; j++) {
			vec2<int> min = vec2<int>(gap + j * ratioW, gap + i * ratioH);
			vec2<int> max = vec2<int>((ratioW - gap) + j * ratioW, (ratioW - gap) + i * ratioH);
			tiles[i][j] = Tile(Rect(min, max), vec2<int>(j, i));
		};
	
	tileStart = &tiles[0][0];
	tileEnd = &tiles[tilesHeight - 1][tilesWidth - 1];

	// Create connections between tiles 
	for (int y = 0; y < tilesHeight; y++)
		for (int x = 0; x < tilesWidth; x++) {
			if(x < tilesWidth - 1)
				tiles[y][x].neighbours.push_back(&tiles[y][x + 1]);
			if(x > 0)
				tiles[y][x].neighbours.push_back(&tiles[y][x - 1]);
			if(y < tilesHeight - 1)
				tiles[y][x].neighbours.push_back(&tiles[y + 1][x]);
			if (y > 0)
				tiles[y][x].neighbours.push_back(&tiles[y - 1][x]);
		};
	

	// Clear screen
	e.clearScreen(BLACK);

	// Draw tiles
	for (int h = 0; h < tilesHeight; h++)
		for (int w = 0; w < tilesWidth; w++) {
			Tile tile = tiles[h][w];
			// Lines connectiong each tile with it's neighbours
			for (Tile* n : tile.neighbours)
				e.drawLine(vec2<int>(tile.rect.minPoint.x + tile.rect.width / 2, tile.rect.minPoint.y + tile.rect.height / 2),
					vec2<int>(n->rect.minPoint.x + n->rect.width / 2, n->rect.minPoint.y + n->rect.height / 2), 0x333333);
			// Draw the tile
			e.drawRectangle(tiles[h][w].rect, 0x333333);
		}

	e.drawRectangle(tileStart->rect, 0x3333C1);
	e.drawRectangle(tileEnd->rect, 0x00C100);

	bool fullscreenHeld = false;

	// Main program loop
	while (running) {
		e.handleMessages();

		if (e.keys[VK_ESCAPE].isHeld) {
			e.destroy();
		}

		if (e.keys[VK_F11].isPressed && !fullscreenHeld) {
			e.toggleFullscreen();
			fullscreenHeld = true;
		}
		else if (!e.keys[VK_F11].isPressed)
			fullscreenHeld = false;

		// On right button click
		if (e.rbClick) {
			// Update tile that got clicked to obstacle
			for (int h = 0; h < tilesHeight; h++)
				for (int w = 0; w < tilesWidth; w++)
					if (tiles[h][w].rect.isPointInside(vec2<int>(e.mouseX, e.mouseY))) {
							Tile& tile = tiles[h][w];
							if (&tile != tileStart && &tile != tileEnd) {
								if (!tile.isObstacle) {
									e.drawRectangle(tiles[h][w].rect, 0x111111);
									tile.isObstacle = true;
								}
								else {
									e.drawRectangle(tiles[h][w].rect, 0x333333);
									tile.isObstacle = false;
								}
							}
					}
		}

		e.mainLoopEndEvents();
	}

	return 0;
}

// Processes the messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	return e.processMessage(hwnd, msg, wParam, lParam, running);
}

#endif
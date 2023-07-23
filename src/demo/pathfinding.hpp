//
// A-Star path finding
// 
// Left Click to place obstacles
// Left Click while holding "Shift" / "Ctrl" to change Starting Location / Target Location respectively
// Press "P" to change path finding type (best possible / reasonably good)
// 
// Blue tile marks Starting Location
// Green tile marks Target Location
// Yellow squares mark the final path
// Dark grey tiles marks obstacles
// Light grey tiles mark empty tiles
// Slightly darker grey marks tiles checked by the algorythm
//
// Solution updates every time you place an obstacle or change position of start / end points
//

#ifndef PATH_DEMO
#define PATH_DEMO

#include "../GraphicsEngine.hpp"
#include<vector>
#include<list>

bool running = true;
GraphicsEngine e;

// It will crash if window ratio is different from tiles ratio
const int windowWidth = 900;
const int windowHeight = 900;
const int tilesWidth = 16;
const int tilesHeight = 16;

struct Tile {
	Rect rect;
	bool isObstacle = false;
	bool isVisited = false;
	float globalGoal = INFINITY;
	float localGoal = INFINITY;
	vec2<int> coords = vec2<int>();
	std::vector<Tile*> neighbours = {};
	Tile* parent = nullptr;

	Tile() : rect(Rect()) {}
	Tile(Rect rect) : rect(rect) {}
	Tile(Rect rect, vec2<int> coords) : rect(rect), coords(coords) {}
};

void solve(std::vector<std::vector<Tile>>& tiles, Tile* tileStart, Tile* tileEnd, bool bestPath = true) {
	// Distance between 2 tiles
	auto dist = [](Tile* a, Tile* b) {
		return sqrtf((a->coords.x - b->coords.x) * (a->coords.x - b->coords.x) + (a->coords.y - b->coords.y) * (a->coords.y - b->coords.y));
	};

	for (int y = 0; y < tilesHeight; y++)
		for (int x = 0; x < tilesWidth; x++) {
			tiles[y][x].isVisited = false;
			tiles[y][x].globalGoal = INFINITY;
			tiles[y][x].localGoal = INFINITY;
			tiles[y][x].parent = nullptr;
		}

	// Set current node to the starting node and set heuristic to distance between start and end
	Tile* tileCurrent = tileStart;
	tileStart->localGoal = 0.0f;
	tileStart->globalGoal = dist(tileStart, tileEnd);

	// All newly discovered tiles go there to ensure that they get tested
	std::list<Tile*> notTestedTiles;
	notTestedTiles.push_back(tileStart);

	while (!notTestedTiles.empty()) {
		// Sort untested tiles by global goal, so that the lowest is first
		notTestedTiles.sort([](Tile* a, Tile* b) { return a->globalGoal < b->globalGoal; });

		// Remove already visited nodes from the front of the list (if there are any)
		while (!notTestedTiles.empty() && notTestedTiles.front()->isVisited)
			notTestedTiles.pop_front();

		// Abort if there are no nodes remaining
		if (notTestedTiles.empty() || (tileCurrent == tileEnd && !bestPath))
			break;
		
		// Explore first node from the list
		tileCurrent = notTestedTiles.front();
		tileCurrent->isVisited = true;

		// Check new current node's neighbours
		for (Tile* neighbour : tileCurrent->neighbours) {
			// Only add neighbours if not visited and not obstacle
			if(!neighbour->isVisited && !neighbour->isObstacle)
				notTestedTiles.push_back(neighbour);

			float possibleLowerGoal = tileCurrent->localGoal + dist(tileCurrent, neighbour);

			// If current node's distance is lower use it as parent
			if (possibleLowerGoal < neighbour->localGoal) {
				neighbour->parent = tileCurrent;
				neighbour->localGoal = possibleLowerGoal;
				// Set neighbour's heuristic to the distance between the neighbour and the end
				neighbour->globalGoal = neighbour->localGoal + dist(neighbour, tileEnd);
			}
		}
	}

	return;
}

int PathDemoMain(_In_ HINSTANCE curInst, _In_opt_ HINSTANCE prevInst, _In_ PSTR cmdLine, _In_ INT cmdCount) {
	const int ratioW = windowWidth / tilesWidth;
	const int ratioH = windowHeight / tilesHeight;

	const int gap = 3;  // Gap between tiles (in pixels)

	// Does the path need to be the shortest or can it be reasonably short
	bool bestPath = true;

	e.createWindow(curInst, windowWidth, windowHeight);

	std::vector<std::vector<Tile>> tiles(windowHeight, std::vector<Tile>(windowWidth, Tile()));

	Tile* tileStart;
	Tile* tileEnd;

	// Generate tiles
	for (int i = 0; i < tilesHeight; i++) 
		for (int j = 0; j < tilesWidth; j++) {
			vec2<int> min = vec2<int>(gap + j * ratioW, gap + i * ratioH);
			vec2<int> max = vec2<int>((ratioW - gap) + j * ratioW, (ratioW - gap) + i * ratioH);
			tiles[i][j] = (Tile(Rect(min, max), vec2<int>(j, i)));
		};
	
	tileStart = &tiles[0][0];
	tileEnd = &tiles[tilesHeight - 1][tilesWidth - 1];

	// Create connections between tiles 
	for (unsigned int y = 0; y < tilesHeight; y++)
		for (unsigned int x = 0; x < tilesWidth; x++) {
			if(x < tilesWidth - 1)
				tiles[y][x].neighbours.push_back(&tiles[y][x + (unsigned int)1]);
			if(x > 0)
				tiles[y][x].neighbours.push_back(&tiles[y][x - (unsigned int)1]);
			if(y < tilesHeight - 1)
				tiles[y][x].neighbours.push_back(&tiles[y + (unsigned int)1][x]);
			if (y > 0)
				tiles[y][x].neighbours.push_back(&tiles[y - (unsigned int)1][x]);
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
	bool bestPathHeld = false;

	// Main program loop
	while (running) {
		e.handleMessages();

		// Handle keyboard events
		if (e.keys[VK_ESCAPE].isHeld)
			e.destroy();

		if (e.keys[VK_F11].isHeld && !fullscreenHeld) {
			e.toggleFullscreen();
			fullscreenHeld = true;
		}
		else if (!e.keys[VK_F11].isHeld)
			fullscreenHeld = false;

		// P to toggle path finding type
		if (e.keys[0x50].isHeld && !bestPathHeld) {
			bestPath = !bestPath;
			bestPathHeld = true;
		}
		else if (!e.keys[0x50].isHeld)
			bestPathHeld = false;


		// On right button click
		if (e.lbClick) {
			// Update tile that got clicked to obstacle
			for (int h = 0; h < tilesHeight; h++)
				for (int w = 0; w < tilesWidth; w++) {
					Tile& tile = tiles[h][w];
					if (tile.rect.isPointInside(vec2<int>(e.mouseX, e.mouseY))) {
						if (e.keys[VK_SHIFT].isHeld) {
							e.drawRectangle(tileStart->rect, 0x333333);
							tile.isObstacle = false;
							tileStart = &tile;
							e.drawRectangle(tileStart->rect, 0x3333C1);
						}
						else if (e.keys[VK_CONTROL].isHeld) {
							e.drawRectangle(tileEnd->rect, 0x333333);
							tile.isObstacle = false;
							tileEnd = &tile;
							e.drawRectangle(tileEnd->rect, 0x00C100);
						}
						else if (&tile != tileEnd && &tile != tileStart)
							if (!tile.isObstacle) {
								e.drawRectangle(tile.rect, 0x111111);
								tile.isObstacle = true;
								tile.isVisited = false;
							}
							else {
								e.drawRectangle(tile.rect, 0x333333);
								tile.isObstacle = false;
							}
					}
				}

			solve(tiles, tileStart, tileEnd, bestPath);

			for (int h = 0; h < tilesHeight; h++)
				for (int w = 0; w < tilesWidth; w++) {
					Tile& tile = tiles[h][w];
					if (&tile != tileEnd && &tile != tileStart) {
						if (tile.isObstacle) e.drawRectangle(tile.rect, 0x111111);
						else if (tile.isVisited) e.drawRectangle(tile.rect, 0x262626);
						else e.drawRectangle(tile.rect, 0x333333);
					}
				}

			if (tileEnd->parent != nullptr) {
				Tile* path = tileEnd;
				while (path->parent != nullptr) {
					if(path != tileEnd)
						e.drawRectangle(path->rect, 0xA97700);
					path = path->parent;
				}
			}

		}

		e.mainLoopEndEvents();
	}

	return 0;
}

// Processes the messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	return e.processMessage(hwnd, msg, wParam, lParam);
}

#endif
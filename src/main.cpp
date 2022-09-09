#include <windows.h>
#include <stdint.h>
#include "GraphicsEngine.hpp"

typedef uint32_t u32;

bool running = true;

GraphicsEngine e;

// Main window function
int WINAPI WinMain(_In_ HINSTANCE curInst, _In_opt_ HINSTANCE prevInst, _In_ PSTR cmdLine, _In_ INT cmdCount) {
	int pinkCircle = false;
	int yellowCircle = false;

	e.createWindow(curInst, 900, 900);

	Rect button(vec2<int>(700, 600), vec2<int>(600, 700));

	// Main program loop
	while (running) {
		e.handleMessages();
		e.clearScreen(0x333333);
		// Board
		e.drawLine(vec2<int>(300, 0), vec2<int>(300, 900), BLACK, 10);
		e.drawLine(vec2<int>(600, 0), vec2<int>(600, 900), BLACK, 10);
		e.drawLine(vec2<int>(0, 300), vec2<int>(900, 300), BLACK, 10);
		e.drawLine(vec2<int>(0, 600), vec2<int>(900, 600), BLACK, 10);

		e.drawRectangle(vec2<int>(600, 600), 100, 100, BLUE);
		e.drawTriangle(vec2<int>(200, 300), vec2<int>(300, 400), vec2<int>(150, 500), GREEN);
		if (button.isPointInside(vec2<int>(e.mouseX, e.mouseY)) && e.lbClick) {
			pinkCircle = false;
			yellowCircle = !yellowCircle;
		}
		if (button.isPointInside(vec2<int>(e.mouseX, e.mouseY)) && e.rbClick) {
			yellowCircle = false;
			pinkCircle = !pinkCircle;
		}
		if (pinkCircle)
			e.drawEmptyCircle(vec2<int>(500, 300), 60, PINK, 2);
		else if (yellowCircle)
			e.drawEmptyCircle(vec2<int>(500, 300), 60, YELLOW, 2);
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
		case VK_F11:
			e.toggleFullscreen();
		}
		break;
	}
	return e.processMessage(hwnd, msg, wParam, lParam, running);
}
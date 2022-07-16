#include <windows.h>
#include <stdint.h>
#include "GraphicsEngine.hpp"

typedef uint32_t u32;

bool running = true;

GraphicsEngine e;

// Main window function
int WINAPI WinMain(_In_ HINSTANCE curInst, _In_opt_ HINSTANCE prevInst, _In_ PSTR cmdLine, _In_ INT cmdCount) {
	e.createWindow(curInst, 800, 800);

	// Main program loop
	while (running) {
		e.handleMessages();
		e.clearScreen(0x333333);
		e.drawLine(vec2<int>(100, 100), vec2<int>(200, 200), 0xFF0000);
		e.drawRectangle(vec2<int>(600, 700), 100, 100, 0x0000FF);
		e.stretchDIBits();
	}

	return 0;
}

// Processes the messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	// Key pressed event
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			DestroyWindow(hwnd);
		case VK_F11:
			e.toggleFullscreen();
		}
		break;
	// Destroy window event
	case WM_DESTROY:
		PostQuitMessage(0);
		running = false;
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}
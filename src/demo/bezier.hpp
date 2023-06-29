#ifndef BEZIER_DEMO
#define BEZIER_DEMO

#include "../GraphicsEngine.hpp"
#include<math.h>

bool running = true;
GraphicsEngine e;

int BezierDemoMain(_In_ HINSTANCE curInst, _In_opt_ HINSTANCE prevInst, _In_ PSTR cmdLine, _In_ INT cmdCount) {
	e.createWindow(curInst, 900, 900);

	// Create points for the bezier curves
	vec2<int> bezier1Points[3] = {
		{ 100, 100 },
		{ 150, 300 },
		{ 500, 150 }
	};

	vec2<int> bezier2Points[4] = {
		{ 400, 300 },
		{ 450, 200 },
		{ 600, 650 },
		{ 800, 150 }
	};

	// Value that modifies bezier curves every step
	int x = 0;

	// Main program loop
	while (running) {
		e.handleMessages();

		// Clear screen
		e.clearScreen(0x333333);

		// Update bezier curves
		x++;

		bezier1Points[0].x += (int)(2.0 * sin((double)(5 * (x - 2))) + 0.1);
		bezier1Points[1].y += (int)(2.0 * sin((double)(5 * (x - 2))) + 0.2);

		bezier2Points[1].x += (int)(2.0 * sin((double)(5 * (x - 2))) + 0.3);
		bezier2Points[2].y -= (int)(2.0 * sin((double)(5 * (x - 2))) + 0.4);
		bezier2Points[3].y += (int)(2.0 * sin((double)(5 * (x - 2))) + 0.5);

		// Draw bezier curves
		e.drawBezierCurve(bezier1Points[0], bezier1Points[1], bezier1Points[2], RED, 1);
		e.drawBezierCurve(bezier2Points[0], bezier2Points[1], bezier2Points[2], bezier2Points[3], BLUE, 1);
		e.drawBezierCurve(bezier2Points[1], bezier2Points[0], bezier2Points[3], bezier2Points[2], GREEN, 1);

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
			break;
		}
	}
	return e.processMessage(hwnd, msg, wParam, lParam, running);
}

#endif
#ifndef GRAPHICS_ENGINE
#define GRAPHICS_ENGINE

#include <windows.h>

// Circle equation check
#define CEQ(x, y, rSq) ((x) * (x) + (y) * (y) <= rSq)

typedef uint32_t u32;

// Processes the messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

enum COLOR {
	RED = 0xFF0000,
	GREEN = 0x00FF00,
	BLUE = 0x0000FF,
	YELLOW = 0xFFFF00,
	PINK = 0xFFC0CB,
	PURPLE = 0x800080,
	ORANGE = 0xFFA500,
	WHITE = 0xFFFFFF,
	BLACK = 0x000000,
};

template<class T>
struct vec2 {
	T x;
	T y;
	vec2() : x(0), y(0) {}
	vec2(T x, T y) : x(x), y(y) {}
};

struct Rect {
	//const wchar_t* id = nullptr;
	vec2<int> minPoint;
	vec2<int> maxPoint;

	Rect() : minPoint(vec2<int>()), maxPoint(vec2<int>()) {}
	Rect(vec2<int> x, vec2<int> y) : minPoint(x), maxPoint(y) {}
	Rect(vec2<int> coords, int width, int height) 
		: maxPoint(vec2<int>(coords.x + width, coords.y)), minPoint(vec2<int>(coords.x, coords.y + height)) {}

	bool isPointInside(vec2<int> point) {
		return point.x > maxPoint.x && point.x < minPoint.x && point.y > minPoint.y && point.y < maxPoint.y;
	}
};


class GraphicsEngine {
private:
	// Name of the window class
	const wchar_t* className = L"MyWindowClass";
	// Handle to the current instance of the application.
	HINSTANCE curInst = nullptr;
	// Memory allocated for the DIB
	void* memory = nullptr;
	// Info about the DIB for the StretchDIBits
	BITMAPINFO bitmapInfo = BITMAPINFO{};
	// Window styles
	int winStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE ;
	// Bonus mouse variables
	bool fsLbClick = false;
	bool fsRbClick = false;

	//// List of all buttons
	//Rect* rectButtons = nullptr;
	//int rectButtonsCount = 0;

	// Clears entire screen (not just bitmap) with a chosen color
	void clearEntireScreen(_In_ u32 color) {
		if (memory) {
			u32* pixel = (u32*)memory;
			for (int index = 0; index < width * height; ++index) {
				*pixel++ = color;
			}
		}
	}

	// Fills a triangle with 2 parallel bottom corners
	void fillBottomFlatTriangle(_In_ vec2<int> v1, _In_ vec2<int> v2, _In_ vec2<int> v3, _In_  u32 color) {
		// Get inverted slopes
		float invslope1 = (float)(v2.x - v1.x) / (float)(v2.y - v1.y);
		float invslope2 = (float)(v3.x - v1.x) / (float)(v3.y - v1.y);
		// X of the left and right end of the current line
		float curx1 = (float)v1.x;
		float curx2 = (float)v1.x;
		// Itarate thru the Y coordinates of the triangle down and draw lines calculating Xs of both ends using the inverted slope
		for (int scanlineY = v1.y; scanlineY <= v2.y; scanlineY++) {
			drawLine(vec2<int>((int)curx1, scanlineY), vec2<int>((int)curx2, scanlineY), color);
			curx1 += invslope1;
			curx2 += invslope2;
		}
	}

	// Fills a triangle with 2 parallel top corners
	void fillTopFlatTriangle(_In_ vec2<int> v1, _In_ vec2<int> v2, _In_ vec2<int> v3, _In_  u32 color) {
		// Get inverted slopes
		float invslope1 = (float)(v3.x - v1.x) / (float)(v3.y - v1.y);
		float invslope2 = (float)(v3.x - v2.x) / (float)(v3.y - v2.y);
		// X of the left and right end of the current line
		float curx1 = (float)v3.x;
		float curx2 = (float)v3.x;
		// Itarate thru the Y coordinates of the triangle up and draw lines calculating Xs of both ends using the inverted slope
		for (int scanlineY = v3.y; scanlineY > v1.y; scanlineY--) {
			drawLine(vec2<int>((int)curx1, scanlineY), vec2<int>((int)curx2, scanlineY), color);
			curx1 -= invslope1;
			curx2 -= invslope2;
		}
	}

public:
	// Handle to the window
	HWND hwnd = nullptr;
	// Handle to the device context of the window
	HDC hdc = nullptr;
	// If the window is in the fullscreen mode
	bool isFullscreen = false;
	// Size of the windowed window (not fullscreen)
	int windowedWidth = 800;
	int windowedHeight = 600;
	// Current size of the window
	int width = windowedWidth;
	int height = windowedHeight;
	// Vertical and horizontal margin in fullscreen mode
	int marginHorizontal = 0;
	int marginVertical = 0;
	// Size of the bitmap
	int bitmapWidth = width;
	int bitmapHeight = height;
	// Title of the window
	const wchar_t* title = L"GraphicsEngine";
	// Mouse positon
	int mouseX = 0;
	int mouseY = 0;
	// Mouse buttons
	bool lbPress = false;  // Left button held
	bool rbPress = false;  // Right button held
	bool lbClick = false;  // Left button clicked
	bool rbClick = false;  // Right button clicked

	// Creates a window
	int createWindow(_In_ HINSTANCE currentInstance, _In_opt_ int windowWidth = 800, _In_opt_ int windowHeight = 600, 
		_In_opt_ const wchar_t* windowTitle = L"GraphicsEngine") {
		// Set the class variables
		windowedWidth = windowWidth;
		width = windowWidth;
		bitmapWidth = windowWidth;
		windowedHeight = windowHeight;
		height = windowHeight;
		bitmapHeight = windowHeight;
		curInst = currentInstance;
		title = windowTitle;

		// Create and register the window class
		WNDCLASS wc = {};
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = currentInstance;
		wc.lpszClassName = className;
		wc.hCursor = LoadCursor(0, IDC_CROSS);

		if (!RegisterClass(&wc)) {
			MessageBox(0, L"RegisterClass failed", 0, 0);
			return -1;
		}

		// Create the window
		hwnd = CreateWindowEx(0, className, title,   // Optional window styles, window class, window title
			winStyle,                                // Window style
			CW_USEDEFAULT, CW_USEDEFAULT,            // Window initial position
			windowedWidth + 15, windowedHeight + 38, // Window size (there is the bonus size because of the bitmap size and windowed size issues)
			nullptr, nullptr, curInst, nullptr);     // Parent window, Menu, Instance handle, Additional app data

		if (!hwnd) {
			MessageBox(0, L"CreateWindowEx failed", 0, 0);
			return -1;
		}

		// Get handle to the device context of the window
		hdc = GetDC(hwnd);

		// Allocate memory for the bitmap 
		memory = VirtualAlloc(0,        // Starting address of the region to allocate
			static_cast<u32>(width) 
			* static_cast<u32>(height) 
			* sizeof(unsigned int),     // Size of the region (in bytes)
			MEM_RESERVE | MEM_COMMIT,   // Type of memory allocation
			PAGE_READWRITE);            // Memory protection for the region
		
		// Set info about the bitmap for the StretchDIBits
		bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader); 
		bitmapInfo.bmiHeader.biWidth = bitmapWidth;
		// Height is reversed so that the top left corner is the coordinate system origin
		bitmapInfo.bmiHeader.biHeight = -bitmapHeight;
		bitmapInfo.bmiHeader.biPlanes = 1;
		bitmapInfo.bmiHeader.biBitCount = 32;         // Number of bits used to represent each pixel  
		bitmapInfo.bmiHeader.biCompression = BI_RGB;  // Compression of the bitmap
		// When entering the fullscreen mode for the first time there appear defects on the margins
		// We quickly enter and exit the fullscreen mode after creating the window
		// So that defects don't appear if the user goes to the fullscreen mode himself
		enterFullscreen();
		exitFullscreen();

		return 0;
	}

	// Handles messages
	void handleMessages() {
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// Message processing
	LRESULT processMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool& isRunning) {
		switch (msg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			isRunning = false;
			return 0;
		case WM_MOUSEMOVE:
			mouseX = LOWORD(lParam);
			mouseY = HIWORD(lParam);
			break;
		case WM_LBUTTONDOWN:
			lbPress = true;
			fsLbClick = true;
			break;
		case WM_LBUTTONUP:
			lbPress = false;
			if (fsLbClick) {
				lbClick = !lbClick;
				fsLbClick = false;
			}
			break;
		case WM_RBUTTONDOWN:
			rbPress = true;
			fsRbClick = true;
			break;
		case WM_RBUTTONUP:
			rbPress = false;
			if (fsRbClick) {
				rbClick = !rbClick;
				fsRbClick = false;
			}	
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		return 0;
	}

	// Code that has to be run at the end of the main loop
	void mainLoopEndEvents() {
		// Strech the rows and columns of the color data of the source rectangle
		// to fit the destination rectangle
		StretchDIBits(hdc,                                              // The handle to the device context
			marginHorizontal, marginVertical,                           // The destination rectangle top left corner
			width - marginHorizontal * 2, height - marginVertical * 2,  // The destination rectangle size
			0, 0, bitmapWidth, bitmapHeight,                             // The source rectangle (top left corner coordinates and size)  
			memory, &bitmapInfo,                                        // A pointer to the image bitmap and bitmap info
			DIB_RGB_COLORS, SRCCOPY);                                   // Specifies whether bmiColors contains RGB values or indexes, a raster-operation code 

		// End button click event
		rbClick = false;
		lbClick = false;
	}

	// Clears screen with a chosen color
	void clearScreen(_In_ u32 color) {
		if (memory) {
			u32* pixel = (u32*)memory;
			for (int index = 0; index < bitmapWidth * bitmapHeight; ++index) {
				*pixel++ = color;
			}
		}
	}

	// Draws a pixel with custom color at specified coordinates
	void drawPixel(_In_ int x, _In_ int y, _In_ u32 color) {
		if (memory && x < bitmapWidth && y < bitmapHeight && x > 0 && y > 0) {
			u32* pixel = (u32*)memory;
			pixel += static_cast<u32>(y) * bitmapWidth + static_cast<u32>(x);
			*pixel = color;
		}
	}

	// Draws a rectangle
	void drawRectangle(_In_ vec2<int> coords, _In_ int recWidth, _In_ int recHeight, _In_ u32 color) {
		u32* pixel = (u32*)memory;
		pixel += coords.y * bitmapWidth + coords.x;
		// For each row of the rectangle draw pixels
		for (int y = 0; y < recHeight; ++y) {
			for (int x = 0; x < recWidth; ++x)
				*pixel++ = color;
			pixel += bitmapWidth - recWidth;
		}
	}

	// Draws a filled circle
	void drawCircle(_In_ vec2<int> origin, _In_ int radius, _In_  u32 color) {
		// Check for the trivial case of a 1 pixel radius circle
		if (radius > 1) {
			// Radius squared
			int rSq = radius * radius;
			// Traverse the y coordinates of the circle (from top (-) to bottom (+))
			for (int y = -radius; y <= radius; y++)
				// For each row length of the 2r
				for (int x = -radius; x <= radius; x++)
					// If the pixel is within the circle
					if (CEQ(x, y, rSq))
						drawPixel(origin.x + x, origin.y + y, color);
		}
		else
			drawPixel(origin.x, origin.y, color);
	}

	// Draws and empty circle
	void drawEmptyCircle(_In_ vec2<int> origin, _In_ int radius, _In_  u32 color, _In_opt_ unsigned short thickness = 1) {
		// Radius squared
		int rSq = radius * radius;
		// Traverse the y coordinates of the circle (from top (-) to bottom (+))
		for (int y = -radius; y <= radius; y++) {
			// Traverse the circle's x coordinates (from left (-) to right (+))
			for (int x = -radius; x <= radius; x++) 
				// If the pixel is within the circle
				if (CEQ(x, y, rSq)) {
					// Draw pixels of the circle until there is other pixel above or below
					drawCircle(vec2<int>(origin.x + x, origin.y + y), thickness, color);
					if (CEQ(x, y + 1, rSq) && CEQ(x, y - 1, rSq))
						break;
				}
			// Traverse the circle's x coordinates (from right (+) to left (-))
			for (int x = radius; x >= -radius; x--)
				// If the pixel is within the circle
				if (CEQ(x, y, rSq)) {
					// Draw pixels of the circle until there is other pixel above or below
					drawCircle(vec2<int>(origin.x + x, origin.y + y), thickness, color);
					if (CEQ(x, y + 1, rSq) && CEQ(x, y - 1, rSq))
						break;
				}

		}
	}

	// Draws a line
	void drawLine(_In_ vec2<int> p1, _In_ vec2<int> p2, _In_ u32 color, _In_opt_ unsigned short thickness = 1) {
		int x, y, e, pk;
		int dx = p2.x - p1.x;
		int dy = p2.y - p1.y;
		int dxAbs = abs(dx);
		int dyAbs = abs(dy);
		
		// If the slope is greater than or equal to 1
		if (dxAbs >= dyAbs) {
			pk = 2 * dyAbs - dxAbs;

			if (dx >= 0) {
				x = p1.x; 
				y = p1.y; 
				e = p2.x;
			}
			else {
				x = p2.x; 
				y = p2.y; 
				e = p1.x;
			}

			drawCircle(vec2<int>(x, y), thickness, color);

			for (int i = 0; x < e; i++) {
				x++;
				if (pk < 0)
					pk += 2 * dyAbs;
				else {
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y++; 
					else y--;
					pk += + 2 * (dyAbs - dxAbs);
				}
				drawCircle(vec2<int>(x, y), thickness, color);
			}
		}
		// If the slope is less than 1
		else {
			pk = 2 * dxAbs - dyAbs;

			if (dy >= 0) {
				x = p1.x; 
				y = p1.y; 
				e = p2.y;
			}
			else {
				x = p2.x; 
				y = p2.y; 
				e = p1.y;
			}

			drawCircle(vec2<int>(x, y), thickness, color);

			for (int i = 0; y < e; i++) {
				y++;
				if (pk <= 0)
					pk += 2 * dxAbs;
				else {
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x++; 
					else x--;
					pk += + 2 * (dxAbs - dyAbs);
				}
				drawCircle(vec2<int>(x, y), thickness, color);
			}
		}
	}

	// Draws a triangle
	void drawTriangle(_In_ vec2<int> v1, _In_ vec2<int> v2, _In_ vec2<int> v3, _In_ u32 color) {
		// Bonus vertice, used as temp for vertices sorting and as a spliting vertice in the general case
		vec2<int> v4;
		// Set vertices so that Y of the first one <= Y of the second one <= Y of the third one
		if (v1.y > v2.y) {
			v4 = v1;
			v1 = v2;
			v2 = v1;
		}
		if (v2.y > v3.y) {
			v4 = v2;
			v2 = v3;
			v3 = v2;
		}
		if (v1.y > v2.y) {
			v4 = v1;
			v1 = v2;
			v2 = v1;
		}
		// Check for trivial case of a bottom-flat triangle
		if (v2.y == v3.y) {
			fillBottomFlatTriangle(v1, v2, v3, color);
		}
		// Check for trivial case of a top-flat triangle
		else if (v1.y == v2.y) {
			fillTopFlatTriangle(v1, v2, v3, color);
		}
		else {
			// Create a bonus vertice spliting with v2 the triangle in a half
			v4 = vec2<int>((int)(v1.x + (float)(v2.y - v1.y) / (float)(v3.y - v1.y) * (v3.x - v1.x)), v2.y);
			// Draw the bottom-flat and the top-flat triangle
			fillBottomFlatTriangle(v1, v2, v4, color);
			fillTopFlatTriangle(v2, v4, v3, color);
		}
	}

	// Exit fullscreen mode
	void exitFullscreen() {
		SetWindowLongPtr(hwnd, GWL_STYLE, winStyle); // Set the window styles
		SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_LEFT); // Set the extended window styles
		
		// Set the window size to the windowed size
		width = windowedWidth;
		height = windowedHeight;
		// Remove the margins
		marginHorizontal = 0;
		marginVertical = 0;
		// Resize and reposition the window
		SetWindowPos(hwnd, HWND_NOTOPMOST,
			GetSystemMetrics(SM_CXSCREEN) / 10,      // Window position X
			GetSystemMetrics(SM_CYSCREEN) / 10,      // Window position Y
			windowedWidth + 15, windowedHeight + 39, // Window size (there is the bonus size because of the bitmap size and windowed size issues)
			SWP_SHOWWINDOW);
	}

	// Enter fullscreen mode
	void enterFullscreen() {
		MONITORINFO monitorInfo; // Get the monitor info
		monitorInfo.cbSize = sizeof(monitorInfo);
		GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &monitorInfo);

		// Set the window styles
		SetWindowLongPtr(hwnd, GWL_STYLE, winStyle & ~(WS_CAPTION));
		SetWindowLongPtr(hwnd, GWL_EXSTYLE, 0 & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));

		// Width and height ofthe screen
		int screenWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
		int screenHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

		// Screen ratios
		double ratioH = (double)windowedHeight / (double)windowedWidth;
		double ratioW = (double)windowedWidth / (double)windowedHeight;

		int screenHeightRatioed = (int)((double)screenHeight * ratioW);
		int screenWidthRatioed = (int)((double)screenWidth * ratioH);

		bool isScreenWider = screenWidth >= screenHeight;

		// Detect whether to use the vertical or the horizontal margins
		if ((isScreenWider && screenHeightRatioed < screenWidth) || (!isScreenWider && screenWidthRatioed > screenHeight)) {
			// Set width based on the screen height
			width = (int)((double)screenHeight * ratioW);
			// Set horizonal margins
			marginHorizontal = (screenWidth - width) / 2;
		}
		else {
			// Set height based on the screen width
			height = (int)((double)screenWidth * ratioH);
			// Set vertical margins
			marginVertical = (screenHeight - height) / 2;
		}

		width = screenWidth;
		height = screenHeight;

		// Allocate memory for the entire screen to clear it
		memory = VirtualAlloc(0,        // Starting address of the region to allocate
			static_cast<u32>(width) * 
			static_cast<u32>(height) * 
			sizeof(unsigned int),       // Size of the region (in bytes)
			MEM_RESERVE | MEM_COMMIT,   // Type of memory allocation
			PAGE_READWRITE);            // Memory protection for the region
		
		// Clear the entire screen to remove the defects appearing on the margins
		clearEntireScreen(0x000000);

		// Update entire screen
		StretchDIBits(hdc,
			0, 0, width, height,
			0, 0, bitmapWidth, bitmapHeight,
			memory, &bitmapInfo,
			DIB_RGB_COLORS, SRCCOPY);

		// Allocate memory back for the bitmap only
		memory = VirtualAlloc(0,             // Starting address of the region to allocate
			static_cast<u32>(bitmapWidth) 
			* static_cast<u32>(bitmapHeight) 
			* sizeof(unsigned int),           // Size of the region (in bytes)
			MEM_RESERVE | MEM_COMMIT,         // Type of memory allocation
			PAGE_READWRITE);                  // Memory protection for the region

		// Resize, move, and refresh the window
		SetWindowPos(
			hwnd,
			nullptr,
			monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
			width, height,
			SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}

	// Toggle fullscreen mode
	void toggleFullscreen() {
		// If fullscreen is not enabled
		if (!isFullscreen) {
			// Enter fullscreen
			enterFullscreen();
			// Indicate that fullscreen is on
			isFullscreen = true;
		}
		// If fullscreen is enabled
		else {
			// Exit fullscreen (go back to windowed)
			exitFullscreen();
			// Indicate that fullscreen is off
			isFullscreen = false;
		}
	}
};

#endif // !GRAPHICS_ENGINE
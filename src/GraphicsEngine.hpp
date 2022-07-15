#ifndef GRAPHICS_ENGINE
#define GRAPHICS_ENGINE

#include <windows.h>
#include <stdint.h>

typedef uint32_t u32;

// Processes the messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

enum COLOR {
	RED = 0xFF0000,
	GREEN = 0x00FF00,
	BLUE = 0x0000FF,
	WHITE = 0xFFFFFF,
	BLACK = 0x000000,
};

template<class T>
struct vec2 {
	T x;
	T y;
	vec2(T x, T y) : x(x), y(y) {}
};

int abs(int num) {
	if (num < 0) return -num;
	return num;
}

class GraphicsEngine {
private:
	// Name of the window class
	const wchar_t* className = L"MyWindowClass";
	// Handle to the current instance of the application.
	HINSTANCE curInst = nullptr;

public:
	// Memory allocated for the DIB
	void* memory;
	// Info about the DIB for the StretchDIBits
	BITMAPINFO bitmapInfo = BITMAPINFO{};

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
	// Title of the window
	const wchar_t* title = L"GraphicsEngine";

	// Creates a window
	int createWindow(_In_ HINSTANCE currentInstance, _In_opt_ int windowWidth = 800, _In_opt_ int windowHeight = 600, 
		_In_opt_ const wchar_t* windowTitle = L"GraphicsEngine") {
		// Set the class variables
		windowedWidth, width = windowWidth;
		windowedHeight, height = windowHeight;
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
		hwnd = CreateWindowEx(0, className, title,       // Optional window styles, window class, window title
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,       // Window style
			CW_USEDEFAULT, CW_USEDEFAULT,           // Window initial position
			width, height,                          // Window size
			nullptr, nullptr, curInst, nullptr);    // Parent window, Menu, Instance handle, Additional app data

		if (!hwnd) {
			MessageBox(0, L"CreateWindowEx failed", 0, 0);
			return -1;
		}

		// Get handle to the device context of the window
		hdc = GetDC(hwnd);

		// Allocate memory for the bitmap 
		memory = VirtualAlloc(0,                    // Starting address of the region to allocate
			width * height * sizeof(unsigned int),  // Size of the region (in bytes)
			MEM_RESERVE | MEM_COMMIT,               // Type of memory allocation
			PAGE_READWRITE);                        // Memory protection for the region
		
		// Set info about the bitmap for the StretchDIBits
		bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader); 
		bitmapInfo.bmiHeader.biWidth = width;
		// Height is reversed so that the top left corner is the coordinate system origin
		bitmapInfo.bmiHeader.biHeight = -height;
		bitmapInfo.bmiHeader.biPlanes = 1;
		bitmapInfo.bmiHeader.biBitCount = 32;         // Number of bits used to represent each pixel  
		bitmapInfo.bmiHeader.biCompression = BI_RGB;  // Compression of the bitmap

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

	// Calls StretchDIBits function
	void stretchDIBits() {
		// Strech the rows and columns of the color data of the source rectangle
		// to fit the destination rectangle
		StretchDIBits(hdc,             // The handle to the device context
			0, 0, width, height,       // The destination rectangle (top left corner coordinates and size)      
			0, 0, width, height,       // The source rectangle (top left corner coordinates and size)  
			memory, &bitmapInfo,       // A pointer to the image bitmap and bitmap info
			DIB_RGB_COLORS, SRCCOPY);  // Specifies whether bmiColors contains RGB values or indexes, a raster-operation code
	}

	// Clears screen with a chosen color
	void clearScreen(_In_ u32 color) {
		if (memory) {
			u32* pixel = (u32*)memory;
			for (int index = 0; index < width * height; ++index) {
				*pixel++ = color;
			}
		}
	}

	// Draws a pixel with custom color at specified coordinates
	void drawPixel(_In_ int x, _In_ int y, _In_ u32 color) {
		if (memory) {
			u32* pixel = (u32*)memory;
			pixel += y * width + x;
			*pixel = color;
		}
	}

	// Draws a line
	void drawLine(_In_ vec2<int> p1, _In_ vec2<int> p2, _In_ u32 color) {
		int x, y, dx, dy, dxAbs, dyAbs, px, py, xe, ye;
		dx = p2.x - p1.x;
		dy = p2.y - p1.y;
		dxAbs = abs(dx);
		dyAbs = abs(dy);
		px = 2 * dyAbs - dxAbs;
		py = 2 * dxAbs - dyAbs;

		if (dyAbs <= dxAbs) {
			if (dx >= 0) {
				x = p1.x; 
				y = p1.y; 
				xe = p2.x;
			}
			else {
				x = p2.x; 
				y = p2.y; 
				xe = p1.x;
			}

			drawPixel(x, y, color);

			for (int i = 0; x < xe; i++) {
				x++;
				if (px < 0)
					px = px + 2 * dyAbs;
				else {
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y--;
					px += + 2 * (dyAbs - dxAbs);
				}
				drawPixel(x, y, color);
			}
		}
		else {
			if (dy >= 0) {
				x = p1.x; 
				y = p1.y; 
				ye = p2.y;
			}
			else {
				x = p2.x; 
				y = p2.y; 
				ye = p1.y;
			}

			drawPixel(x, y, color);

			for (int i = 0; y < ye; i++) {
				y++;
				if (py <= 0)
					py = py + 2 * dxAbs;
				else {
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x++; 
					else x--;
					py += + 2 * (dxAbs - dyAbs);
				}
				drawPixel(x, y, color);
			}
		}
	}

	// Enter fullscreen mode
	bool enterFullscreen() {
		// Holds infomation about the display
		DEVMODE fullscreenSettings = DEVMODE{};
		bool isChangeSuccessful;

		// Get width and height of the screen
		int fullscreenWidth = GetDeviceCaps(hdc, DESKTOPHORZRES);
		int fullscreenHeight = GetDeviceCaps(hdc, DESKTOPVERTRES);

		// Set information about the initialization and environment of the display
		EnumDisplaySettings(NULL, 0, &fullscreenSettings);
		fullscreenSettings.dmPelsWidth = fullscreenWidth;
		fullscreenSettings.dmPelsHeight = fullscreenHeight;
		fullscreenSettings.dmBitsPerPel = GetDeviceCaps(hdc, BITSPIXEL);
		fullscreenSettings.dmDisplayFrequency = GetDeviceCaps(hdc, VREFRESH);
		fullscreenSettings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;

		// Set the window attributes
		SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
		SetWindowLongPtr(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, fullscreenWidth, fullscreenHeight, SWP_SHOWWINDOW);

		// Change the display settings and see if the change was successful
		isChangeSuccessful = ChangeDisplaySettings(&fullscreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL;
		ShowWindow(hwnd, SW_MAXIMIZE);

		// Change window width and height to the new one if the change succeded 
		if (isChangeSuccessful) {
			width = fullscreenWidth;
			height = fullscreenHeight;
		}
		return isChangeSuccessful;
	}

	// Exit fullscreen mode
	bool exitFullscreen() {
		bool isChangeSuccessful;

		// Set the window attributes
		SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_LEFT);
		SetWindowLongPtr(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
		SetWindowPos(hwnd, HWND_NOTOPMOST,
			GetSystemMetrics(SM_CXSCREEN) / 4, GetSystemMetrics(SM_CYSCREEN) / 4, // Window position
			windowedWidth, windowedHeight,                                        // Window size
			SWP_SHOWWINDOW);

		// Change the display settings and see if the change was successful
		isChangeSuccessful = ChangeDisplaySettings(NULL, CDS_RESET) == DISP_CHANGE_SUCCESSFUL;
		ShowWindow(hwnd, SW_RESTORE);

		// Change window width and height to the new one if the change succeded 
		if (isChangeSuccessful) {
			width = windowedWidth;
			height = windowedHeight;
		}
		return isChangeSuccessful;
	}

};


#endif // !GRAPHICS_ENGINE
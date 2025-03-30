#ifndef GRAPH_DEMO
#define GRAPH_DEMO

#include "../GraphicsEngine.hpp"

#include<algorithm>
#include<iostream>
#include<iomanip>
#include<sstream>
#include<math.h>
#include<chrono>
#include<string>
#include<vector>

std::pair<double, double> sortTester(void (*f)(std::vector<int>&), int size, int runs = 1);
void bubbleSort(std::vector<int>& v);
void bubbleSortNoCheck(std::vector<int>& v);
void insertionSort(std::vector<int>& v);
void selectionSort(std::vector<int>& v);
void basicSort(std::vector<int>& v);
void mergeSort(std::vector<int>& v);

bool running = true;
GraphicsEngine e;

int randMinMax(int min, int max) {
	return rand() % (max - min + 1) + min;
}

int setButtons(std::wstring labels[], int size) {
	std::vector<Rect> buttons = {};
	int h = 200;
	int iW = 0;
	int choice = 0;
	for (int i = 0; i < size; i++) {
		if (220 + iW * 220 > e.width) {
			h += 100;
			iW = 0;
		}
		buttons.push_back(Rect(vec2<int>(70 + iW * 220, h), vec2<int>(220 + iW * 220, h + 50)));
		e.drawRectangle(buttons[i], GREY);
		e.drawText(145 - labels[i].length() * 4 + iW * 220, h + 15, labels[i].c_str(), 20);
		if (buttons[i].isPointInside(vec2<int>(e.mouseX, e.mouseY)) && e.lbClick)
			choice = i + 1;
		iW++;
	}
	return choice;
}

void showGraph(int lengths[], std::pair<double, double> times[], int size, std::wstring title) {
	int gWMax = e.width - 70;
	int gWMin = 70;
	int gHMax = e.height - 50;
	int gHMin = 50;

	double maxFound = 0.0;

	for (int i = 0; i < size; i++) {
		std::pair<double, double> curPair = times[i];
		if (curPair.second > maxFound)
			maxFound = curPair.second;
	}

	int maxFoundTop = ceil(maxFound);
	double unit = ceil((double)maxFoundTop * 10.0) / 100.0;

	e.drawText((gWMax - gWMin) / 2, gHMin - 30, title.c_str(), 40, WHITE);

	for (int i = 0; i < 11; i++) {
		int basePartH = gHMax - 30 - i * (int)((double)((gHMax - gHMin) / 11));
		std::wstringstream ss;
		ss << std::fixed << std::setprecision(2) << unit * i;
		e.drawText(gWMin, basePartH, ss.str().c_str(), 20, WHITE);
		e.drawLine(vec2<int>(gWMin + 50, basePartH + 10), vec2<int>(gWMax - 5, basePartH + 10), GREY, 1);
	}

	for (int i = 0; i < size; i++) {
		int basePartW = gWMin + 90 + i * (gWMax - gWMin - 50) / size;
		std::wstring lenStr = std::to_wstring(lengths[i]);
		// What percent of the highest displayed OY value is the number
		double percent = times[i].first / maxFoundTop;
		double percentMax = times[i].second / maxFoundTop;
		// "Ceiling" dof the current column
		int pixelTop = gHMax - 20 - (int)((double)((gHMax - gHMin) / 11) * 10 * (double)percent);
		int pixelTopMax = gHMax - 20 - (int)((double)((gHMax - gHMin) / 11) * 10 * (double)percentMax);
		e.drawRectangle(Rect(vec2<int>(basePartW - 30, pixelTopMax), vec2<int>(basePartW + (gWMax - gWMin - 40) / size - 60, gHMax - 20)), RED);
		e.drawRectangle(Rect(vec2<int>(basePartW - 30, pixelTop), vec2<int>(basePartW + (gWMax - gWMin - 40) / size - 60, gHMax - 20)), GREEN);
		e.drawText(basePartW - 26, pixelTop - 20, std::to_wstring(times[i].first).c_str(), 20, 0xdbffd9);
		e.drawText(basePartW - 26, pixelTop - 40, std::to_wstring(times[i].second).c_str(), 20, 0xff9c9c);
		e.drawText(basePartW - lenStr.length(), gHMax - 15, lenStr.c_str(), 20, WHITE);
	}
};

void showGraph2(int lengths[], std::pair<double, double> times[], int size, std::wstring title) {
	int gWMax = e.width - 70;
	int gWMin = 70;
	int gHMax = e.height - 50;
	int gHMin = 50;

	double maxFound = 0.0;

	for (int i = 0; i < size; i++) {
		std::pair<double, double> curPair = times[i];
		if (curPair.first > maxFound)
			maxFound = curPair.first;
	}

	int maxFoundTop = ceil(maxFound);
	double unit = ceil((double)maxFoundTop * 10.0) / 100.0;

	e.drawText((gWMax - gWMin) / 2, gHMin - 30, title.c_str(), 40, WHITE);

	for (int i = 0; i < 11; i++) {
		int basePartH = gHMax - 30 - i * (int)((double)((gHMax - gHMin) / 11));
		std::wstringstream ss;
		ss << std::fixed << std::setprecision(2) << unit * i;
		e.drawText(gWMin, basePartH, ss.str().c_str(), 20, WHITE);
		// X grid lines
		if(i != 0)
			e.drawLine(vec2<int>(gWMin + 40, basePartH + 10), vec2<int>(gWMax, basePartH + 10), GREY, 1);
	}

	int lineSpace = (gWMax - gWMin - 20) / 21;
	int increment = lengths[size - 1] / 20;

	// Y grid lines
	for (int i = 0; i < 21; i++) {
		std::wstring incStr = std::to_wstring(increment * i);
		if(i != 0)
			e.drawLine(vec2<int>(gWMin + 50 + i * lineSpace, gHMax - 10), vec2<int>(gWMin + 50 + i * lineSpace, gHMin + 20), GREY);
		e.drawText(gWMin + 46 - incStr.size() * 2 + i * lineSpace, gHMax - 5, incStr.c_str(), 16, WHITE);
	}

	// OX and OY
	e.drawLine(vec2<int>(gWMin + 40, gHMax - 20), vec2<int>(gWMax, gHMax - 20), WHITE, 2);
	e.drawLine(vec2<int>(gWMin + 50, gHMax - 10), vec2<int>(gWMin + 50, gHMin + 20), WHITE, 2);

	for (int i = 0; i < size; i++) {
		double percentX = (double)lengths[i] / (double)lengths[size - 1];
		double percentY = times[i].first / maxFoundTop;
		int pointX = gWMin + 50 + (double)((gWMax - gWMin - 20) / 21.0) * 20 * percentX;
		int pointY = gHMax - 20 - (int)((double)((gHMax - gHMin) / 11) * 10 * (double)percentY);
		e.drawCircle(vec2<int>(pointX, pointY), 5, RED);
	}
};

int GraphDemoMain(_In_ HINSTANCE curInst, _In_opt_ HINSTANCE prevInst, _In_ PSTR cmdLine, _In_ INT cmdCount) {
	e.createWindow(curInst, 960, 600, L"Lista 2");

	// Interaction variables
	bool fullscreenHeld = false;
	bool curGraphCols = true;
	bool toggleGraph = false;
	bool back = false;

	int screen = 0;
	int tempScreenState = 0;

	// Test nums
	int runs = 20;

	// Lengths of the test arrays
	int lengths[7] = { 10, 20, 50, 100, 200, 500, 1000 };

	// Measured times
	std::pair<double, double> times[7] = {};

	// List of sorting algorithms
	void (*funcs[6])(std::vector<int>&) = {bubbleSort, insertionSort, selectionSort, basicSort, bubbleSortNoCheck, mergeSort};

	std::wstring labels[6] = { L"Bubble Sort", L"Insertion Sort", L"Selection Sort", L"sort()", L"Bubble Sort 2", L"Merge Sort"};

	// Main program loop
	while (running) {
		e.handleMessages();

		if (!e.keys[VK_ESCAPE].isHeld && back) {
			e.clearScreen();
			screen = 0;
			back = false;
			toggleGraph = false;
		}

		if (screen == 0) {
			if (e.keys[VK_ESCAPE].isHeld)
				e.destroy();
			if (!e.keys['R'].isHeld)
				screen = setButtons(labels, 6);
			else
				screen = tempScreenState;
			if (screen != 0) {
				e.clearScreen();
				for (int i = 0; i < 7; i++) {
					std::pair<double, double> avgMax = sortTester(funcs[screen - 1], lengths[i], runs);
					times[i] = avgMax;
				}
				if (curGraphCols)
					showGraph(lengths, times, 7, labels[screen - 1]);
				else
					showGraph2(lengths, times, 7, labels[screen - 1]);
			}
		}
		else {
			if (e.keys[VK_ESCAPE].isHeld)
				back = true;
			else if (e.keys['R'].isHeld) {
				tempScreenState = screen;
				screen = 0;
			}
			else if (e.keys['G'].isHeld)
				toggleGraph = true;
			else if (!e.keys['G'].isHeld && toggleGraph) {
				e.clearScreen();
				if (curGraphCols) {
					curGraphCols = false;
					showGraph2(lengths, times, 7, labels[screen - 1]);
				}
				else {
					curGraphCols = true;
					showGraph(lengths, times, 7, labels[screen - 1]);
				}
				toggleGraph = false;
			}
		}

		if (e.keys[VK_F11].isHeld && !fullscreenHeld) {
			e.toggleFullscreen();
			fullscreenHeld = true;
		}
		else if (!e.keys[VK_F11].isHeld)
			fullscreenHeld = false;

		e.mainLoopEndEvents();
	}

	return 0;
}

std::pair<double, double> sortTester(void (*f)(std::vector<int>&), int size, int runs) {
	std::vector<int> v(size);
	// Avg and max measured time
	std::chrono::duration<double> avgDuration{};
	std::chrono::duration<double> maxDuration{};

	for (int i = 0; i < runs; i++) {
		if (size <= 10 && i == 0)
			OutputDebugStringW(L"\nPrzed sortowaniem:\n");
		for (int j = 0; j < size; j++) {
			v[j] = randMinMax(1, size * 2);
			if (size <= 10 && i == 0)
				OutputDebugStringW((std::to_wstring(v[j]) + L',').c_str());
		}
		auto start = std::chrono::high_resolution_clock::now(); // Pocz¹tek pomiaru

		f(v);

		auto end = std::chrono::high_resolution_clock::now(); // Koniec pomiaru
		std::chrono::duration<double> curDuration = end - start;
		avgDuration += curDuration;
		if (curDuration > maxDuration) maxDuration = curDuration;
		if (size <= 10 && i == 0) {
			OutputDebugStringW(L"\nPosortowana:\n");
			for (int j = 0; j < size; j++)
				OutputDebugStringW((std::to_wstring(v[j]) + L',').c_str());
			OutputDebugStringW(L"\n");
		}
	}

	avgDuration /= static_cast<double>(runs);

	// Time in ms instead of s (*1000)
	return { avgDuration.count() * 1000.0, maxDuration.count() * 1000.0 };
}


//          Algorithms          //
void bubbleSort(std::vector<int>& v) {
	int n = v.size();

	// Looped over maximum of size times
	for (int i = 0; i < n - 1; i++) {
		bool swapped = false;

		for (int j = 0; j < n - 1 - i; j++) {
			if (v[j] > v[j + 1]) {
				int temp = v[j];
				v[j] = v[j + 1];
				v[j + 1] = temp;
				swapped = true;
			}
		}

		if (!swapped) break;
	}
}

void bubbleSortNoCheck(std::vector<int>& v) {
	int n = v.size();

	// Always loop size times
	for (int i = 0; i < n - 1; i++)
		for (int j = 0; j < n - 1 - i; j++)
			if (v[j] > v[j + 1]) {
				int temp = v[j];
				v[j] = v[j + 1];
				v[j + 1] = temp;
			}
}

void insertionSort(std::vector<int>& arr) {
	for (int i = 1; i < arr.size(); i++) {
		int key = arr[i];
		int j = i - 1;

		// Moving element slarger than the key 1 to the right
		while (j >= 0 && arr[j] > key) {
			arr[j + 1] = arr[j];
			--j;
		}

		arr[j + 1] = key;
	}
}

void selectionSort(std::vector<int>& arr) {
	int n = arr.size();

	for (int i = 0; i < n - 1; i++) {
		int minIndex = i;

		// Find lowest variable in the unsorted part
		for (int j = i + 1; j < n; j++)
			if (arr[j] < arr[minIndex])
				minIndex = j;

		// Switch smallest found with the first unsorted
		if (minIndex != i) {
			int temp = arr[minIndex];
			arr[minIndex] = arr[i];
			arr[i] = temp;
		}
	}
}

void basicSort(std::vector<int>& v) {
	std::sort(v.begin(), v.end());
}

void merge(std::vector<int>& arr, int left, int mid, int right) {
	std::vector<int> temp;
	int i = left, j = mid + 1;

	while (i <= mid && j <= right) {
		if (arr[i] < arr[j])
			temp.push_back(arr[i++]);
		else
			temp.push_back(arr[j++]);
	}

	while (i <= mid) temp.push_back(arr[i++]);
	while (j <= right) temp.push_back(arr[j++]);

	for (int k = 0; k < temp.size(); k++)
		arr[left + k] = temp[k];
}

void mergeSortRecursion(std::vector<int>& arr, int left, int right) {
	if (left >= right) return;

	int mid = left + (right - left) / 2;
	mergeSortRecursion(arr, left, mid);
	mergeSortRecursion(arr, mid + 1, right);
	merge(arr, left, mid, right);
}

void mergeSort(std::vector<int>& arr) {
	mergeSortRecursion(arr, 0, arr.size() - 1);
}


// Processes the messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	return e.processMessage(hwnd, msg, wParam, lParam);
}

#endif
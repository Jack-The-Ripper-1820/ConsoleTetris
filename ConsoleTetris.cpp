// ConsoleTetris.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <Windows.h>
#include <memory>

using namespace std;

wstring tetrisBlock[7];
int fieldWidth = 12;
int fieldHeight = 18;
vector<uint8_t> field;

int screenWidth = 120;
int screenHeight = 40;


int rotate(int x, int y, int angle) {
	switch (angle % 4) {
	case 0: return y * 4 + x;
	case 1: return 12 + y - (x * 4);
	case 2: return 15 - (y * 4) - x;
	case 3: return 3 - y + (x * 4);
	}

	return 0;
}

bool validFit(int tetrisblock, int rotation, int locx, int locy) {
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			// get index of tetris block according to its rotation
			int index = rotate(locx, locy, rotation);

			// get index of block inside the play field
			int findex = (locy + y) * fieldWidth + (locx + x);

			if (locx + x >= 0 && locx + x < fieldWidth) {
				if (locy + y >= 0 && locy + y < fieldHeight) {
					if (tetrisBlock[tetrisblock][index] == L'X' && field[findex] != 0)
						return false; //  fail on first hit
				}
			}
		}
	}

	return true;
}
int main()
{
	//// try to get dynamically screen width and height
	//try {

	//}
	//catch (exception const& e) {
	//	cout << e.what() << endl;
	//}

	tetrisBlock[0].append(L"..X...X...X...X."); 
	tetrisBlock[1].append(L"..X..XX...X.....");
	tetrisBlock[2].append(L".....XX..XX.....");
	tetrisBlock[3].append(L"..X..XX..X......");
	tetrisBlock[4].append(L".X...XX...X.....");
	tetrisBlock[5].append(L".X...X...XX.....");
	tetrisBlock[6].append(L"..X...X..XX.....");

	//field = vector<uint8_t>(fieldWidth * fieldHeight); // play field buffer

	field.resize(fieldWidth * fieldHeight);

	for (int x = 0; x < fieldWidth; x++) { // boundary 
		for (int y = 0; y < fieldHeight; y++) {
			field[y * fieldWidth + x] = (x == 0 || x == fieldWidth - 1
				|| y == fieldHeight - 1) ? 9 : 0;
		}
	}

	//vector<wchar_t> sc 
	//wchar_t* screen = new wchar_t[screenWidth, screenHeight];
	vector<wchar_t> screen(screenWidth * screenHeight, L' ');
	for (int i = 0; i < screenWidth * screenHeight; i++) screen[i] = L' ';

	HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(console);
	DWORD bytesWritten = 0;

	bool gameOver = false;

	int currentPiece = 0;
	int currentRotation = 0;
	int currentX = fieldWidth / 2;
	int currentY = 0;

	while (!gameOver) {
		// game timer
		
		// game input

		// game logic

		// render output

		// draw board
		for (int x = 0; x < fieldWidth; x++) {
			for (int y = 0; y < fieldHeight; y++) {
				screen[(y + 2) * screenWidth + (x + 2)] = L" ABCDEFG=#"[field[y * fieldWidth + x]];
			}
		}

		//// draw falling block
		//for (int x = 0; x < 4; x++) {
		//	for (int y = 0; y < 4; y++) {
		//		if (tetrisBlock[currentPiece][rotate(x, y, currentRotation)] == L'X') {
		//			screen[(currentY + y + 2) * screenWidth + ]
		//		}
		//	}
		//}

		WriteConsoleOutputCharacter(console, screen.data(), screenWidth * screenHeight, {0, 0}, &bytesWritten);
	}

	// Display frame

	//std::cout << "Hello" << endl;

	return 0;
}
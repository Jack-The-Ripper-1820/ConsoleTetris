// ConsoleTetris.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <Windows.h>
#include <memory>
#include <thread>

using namespace std;

wstring tetrisBlock[7];
int boardWidth = 12;
int boardHeight = 18;
vector<uint8_t> board;

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
			int index = rotate(x, y, rotation);

			// get index of block inside the play board
			int findex = (locy + y) * boardWidth + (locx + x);

			if (locx + x >= 0 && locx + x < boardWidth && locy + y >= 0 && locy + y < boardHeight) {
				if (tetrisBlock[tetrisblock][index] == L'X' && board[findex] != 0) {
					return false; //  fail on first hit
				}
			}
		}
	}

	return true;
}
int main()
{
	tetrisBlock[0].append(L"..X...X...X...X."); 
	tetrisBlock[1].append(L"..X..XX...X.....");
	tetrisBlock[2].append(L".....XX..XX.....");
	tetrisBlock[3].append(L"..X..XX..X......");
	tetrisBlock[4].append(L".X...XX...X.....");
	tetrisBlock[5].append(L".X...X...XX.....");
	tetrisBlock[6].append(L"..X...X..XX.....");

	board.resize(boardWidth * boardHeight);

	for (int x = 0; x < boardWidth; x++) { // boundary 
		for (int y = 0; y < boardHeight; y++) {
			board[y * boardWidth + x] = (x == 0 || x == boardWidth - 1
				|| y == boardHeight - 1) ? 9 : 0;
		}
	}

	vector<wchar_t> screen(screenWidth * screenHeight, L' ');
	for (int i = 0; i < screenWidth * screenHeight; i++) screen[i] = L' ';

	HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(console);
	DWORD bytesWritten = 0;

	bool gameOver = false;

	int currentblock = 0;
	int currentRotation = 0;
	int currentX = boardWidth / 2;
	int currentY = 0;

	bool key[4];
	const unsigned char keys[] = { 'A', 'S', 'D', VK_SHIFT };
	bool rotateKeyHold = false;

	// difficulty vars
	int diff = 20, diffLvl = 0, blockCnt = 0;
	bool autoLower = false;

	int score = 0;

	vector<int> lines;

	// game loop (tick)
	while (!gameOver) {
		// game timer
		this_thread::sleep_for(50ms);
		diffLvl++;
		autoLower = diffLvl == diff;

		// game input
		for (int i = 0; i < 4; i++) {
			key[i] = (0x8000 & GetAsyncKeyState(keys[i])) != 0;
		}

		// game logic
		currentX -= key[0] && validFit(currentblock, currentRotation, currentX - 1, currentY);
		
		currentX += key[2] && validFit(currentblock, currentRotation, currentX + 1, currentY);

		currentY += key[1] && validFit(currentblock, currentRotation, currentX, currentY + 1);

		if (key[3]) {
			currentRotation += !rotateKeyHold && validFit(currentblock, currentRotation + 1, currentX, currentY);
			rotateKeyHold = true;
		}
		
		else rotateKeyHold = false;

		if (autoLower) {
			if (validFit(currentblock, currentRotation, currentX, currentY + 1)) currentY++; // can fit lower the block
			else {
				// lock the block in the board
				for (int x = 0; x < 4; x++) {
					for (int y = 0; y < 4; y++) {
						if (tetrisBlock[currentblock][rotate(x, y, currentRotation)] == L'X')
							board[(currentY + y) * boardWidth + (currentX + x)] = currentblock + 1;
					}
				}

				// increase difficulty
				blockCnt++;
				diff -= blockCnt % 10 == 0 && diff >= 10;

				// check full line formation
				for (int y = 0; y < 4; y++) {
					if (currentY + y < boardHeight - 1) {
						bool line = true;

						for (int x = 1; x < boardWidth - 1; x++) {
							if (board[(currentY + y) * boardWidth + x] == 0) {
								line = false;
								break;
							}
						}

						if (line) {
							// delete line, set to ======

							for (int x = 1; x < boardWidth - 1; x++) {
								board[(currentY + y) * boardWidth + x] = 8;
							}

							lines.push_back(currentY + y);
						}
					}
				}

				score += 5;
				if (!lines.empty()) {
					score += (1 << lines.size()) * 20;
				}

				// next block
				currentX = boardWidth / 2;
				currentY = 0;
				currentRotation = 0;
				currentblock = rand() % 7;

				// game over condition
				gameOver = !validFit(currentblock, currentRotation, currentX, currentY);
			}

			diffLvl = 0;
		}

		// render output

		// draw board
		for (int x = 0; x < boardWidth; x++) {
			for (int y = 0; y < boardHeight; y++) {
				screen[(y + 2) * screenWidth + (x + 2)] = L" ABCDEFG=#"[board[y * boardWidth + x]];
			}
		}

		// draw falling block
		for (int x = 0; x < 4; x++) {
			for (int y = 0; y < 4; y++) {
				if (tetrisBlock[currentblock][rotate(x, y, currentRotation)] == L'X') {
					screen[(currentY + y + 2) * screenWidth + (currentX + x + 2)] = currentblock + 65;
				}
			}
		}

		//display score
		swprintf_s(&screen[2 * screenWidth + boardWidth + 6], 16, L"SCORE: %8d", score);

		if (!lines.empty()) {
			WriteConsoleOutputCharacter(console, screen.data(), screenWidth * screenHeight, { 0, 0 }, &bytesWritten);
			this_thread::sleep_for(400ms); // delay removing lines

			for (auto& lineElem : lines) {
				for (int x = 1; x < boardWidth - 1; x++) {
					for (int y = lineElem; y > 0; y--) {
						board[y * boardWidth + x] = board[(y - 1) * boardWidth + x];
					}

					board[x] = 0;
				}
			}

			lines.clear();
		}

		// display frame
		WriteConsoleOutputCharacter(console, screen.data(), screenWidth * screenHeight, {0, 0}, &bytesWritten);
	}

	// Game Over Message
	CloseHandle(console);
	std::cout << "GAME OVER! SCORE: " << score << endl;
	std::system("pause");

	return 0;
}
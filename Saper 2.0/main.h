#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include "resource.h"

#define settingsMenuCode 0
#define exitMenuCode 1

int columnsArena = 11,
rowsArena = 6,
levelOfGame = 2,
currentScore = 0,
currentTime = 0,
currentRemainingLifes = 0;
bool firstStart = true,
soundOn = true,
extraLifesOn = true,
flagsOn = true;

std::vector <std::vector<bool>> arenaGrass(columnsArena, std::vector <bool>(rowsArena, true)); //2d array in vector (vector array of booleans, with vectors of booleans inside)
std::vector <std::vector<int>> arenaGround(columnsArena, std::vector <int>(rowsArena, 0)); //2d array in vector (vector array of booleans, with vectors of booleans inside)
RECT arena;
HINSTANCE hInst;

bool IsPointInRect(POINT, RECT);
POINT GetIndexGrassBlockClicked(POINT, int);
//RECT GetRectOfClickedBlock(POINT, int);
void DiggingGrassToCoords(POINT, int);
void GenerateBombs(POINT, int);
void CalculateNumberBlocks();
void DiggingVoidArea(int, int);
void RestartGame();

void WndMenu(HWND);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int APIENTRY WinMain(HINSTANCE, HINSTANCE, PSTR, int);
INT_PTR CALLBACK SettingsDialog(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK GameOverDialog(HWND, UINT, WPARAM, LPARAM);
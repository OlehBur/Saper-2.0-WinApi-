#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include "resource.h"

#define settingsMenuCode 0
#define exitMenuCode 1

int sidesArena[4][2] = { {5,3}, {11,6},{22,12}, {33,18} };
int /*columnsArena = 11,
rowsArena = 6,*/
levelOfGame = 1,
currentScore = 0,
currentTime = 0,
currentRemainingLifes = 0;
bool firstStart = true,
//isArenaReload = false,
soundOn = true,
extraLifesOn = true,
flagsOn = true;

std::vector <std::vector<bool>> arenaGrass(sidesArena[levelOfGame][0], std::vector <bool>(sidesArena[levelOfGame][1], true)); //2d array in vector (vector array of booleans, with vectors of booleans inside)
std::vector <std::vector<int>> arenaGround(sidesArena[levelOfGame][0], std::vector <int>(sidesArena[levelOfGame][1], 0)); //2d array in vector (vector array of booleans, with vectors of booleans inside)
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
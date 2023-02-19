#include "main.h"


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, int cmdShow) {
	HWND hWnd;
	MSG msg;
	hInst = hInstance;

	WNDCLASS saperWnd;
	saperWnd.hInstance = hInstance;
	saperWnd.lpszClassName = L"SaperWnd";
	saperWnd.lpfnWndProc = WndProc;
	saperWnd.style = CS_BYTEALIGNWINDOW;// - dont need
	saperWnd.hIcon = NULL;// LoadIcon(NULL, NULL);
	saperWnd.hCursor = LoadCursor(hInst, MAKEINTRESOURCE(IDC_SHOVEL_CURSOR));
	saperWnd.lpszMenuName = NULL;
	saperWnd.cbClsExtra = 0;		//-dont need
	saperWnd.cbWndExtra = 0;
	saperWnd.hbrBackground = CreateSolidBrush(RGB(0, 153, 0));

	if (!RegisterClass(&saperWnd)) //if trouble with class - go out
		return 0;

	hWnd = CreateWindow(L"SaperWnd", L"Saper 2.0",
		WS_POPUPWINDOW,
		0,
		0,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		HWND_DESKTOP,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
		return MessageBox(hWnd, L"HWND trouble...", L"Oops!", MB_OK | MB_ICONWARNING);

	ShowWindow(hWnd, cmdShow);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	HDC hdc; 
	static HDC memBit;
	POINT cursorPosition;
	PAINTSTRUCT lp;

	static BITMAP bitmap;
	HBITMAP heartBmp, flagBmp, soundBmp;

	HBRUSH standartBrush, grassBrush, groundBrush, bombBrush, mainBackgrounfBrush;
	HPEN standartPen, grassPen, groundPen, bombPen;
	HFONT standartFont, numbersFont, gameInfoFont;

	static RECT currentBlock, scoreRect, timeRect, startTextRect, infoIconsRect/*heartBarRect*/, flagRect, soundRect;
	static int blockSide{ 0 },
		wndWidth{ 0 }, wndHeight{ 0 },
		heartCentering{ 0 };
    static std::string scoreStr, timeStr;

	switch (message) {
	case WM_CREATE:
		WndMenu(hWnd);

		heartBmp = (HBITMAP)LoadImage(NULL, L"assets/bmp/heart.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		flagBmp = (HBITMAP)LoadImage(NULL, L"assets/bmp/flag.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		soundBmp = (HBITMAP)LoadImage(NULL, L"assets/bmp/sound.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		/*if (soundBmp == NULL)
		{
			MessageBox(hWnd, TEXT("Файл не знайдено"), TEXT("Завантаження бітмапу soundBmp"),
				MB_OK | MB_ICONHAND);
			DestroyWindow(hWnd);
			return 1;
		}*/

		GetObject(heartBmp, sizeof(bitmap), &bitmap);
		hdc = GetDC(hWnd);
		memBit = CreateCompatibleDC(hdc);
		ReleaseDC(hWnd, hdc);

		InvalidateRect(hWnd, NULL, true);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case settingsMenuCode:
			KillTimer(hWnd, 1);
			DialogBoxW(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, SettingsDialog);
			if (firstStart) {
				blockSide = wndHeight / (sidesArena[levelOfGame][1] + 2); // - wndHeight / 1.15;
				arena.right = wndWidth - (wndWidth - blockSide * sidesArena[levelOfGame][0]) / 2;
				arena.bottom = blockSide + blockSide * sidesArena[levelOfGame][1];
				arena.left = (wndWidth - blockSide * sidesArena[levelOfGame][0]) / 2;
				arena.top = blockSide;

				scoreRect.left = arena.left;
				scoreRect.right = arena.left + blockSide * 1.5;
				scoreRect.top = arena.top / 3;
				scoreRect.bottom = 2 * arena.top / 3; //2:3 size

				timeRect.left = arena.right - blockSide * 1.5;
				timeRect.right = arena.right;
				timeRect.top = arena.top / 3;
				timeRect.bottom = 2 * arena.top / 3;

				startTextRect.left = arena.left;
				startTextRect.right = arena.right;
				startTextRect.top = arena.top + (arena.bottom - arena.top) / 3;
				startTextRect.bottom = startTextRect.top + (arena.bottom - arena.top) / 3;

				infoIconsRect.left = wndWidth / 2 - bitmap.bmWidth * 4;
				infoIconsRect.top = arena.top / 2 - bitmap.bmHeight / 2;
				infoIconsRect.right = wndWidth / 2 + bitmap.bmWidth * 4;
				infoIconsRect.bottom = infoIconsRect.top + bitmap.bmHeight;

				soundRect.left = wndWidth / 2 + bitmap.bmWidth * 3;
				soundRect.top = arena.top / 2 - bitmap.bmHeight / 2;
				soundRect.right = bitmap.bmWidth;
				soundRect.bottom = bitmap.bmHeight;

				InvalidateRect(hWnd, NULL, true);
			}
			break;
		case exitMenuCode:
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_LBUTTONDOWN:
		GetCursorPos(&cursorPosition);
		cursorPosition.y -= GetSystemMetrics(SM_CYCAPTION); //because Getcurs return po on screen, but not on client rect
		if (IsPointInRect(cursorPosition, arena)) {
			if (firstStart) {
				currentRemainingLifes = levelOfGame;//hearts max count
				GenerateBombs(cursorPosition, blockSide);
				CalculateNumberBlocks();
				firstStart = false;
				SetTimer(hWnd, 1, 1000, NULL);
			}
			//air digg
		//currentBlockRect = GetRectOfClickedBlock(cursorPosition, blockSide);
			DiggingGrassToCoords(cursorPosition, blockSide);
			if (currentRemainingLifes == 0) {
				KillTimer(hWnd, 1);
				InvalidateRect(hWnd, &arena, true);
				InvalidateRect(hWnd, &infoIconsRect, true);//for crack last heart
				DialogBoxW(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, GameOverDialog);
			}
			//if (!firstStart) {
				InvalidateRect(hWnd, &arena, true); //InvalidateRect(hWnd, &currentBlockRect, false);
				InvalidateRect(hWnd, &scoreRect, true);
				InvalidateRect(hWnd, &infoIconsRect, true);
			//}
			//InvalidateRect(hWnd, NULL, true);
		}

		if (IsPointInRect(cursorPosition, soundRect)) {
			soundOn = !soundOn;
			//music off
			InvalidateRect(hWnd, &soundRect, true);
		}

		break;

	case WM_SIZE:
		wndHeight = HIWORD(lParam);
		wndWidth = LOWORD(lParam);
		blockSide = wndHeight / (sidesArena[levelOfGame][1] + 2); // - wndHeight / 1.15;

		arena.right = wndWidth - (wndWidth - blockSide * sidesArena[levelOfGame][0]) / 2;
		arena.bottom = blockSide + blockSide * sidesArena[levelOfGame][1];
		arena.left = (wndWidth - blockSide * sidesArena[levelOfGame][0]) / 2;
		arena.top = blockSide;

		scoreRect.left = arena.left;
		scoreRect.right = arena.left + blockSide * 1.5;
		scoreRect.top = arena.top / 3;
		scoreRect.bottom = 2 * arena.top / 3; //2:3 size

		timeRect.left = arena.right - blockSide * 1.5;
		timeRect.right = arena.right;
		timeRect.top = arena.top / 3;
		timeRect.bottom = 2 * arena.top / 3;

		startTextRect.left = arena.left;
		startTextRect.right = arena.right;
		startTextRect.top = arena.top + (arena.bottom - arena.top) / 3;
		startTextRect.bottom = startTextRect.top + (arena.bottom - arena.top) / 3;

		infoIconsRect.left = wndWidth / 2 - bitmap.bmWidth * 4;
		infoIconsRect.top = arena.top / 2 - bitmap.bmHeight / 2;
		infoIconsRect.right = wndWidth / 2 + bitmap.bmWidth * 4;
		infoIconsRect.bottom = infoIconsRect.top + bitmap.bmHeight;

		soundRect.left = wndWidth / 2 + bitmap.bmWidth * 3;
		soundRect.top = arena.top / 2 - bitmap.bmHeight / 2;
		soundRect.right = bitmap.bmWidth;
		soundRect.bottom = bitmap.bmHeight;

		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &lp);

					//info
		scoreStr = "Score: " + std::to_string(currentScore);
		timeStr = "Time: " + std::to_string(currentTime);

		gameInfoFont = CreateFont(blockSide / 3, blockSide / 8, 0, 0, FW_DONTCARE,
			0, 0, 0, ANSI_CHARSET,
			OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
			DRAFT_QUALITY, VARIABLE_PITCH,
			TEXT("Comic Sans MS"));

		standartFont = (HFONT)SelectObject(hdc, gameInfoFont);
		standartPen = (HPEN)GetStockObject(WHITE_BRUSH);
		standartBrush = (HBRUSH)GetStockObject(DC_PEN);

		SelectObject(hdc, gameInfoFont);
		SetTextColor(hdc, RGB(0, 0, 0));
		SetBkMode(hdc, TRANSPARENT);

		DrawText(hdc, std::wstring(scoreStr.begin(), scoreStr.end()).c_str(),
			scoreStr.length(), &scoreRect, DT_LEFT);
		DrawText(hdc, std::wstring(timeStr.begin(), timeStr.end()).c_str(),
			timeStr.length(), &timeRect, DT_RIGHT);

		SelectObject(hdc, standartFont);
		DeleteObject(gameInfoFont);

			//bmp`s
			mainBackgrounfBrush = CreateSolidBrush(RGB(0, 153, 0));
			//memBit = CreateCompatibleDC(hdc);
			if (!firstStart) {
				SelectObject(hdc, mainBackgrounfBrush);
				//heart bar
				if (extraLifesOn) {
					for (int i = 1; i <= levelOfGame; i++) {
						heartBmp = (i <= (levelOfGame - currentRemainingLifes)) ? //which heart crack
							(HBITMAP)LoadImage(NULL, L"assets/bmp/lost_heart.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION) :
							(HBITMAP)LoadImage(NULL, L"assets/bmp/heart.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);


						SelectObject(memBit, heartBmp);
						heartCentering = (levelOfGame == 3) ? bitmap.bmWidth * 1.5 : (levelOfGame == 2) ? bitmap.bmWidth : bitmap.bmWidth * .5/*(levelOfGame==1)?*/;
						BitBlt(hdc,
							wndWidth / 2 + heartCentering - bitmap.bmWidth * i,
							arena.top / 2 - bitmap.bmHeight / 2,
							bitmap.bmWidth,
							bitmap.bmHeight,
							memBit, 0, 0, MERGECOPY);

						DeleteObject(heartBmp);
					}
				}
				
				//flag Bar
				flagBmp = (flagsOn) ?
					(HBITMAP)LoadImage(NULL, L"assets/bmp/flag.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION) :
					(HBITMAP)LoadImage(NULL, L"assets/bmp/unflag.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
				SelectObject(memBit, flagBmp);
				BitBlt(hdc,
					wndWidth / 2 - bitmap.bmWidth * 4,
					arena.top / 2 - bitmap.bmHeight / 2,
					bitmap.bmWidth,
					bitmap.bmHeight,
					memBit, 0, 0, MERGECOPY);
				
				//Sound Bar
				soundBmp = (soundOn) ?
					(HBITMAP)LoadImage(NULL, L"assets/bmp/sound.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION) :
					(HBITMAP)LoadImage(NULL, L"assets/bmp/unsound.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
				SelectObject(memBit, soundBmp);
				BitBlt(hdc,
					wndWidth / 2 + bitmap.bmWidth * 3,
					arena.top / 2 - bitmap.bmHeight / 2,
					bitmap.bmWidth,
					bitmap.bmHeight,
					memBit, 0, 0, MERGECOPY);
				
				DeleteObject(flagBmp);
				DeleteObject(soundBmp);
			}
			//DeleteObject(memBit);
			SelectObject(hdc, standartBrush);
			DeleteObject(mainBackgrounfBrush);

					//arena draw
			Rectangle(hdc, arena.left, arena.top, arena.right, arena.bottom);

			for (int i = 0; i < sidesArena[levelOfGame][0]; i++)
				for (int j = 0; j < sidesArena[levelOfGame][1]; j++) {
					if ((i + j) & 1) {//is even numb
						grassBrush = CreateSolidBrush(RGB(0, 255, 0));
						grassPen = CreatePen(BS_SOLID, 1, RGB(0, 255, 0));
						groundBrush = CreateSolidBrush(RGB(255, 204, 102));
						groundPen = CreatePen(BS_SOLID, 1, RGB(255, 204, 102));
					}
					else {
						grassBrush = CreateSolidBrush(RGB(0, 204, 0));
						grassPen = CreatePen(BS_SOLID, 1, RGB(0, 204, 0));
						groundBrush = CreateSolidBrush(RGB(255, 178, 153));
						groundPen = CreatePen(BS_SOLID, 1, RGB(255, 178, 153));
					}

					if (arenaGrass[i][j]) {//grass
						SelectObject(hdc, grassPen);
						SelectObject(hdc, grassBrush);

						Rectangle(hdc,
							arena.left + blockSide * i,
							arena.top + blockSide * j,
							arena.left + blockSide * i + blockSide,
							arena.top + blockSide * j + blockSide);

						SelectObject(hdc, standartPen);
						DeleteObject(grassPen);
						SelectObject(hdc, standartBrush);
						DeleteObject(grassBrush);
					}
					else {
						//ground
						SelectObject(hdc, groundPen);
						SelectObject(hdc, groundBrush);

						Rectangle(hdc, arena.left + blockSide * i,
							arena.top + blockSide * j,
							arena.left + blockSide * i + blockSide,
							arena.top + blockSide * j + blockSide);

						SelectObject(hdc, standartBrush);
						DeleteObject(groundBrush);
						SelectObject(hdc, standartPen);
						DeleteObject(groundPen);

						if (arenaGround[i][j] == 10) {
							bombBrush = CreateSolidBrush(RGB(192, 192, 192));	
							bombPen = CreatePen(BS_SOLID, 4, RGB(32, 32, 32));
							SelectObject(hdc, bombPen);
							SelectObject(hdc, bombBrush);

							Ellipse(hdc,
								arena.left + blockSide * i + 40,
								arena.top + blockSide * j + 40,
								arena.left + blockSide * i + blockSide - 40,
								arena.top + blockSide * j + blockSide - 40);

							SelectObject(hdc, standartPen);
							DeleteObject(bombPen);
							SelectObject(hdc, standartBrush);
							DeleteObject(bombBrush);
						}

						else if (arenaGround[i][j] > 0) {//draw count of bombs around
							numbersFont = CreateFont(blockSide / .9, blockSide / 2.5, 0, 0, FW_DONTCARE,
								0, 0, 0, ANSI_CHARSET,
								OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
								DRAFT_QUALITY, VARIABLE_PITCH,
								TEXT("Comic Sans MS"));
							SelectObject(hdc, numbersFont);
							SetBkMode(hdc, TRANSPARENT);
							SetTextColor(hdc, RGB(30 * arenaGround[i][j], 0, 15 * arenaGround[i][j]));

							currentBlock.left = arena.left + blockSide * i;
							currentBlock.top = arena.top + blockSide * j;
							currentBlock.right = arena.left + blockSide * i + blockSide;
							currentBlock.bottom = arena.top + blockSide * j + blockSide;

							std::string countOfBombs = std::to_string(arenaGround[i][j]);
							DrawText(hdc,
								std::wstring(countOfBombs.begin(), countOfBombs.end()).c_str(), //convert str to lpcwstr
								1, &currentBlock, DT_CENTER);
							SelectObject(hdc, standartFont);
							DeleteObject(numbersFont);
						}

					}
				}

			if (firstStart) {
				numbersFont = CreateFont(blockSide / .9, blockSide / 2.5, 0, 0, FW_DONTCARE,
					0, 0, 0, ANSI_CHARSET,
					OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
					DRAFT_QUALITY, VARIABLE_PITCH,
					TEXT("Comic Sans MS"));
				SelectObject(hdc, numbersFont);
				SetBkMode(hdc, TRANSPARENT);
				SetTextColor(hdc, RGB(0, 0, 0));
				DrawText(hdc, TEXT("Click on the arena to start."),
					29, &startTextRect, DT_CENTER);
				SelectObject(hdc, standartFont);
				DeleteObject(numbersFont);
			}

		EndPaint(hWnd, &lp);	
		break;

	case WM_TIMER:
		currentTime++;
		InvalidateRect(hWnd, &timeRect, true);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK SettingsDialog(HWND hWndDlg, UINT messageDlg, WPARAM wParam, LPARAM lParam) {

	switch (messageDlg) {
	case WM_INITDIALOG:
		CheckRadioButton(hWndDlg, IDC_RADIO1, IDC_RADIO3, IDC_RADIO1 + levelOfGame - 1);
		SendDlgItemMessage(hWndDlg, IDC_CHECK_SOUND, BM_SETCHECK, soundOn, 0);
		SendDlgItemMessage(hWndDlg, IDC_CHECK_LIFES, BM_SETCHECK, extraLifesOn, 0);
		SendDlgItemMessage(hWndDlg, IDC_CHECK_FLAGS, BM_SETCHECK, flagsOn, 0);

		return INT_PTR(true);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_RADIO1:
			levelOfGame = 1;
			break;

		case IDC_RADIO2:
			levelOfGame = 2;
			break;

		case IDC_RADIO3:
			levelOfGame = 3;
			break;

		case IDC_CHECK_SOUND:
			soundOn = !soundOn;
			break;

		case IDC_CHECK_LIFES:
			extraLifesOn = !extraLifesOn;
			break;

		case IDC_CHECK_FLAGS:
			flagsOn = !flagsOn;
			break;

		case IDOK:
			//isArenaReload = true;
			RestartGame();
			EndDialog(hWndDlg, LOWORD(wParam));
			//case IDCANCEL:

			break;
		}

		break;
	//case WM_CTLCOLORDLG:
	//	return (INT_PTR)CreateSolidBrush(RGB(0, 255, 0));
	//	break;
	//case WM_DESTROY:

	//	break;

	}
	return INT_PTR(false);
}

INT_PTR CALLBACK GameOverDialog(HWND hWndGO, UINT message, WPARAM wParam, LPARAM lParam) {
	static HDC hdcDlg;
	static PAINTSTRUCT ps;
	static HBRUSH background;
	static std::string finalScore, finalTime;
	static HFONT tittleFont;
	static RECT dialogRect, tittleRect;
	static int widthDlg, heightDlg;

	switch (message) {
	case WM_INITDIALOG:
		background= CreateSolidBrush(RGB(0, 255, 0));

		finalScore = "Score: " + std::to_string(currentScore);
		finalTime = "Time: " + std::to_string(currentTime);
		SetDlgItemText(hWndGO, IDC_SCORE, std::wstring(finalScore.begin(), finalScore.end()).c_str());
		SetDlgItemText(hWndGO, IDC_TIME, std::wstring(finalTime.begin(), finalTime.end()).c_str());

		GetClientRect(hWndGO, &dialogRect);

		/*widthDlg = dialogRect.right - dialogRect.left;
		heightDlg = dialogRect.bottom - dialogRect.top;*/

		tittleRect.left = 10;
		tittleRect.right = dialogRect.right -10;
		tittleRect.top = 30;
		tittleRect.bottom = dialogRect.bottom / 2.5;
		return INT_PTR(true);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			//isArenaReload = true;
			RestartGame();
			EndDialog(hWndGO, LOWORD(wParam));
			break;
		case IDCANCEL:
			PostQuitMessage(0);
			break;
		}
			break;
	case WM_CTLCOLORDLG:
		return (INT_PTR)background;//CreateSolidBrush(RGB(0, 255, 0));
		break;

	case WM_PAINT:
		tittleFont = CreateFont(40, 15, 0, 0, FW_DONTCARE,
			0, 0, 0, ANSI_CHARSET,
			OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
			DRAFT_QUALITY, VARIABLE_PITCH,
			TEXT("Comic Sans MS"));

		hdcDlg = BeginPaint(hWndGO, &ps);
		//FillRect(hdcDlg, &dialogRect, (HBRUSH)CreateSolidBrush(RGB(0, 255, 0)));
		SelectObject(hdcDlg, tittleFont);
		SetBkMode(hdcDlg, TRANSPARENT);
		SetTextColor(hdcDlg, RGB(0, 0, 0));
		DrawText(hdcDlg, (currentRemainingLifes) ? L"You Won!!!" : L"You Lost..", 11, &tittleRect, DT_CENTER);

		BeginPaint(hWndGO, &ps);

		DeleteObject(tittleFont);
		break;
	case WM_DESTROY:
		DeleteObject(background);
		break;
	}
	return INT_PTR(false);
}


void WndMenu(HWND hWnd) {
	HMENU mainMenu = CreateMenu();

	AppendMenu(mainMenu, MF_POPUP, settingsMenuCode, L"Settings");
	AppendMenu(mainMenu, MF_POPUP, exitMenuCode, L"Exit");

	SetMenu(hWnd, mainMenu);
}

bool IsPointInRect(POINT point, RECT rect) {
	return (point.x <= rect.right
		&& point.x >= rect.left
		&& point.y >= rect.top
		&& point.y <= rect.bottom) ?
		true : false;
}

POINT GetIndexGrassBlockClicked(POINT point, int side) {
	RECT currentGrassBlock; /*= GetRectOfClickedBlock(point, side);*/

	for (int i = 0; i < sidesArena[levelOfGame][0]; i++)
		for (int j = 0; j < sidesArena[levelOfGame][1]; j++) {
			currentGrassBlock.left = arena.left + side * i;
			currentGrassBlock.right = arena.left + side * i + side;
			currentGrassBlock.top = arena.top + side * j;
			currentGrassBlock.bottom = arena.top + side * j + side;
			if (IsPointInRect(point, currentGrassBlock))
				return POINT{ i, j };
		}
	return POINT{ -1, -1 };
}

void DiggingGrassToCoords(POINT point, int side) {
	int i = GetIndexGrassBlockClicked(point, side).x,
		j = GetIndexGrassBlockClicked(point, side).y;
	if (i >= 0 && j >= 0 && arenaGrass[i][j]) {
		arenaGrass[i][j] = false;

		switch (arenaGround[i][j]) {
		case 10:
			(extraLifesOn)?
				currentRemainingLifes--:
				currentRemainingLifes = 0;
			break;
		case 0:
			DiggingVoidArea(i, j);
			break;
		default://if it`s number block
			currentScore += arenaGround[i][j];
		}

	}
}

void DiggingVoidArea(int i, int j) {
	for (int k = i - 1; k <= i + 1; k++)
		for (int l = j - 1; l <= j + 1; l++)
			if (k >= 0 && l >= 0 && k < sidesArena[levelOfGame][0] && l < sidesArena[levelOfGame][1])
				if (arenaGrass[k][l])
					if (arenaGround[k][l] != 0 && arenaGround[k][l] != 10) {
						currentScore += arenaGround[k][l];
						arenaGrass[k][l] = false;
					}
					else if (arenaGround[k][l] != 10) {
						arenaGrass[k][l] = false;
						DiggingVoidArea(k, l);
					}
}

void GenerateBombs(POINT point, int side) {
	int col = GetIndexGrassBlockClicked(point, side).x,
		row = GetIndexGrassBlockClicked(point, side).y;

	for (int i = 0; i < sidesArena[levelOfGame][0]; i++)
		for (int j = 0; j < sidesArena[levelOfGame][1]; j++)
			if (rand() % int(6-levelOfGame*1.1) == 0)
				arenaGround[i][j] = 10;

	for (int k = col - 1; k <= col + 1; k++)//start void area
		for (int l = row - 1; l <= row + 1; l++)
			if (k >= 0 && l >= 0 && k < sidesArena[levelOfGame][0] && l < sidesArena[levelOfGame][1])
				arenaGround[k][l] = 0;
}

void CalculateNumberBlocks() {
	for (int i = 0; i < sidesArena[levelOfGame][0]; i++)
		for (int j = 0; j < sidesArena[levelOfGame][1]; j++)
			if (arenaGround[i][j] != 10)
				for (int k = i - 1; k <= i + 1; k++)
					for (int l = j - 1; l <= j + 1; l++)
						if (k >= 0 && l >= 0 && k < sidesArena[levelOfGame][0] && l < sidesArena[levelOfGame][1])
							if (arenaGround[k][l] == 10)
								arenaGround[i][j]++;
}

void RestartGame() {
	currentScore = 0;
	currentTime = 0;
	currentRemainingLifes = levelOfGame;
	firstStart = true;
	//isArenaReload = false;

	for (int i = 0; i < arenaGrass.size()-1/*sidesArena[levelOfGame][1]*/; i++) {
		arenaGrass.at(i).clear();
		arenaGround.at(i).clear();
	}
	arenaGrass.clear();
	arenaGround.clear();

	for (int i = 0; i < sidesArena[levelOfGame][0]; i++) {
		arenaGrass.push_back(/*at(i)*/ std::vector <bool>(sidesArena[levelOfGame][1], true));
		arenaGround.push_back(/*at(i)*/ std::vector <int>(sidesArena[levelOfGame][1], 0));
	}
		/*for (int j = 0; j < rowsArena; j++) {
			arenaGrass[i][j] = true;
			arenaGround[i][j] = 0;
		}*/
}

// DrawingApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include <iostream>
#include <string>
#include <fstream>
#include <thread>

#include "BrushPoint.h"
#include "List.h"
#include "Button.h"

#pragma comment(lib, "winmm.lib")

using namespace std;

enum Colors
{
	B_BLACK = 0,
	F_BLACK = 0,
	B_RED = BACKGROUND_RED,
	F_RED = FOREGROUND_RED,
	B_BLUE = BACKGROUND_BLUE,
	F_BLUE = FOREGROUND_BLUE,
	B_GREEN = BACKGROUND_GREEN,
	F_GREEN = FOREGROUND_GREEN,
	B_BRIGHTEN = BACKGROUND_INTENSITY,
	F_BRIGHTEN = FOREGROUND_INTENSITY
};

enum Menus
{
	MENU_INTRO = 0,
	MENU_MAIN = 2,
	MENU_GAME = 3,
	MENU_DLC = 4
};

//VARIABLES------------------------------------------
int currentMenu;
int timer;
int drawTimer;
bool firstLoad;
bool isFreeDraw;

const int buttonsCount = 10;
int buttonTotalCount = buttonsCount;
Button buttons[buttonsCount];

const int NORMAL_MIN = 20;
const int HARD_MIN = 30;
const int AUSTERE_MIN = 10;
const int DLC_DESSERT_MIN = 2;

BrushPoint cursor;
POINT lastPos;
int mouseColor;
int mouseColorBack;
bool tabDown;
bool mouseDown;
bool mouseLeftClicked;

POINT splatterPos;
bool splatterOn;
int shotRemaining;
int shotRemainingLast;

const int LEVELS_COUNT = 20;
int levelScoresBest[LEVELS_COUNT];
int currentLevel;
int levelScoreCurrent;

//int brushPointCount;
//List<BrushPoint> brushPoints;
const int CANVAS_WIDTH = 60;
const int CANVAS_HEIGHT = 30;
BrushPoint brushPoints[CANVAS_WIDTH][CANVAS_HEIGHT];
bool fillMode;
bool delayToTraceMode;

bool recentSplatter;
int blinkEndButton = -1;

//DEFS-----------------------------------------------
void Initialize();

void InitializeIntro();

void InitializeMenu();

void InitializeDLCMenu();

void InitializeCanvas();
void NewCanvas(bool border);

void Update();
void Draw();

void LaunchLevels();

void TransitionMenu(int menu);

void ColourButtons();

void DrawButton(Button b);
void DrawColourChart();
void DrawColourDefTwo(int colour1, int colour2, int xPos, int yPos);
void DrawColourDefThree(int colour1, int colour2, int colour3, int xPos, int yPos);

void MakeTraceMode();

void ScoreLevel();

void PlaceText(char text, int posX, int posY, int colorFore, int colorBack);
void PlaceText(char* text, int posX, int posY, int colorFore, int colorBack);
void PlaceNum(int num, int posX, int posY, int colorFore, int colorBack);
void PlaceNum(float num, int posX, int posY, int colorFore, int colorBack);
void DrawChar(char charToDraw, int colorFore, int colorBack);
void DrawChar(char* charToDraw, int colorFore, int colorBack);
void DrawNum(int num, int colorFore, int colorBack);
void DrawNum(float num, int colorFore, int colorBack);
void ClearText(int posX, int posY);

void WipeScreen();

void PlayClickSound();
void PlaySplatterSound();

POINT GetMousePos();
WINDOWPLACEMENT GetWindowData();
void SetWindowData(RECT windowRectangle);
bool IsMouseDownLeft();
bool IsMouseUpLeft();
bool IsMouseDownRight();
bool IsMouseLeftClicked();

int Clamp(int value, int minV, int maxV);
int Random(int base, int max);

void SoundLoop(string soundName);

//MAIN------------------------------------------------

int _tmain(int argc, _TCHAR* argv[])
{

	//Initialize
	Initialize();
	//InitializeCanvas();
	//InitializeMenu();

	while(true)
	{
		timer++;

		Update();

		if(timer % drawTimer == 0)
		{
			Draw();
		}
	}

	return 0;
}

//FUNCTIONS-------------------------------------------

//Save/Load Files
void SaveScreen()
{
	ofstream ofs("LevelTEMP.txt");

	for(int x = 0; x < CANVAS_WIDTH; ++x)
	{
		for(int y = 0; y < CANVAS_HEIGHT; ++y)
		{
			ofs << brushPoints[x][y].ColorBack() << '\n';
		}
	}
}
void LoadScreen(string name)
{
	ifstream ifs(name);

	int xIndex = 0;
	int yIndex = 0;

	NewCanvas(false);

	/*while(!ifs.eof())
	{*/
	for(int x = 0; x < CANVAS_WIDTH; ++x)
	{
		for(int y = 0; y < CANVAS_HEIGHT; ++y)
		{
			char culmValue[255];
			/*char nextText[1];
			ifs.read(nextText, 1);*/

			ifs.getline(culmValue, 255);

			if(culmValue[0] != '0')
				brushPoints[x][y].ColorBack(atoi(culmValue));
		}
	}
		//ifs.getline(
		//if(nextText[0] == '\n')
		//{
		//	if(culmValue == "0")
		//		brushPoints[xIndex][yIndex].Color(B_RED);
		//	//Set brush point to value
		//	/*brushPoints[xIndex][yIndex].ColorBack*/(atoi(culmValue.c_str()));

		//	//Increment index
		//	yIndex++;
		//	if(yIndex >= CANVAS_HEIGHT)
		//	{
		//		yIndex = 0;

		//		xIndex++;
		//		if(xIndex >= CANVAS_WIDTH)
		//			xIndex = 0;
		//	}
		//}
		//else
		//{
		//	culmValue += nextText[0];
		//}
	//}
}

void SaveScores()
{
	ofstream ofs("Scores.txt");

	for(int i = 0; i < LEVELS_COUNT; ++i)
	{
		ofs << levelScoresBest[i] << '\n';
	}
}
void LoadScores()
{
	ifstream ifs("Scores.txt");

	if(ifs == NULL)
		return;

	for(int i = 0; i < LEVELS_COUNT; ++i)
	{
		char culmValue[255];
		ifs.getline(culmValue, 255);

		levelScoresBest[i] = atoi(culmValue);
	}
}

//Initialize called before Update to preset everything
void Initialize()
{
	drawTimer = 1;
	tabDown = false;
	firstLoad = true;

	//Set screen Dimension
	system("mode 100, 70");
	//Set screen colour
	system("color 00");

	//******GET display resolution data
	HWND foreGround = GetForegroundWindow();
	RECT clientRect;
	GetClientRect(foreGround, &clientRect);
	int dwWidth = clientRect.right;
	int dwHeight = clientRect.bottom;

	//******SET cmd window to top left and display size
	RECT windowRec;
	windowRec.left = dwWidth / 4;
	windowRec.top = 0;
	windowRec.bottom = dwHeight;
	windowRec.right = dwWidth * 2;
	SetWindowData(windowRec);

	//*********SET FONT
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_FONT_INFOEX tempfont;
	tempfont.dwFontSize.X = 2;
	tempfont.dwFontSize.Y = 2;
	GetCurrentConsoleFontEx(hConsole, false, &tempfont);
	SetCurrentConsoleFontEx(hConsole, false, &tempfont);

	mouseColor = F_RED;
	mouseColorBack = B_RED;

	//Set random seed
	srand(time(0)); 

	//Load Level Scores
	for(int i = 0; i < LEVELS_COUNT; ++i)
	{
		levelScoresBest[i] = 0;
	}
	LoadScores();

	TransitionMenu(MENU_INTRO);
}
void InitializeIntro()
{
	buttonTotalCount = 0;

	LoadScreen("Levels/SplashScreenWTF.txt");

	//printf("%d", F_RED | F_GREEN | F_BRIGHTEN);
}
void InitializeMenu()
{	
	buttonTotalCount = 17;
	Button play = Button(40, 20, 20, 5, "FREE DRAW", 9,
						F_GREEN | F_BLUE | F_BRIGHTEN, 
						F_GREEN | F_BLUE, 
						F_GREEN | F_BRIGHTEN);
	buttons[0] = play;

	////Level Buttons
	//for(int x = 0; x < 2; ++x)
	//{
	//	for(int y = 0; y < 4; ++y)
	//	{
	//		char* text = "LEVEL";
	//		Button lvl1 = Button(10 + (x * 20), 30 + (y * 6), 
	//						15, 5, text, 7,
	//						F_RED | F_GREEN | F_BLUE | F_BRIGHTEN, 
	//						F_RED | F_BRIGHTEN, F_RED);
	//		buttons[1 + (4 * x) + y] = lvl1;
	//	}
	//}

	int levelButtonColour = F_RED | F_GREEN | F_BLUE | F_BRIGHTEN;
	int levelButtonColourHover = F_GREEN | F_BLUE;
	int levelButtonColourClick = F_GREEN | F_BLUE | F_BRIGHTEN;

	//Normal Levels
	PlaceText("NORMAL:", 14, 28, F_GREEN | F_BRIGHTEN, F_BLACK);
	for(int x = 0; x < 5; ++x)
	{
		int colN = levelButtonColour;
		int colHoverN = levelButtonColourHover;
		int colClickN = levelButtonColourClick;

		if(levelScoresBest[x] >= NORMAL_MIN)
		{
			colN = F_GREEN | F_BRIGHTEN;
			colHoverN = F_GREEN;
			colClickN = F_BLACK;
		}
		else if(x > 0 && levelScoresBest[x - 1] < NORMAL_MIN)
		{
			colN = F_BLACK;
			colHoverN = F_BLACK;
			colClickN = F_BLACK;
		}

		char* text = "LVL";
		Button lvl1 = Button(10 + (x * 15), 30, 
						10, 5, text, 3,
						colN, 
						colHoverN, colClickN);
		lvl1.SetValue(levelScoresBest[x]);
		buttons[1 + x] = lvl1;
	}

	//Hard Levels
	PlaceText("HARD:", 14, 38, F_GREEN | F_RED | F_BRIGHTEN, F_BLACK);
	for(int x = 0; x < 5; ++x)
	{
		int colH = levelButtonColour;
		int colHoverH = levelButtonColourHover;
		int colClickH = levelButtonColourClick;

		if(levelScoresBest[x + 5] >= HARD_MIN)
		{
			colH = F_GREEN | F_RED | F_BRIGHTEN;
			colHoverH = F_GREEN | F_RED;
			colClickH = F_BLACK;
		}
		else if(levelScoresBest[x + 4] < HARD_MIN)
		{
			colH = F_BLACK;
			colHoverH = F_BLACK;
			colClickH = F_BLACK;
		}

		char* text = "LVL";
		Button lvl1 = Button(10 + (x * 15), 40, 
						10, 5, text, 3,
						colH, 
						colHoverH, colClickH);
		lvl1.SetValue(levelScoresBest[5 + x]);
		buttons[6 + x] = lvl1;
	}

	//Austere Levels
	PlaceText("AUSTERE:", 14, 48, F_RED | F_BRIGHTEN, F_BLACK);
	for(int x = 0; x < 5; ++x)
	{
		int colA = levelButtonColour;
		int colHoverA = levelButtonColourHover;
		int colClickA = levelButtonColourClick;

		if(levelScoresBest[x + 10] >= AUSTERE_MIN)
		{
			colA = F_RED | F_BRIGHTEN;
			colHoverA = F_RED;
			colClickA = F_BLACK;
		}
		else if(levelScoresBest[x + 9] < AUSTERE_MIN)
		{
			colA = F_BLACK;
			colHoverA = F_BLACK;
			colClickA = F_BLACK;
		}

		char* text = "LVL";
		Button lvl1 = Button(10 + (x * 15), 50, 
						10, 5, text, 3,
						colA, 
						colHoverA, colClickA);
		lvl1.SetValue(levelScoresBest[10 + x]);
		buttons[11 + x] = lvl1;
	}

	//Add Level Goals
		//Normal
	PlaceText('-', 82, 30, F_GREEN, F_BLACK);
	PlaceText('|', 83, 31, F_GREEN, F_BLACK);
	PlaceText("20 PASS", 82, 32, F_GREEN | F_BRIGHTEN, F_BLACK);
	PlaceText('|', 83, 33, F_GREEN, F_BLACK);
	PlaceText('-', 82, 34, F_GREEN, F_BLACK);
		//Hard
	PlaceText('-', 82, 40, F_RED | F_GREEN, F_BLACK);
	PlaceText('|', 83, 41, F_RED | F_GREEN, F_BLACK);
	PlaceText("30 PASS", 82, 42, F_RED | F_GREEN | F_BRIGHTEN, F_BLACK);
	PlaceText('|', 83, 43, F_RED | F_GREEN, F_BLACK);
	PlaceText('-', 82, 44, F_RED | F_GREEN, F_BLACK);
		//Austere
	PlaceText('-', 82, 50, F_RED, F_BLACK);
	PlaceText('|', 83, 51, F_RED, F_BLACK);
	PlaceText("10 PASS", 82, 52, F_RED | F_BRIGHTEN, F_BLACK);
	PlaceText('|', 83, 53, F_RED, F_BLACK);
	PlaceText('-', 82, 54, F_RED, F_BLACK);

	/*Button lvl1 = Button(10, 30, 10, 5, "LEVEL 1", 9,
						F_RED | F_GREEN | F_BLUE | F_BRIGHTEN, 
						F_RED | F_BRIGHTEN, F_RED);
	buttons[1] = lvl1;
	Button lvl2 = Button(25, 30, 10, 5, "LEVEL 2", 9,
						F_RED | F_GREEN | F_BLUE | F_BRIGHTEN, 
						F_RED | F_BRIGHTEN, F_RED);
	buttons[2] = lvl2;*/

	//Add DLC Button
	Button DLC = Button(10, 60, 10, 5, "*DLC*", 5,
						F_RED | F_BLUE | F_BRIGHTEN, 
						F_RED | F_BLUE, 
						F_GREEN | F_BRIGHTEN);
	buttons[16] = DLC;
}
void InitializeDLCMenu()
{	
	buttonTotalCount = 7;
	Button play = Button(40, 20, 20, 5, "FREE DRAW", 9,
						F_GREEN | F_BLUE | F_BRIGHTEN, 
						F_GREEN | F_BLUE, 
						F_GREEN | F_BRIGHTEN);
	buttons[0] = play;

	////Level Buttons
	//for(int x = 0; x < 2; ++x)
	//{
	//	for(int y = 0; y < 4; ++y)
	//	{
	//		char* text = "LEVEL";
	//		Button lvl1 = Button(10 + (x * 20), 30 + (y * 6), 
	//						15, 5, text, 7,
	//						F_RED | F_GREEN | F_BLUE | F_BRIGHTEN, 
	//						F_RED | F_BRIGHTEN, F_RED);
	//		buttons[1 + (4 * x) + y] = lvl1;
	//	}
	//}

	int levelButtonColour = F_RED | F_GREEN | F_BLUE | F_BRIGHTEN;
	int levelButtonColourHover = F_GREEN | F_BLUE;
	int levelButtonColourClick = F_GREEN | F_BLUE | F_BRIGHTEN;

	//Normal Levels
	PlaceText("DESSERT:", 14, 28, F_RED | F_BLUE, F_BLACK);
	for(int x = 0; x < 5; ++x)
	{
		int colN = levelButtonColour;
		int colHoverN = levelButtonColourHover;
		int colClickN = levelButtonColourClick;

		if(levelScoresBest[15 + x] >= DLC_DESSERT_MIN)
		{
			colN = F_RED | F_BLUE | F_BRIGHTEN;
			colHoverN = F_RED | F_BLUE;
			colClickN = F_BLACK;
		}
		else if(x > 0 && levelScoresBest[15 + x - 1] < DLC_DESSERT_MIN)
		{
			colN = F_BLACK;
			colHoverN = F_BLACK;
			colClickN = F_BLACK;
		}

		char* text = "LVL";
		Button lvl1 = Button(10 + (x * 15), 30, 
						10, 5, text, 3,
						colN, 
						colHoverN, colClickN);
		lvl1.SetValue(levelScoresBest[15 + x]);
		buttons[1 + x] = lvl1;
	}
	//Add Level Goals
		//Normal
	PlaceText('-', 82, 30, F_RED | F_BLUE, F_BLACK);
	PlaceText('|', 83, 31, F_RED | F_BLUE, F_BLACK);
	PlaceText("2 PASS", 82, 32, F_RED | F_BLUE | F_BRIGHTEN, F_BLACK);
	PlaceText('|', 83, 33, F_RED | F_BLUE, F_BLACK);
	PlaceText('-', 82, 34, F_RED | F_BLUE, F_BLACK);

	/*Button lvl1 = Button(10, 30, 10, 5, "LEVEL 1", 9,
						F_RED | F_GREEN | F_BLUE | F_BRIGHTEN, 
						F_RED | F_BRIGHTEN, F_RED);
	buttons[1] = lvl1;
	Button lvl2 = Button(25, 30, 10, 5, "LEVEL 2", 9,
						F_RED | F_GREEN | F_BLUE | F_BRIGHTEN, 
						F_RED | F_BRIGHTEN, F_RED);
	buttons[2] = lvl2;*/

	//Add DLC Button
	Button NORMAL = Button(10, 60, 10, 5, "*NORM*", 6,
						F_RED | F_BLUE | F_BRIGHTEN, 
						F_RED | F_BLUE, 
						F_GREEN | F_BRIGHTEN);
	buttons[6] = NORMAL;
}
void InitializeCanvas()
{
	recentSplatter = false;

	char* startText = "DONE!";

	if(!isFreeDraw)
		startText = "START";

	buttonTotalCount = 7;
	Button play = Button(30, 35, 20, 5, startText, 5,
						F_RED | F_GREEN | F_BLUE | F_BRIGHTEN, 
						F_RED | F_GREEN | F_BRIGHTEN, F_RED | F_GREEN);
	buttons[0] = play;

	//Initialize Brush
	//brushPointCount = dwWidth * dwHeight;
	//brushPoints = List<BrushPoint>();

	NewCanvas(true);

	cursor = BrushPoint(0, 0, F_BLUE | F_BRIGHTEN, B_BLACK, 'O');
	lastPos.x = 0;
	lastPos.y = 0;

	//Instructions
	PlaceText("TAB: change colour", 0, CANVAS_HEIGHT + 5, 
								mouseColor, F_BLACK);

	if(isFreeDraw)
	{
		buttonTotalCount = 1;

		PlaceText("DELETE: clear canvas", 0, CANVAS_HEIGHT + 7, 
									F_GREEN | F_BRIGHTEN, F_BLACK);
		PlaceText("R_SHIFT: save screen", 0, CANVAS_HEIGHT + 9, 
									F_GREEN, F_BLACK);
		PlaceText("ENTER: load level", 0, CANVAS_HEIGHT + 11, 
									F_GREEN, F_BLACK);
		PlaceText("LEFT CLICK: draw", 0, CANVAS_HEIGHT + 13, 
									F_GREEN | F_BRIGHTEN, F_BLACK);
		PlaceText("RIGHT CLICK: erase", 0, CANVAS_HEIGHT + 15, 
									F_GREEN | F_BRIGHTEN, F_BLACK);
		PlaceText("DOWN ARROW + CLICK: splatter effect", 0, CANVAS_HEIGHT + 16, 
									F_GREEN | F_BRIGHTEN, F_BLACK);
		PlaceText("ESC: exit to menu", 0, CANVAS_HEIGHT + 18, 
									F_GREEN | F_BRIGHTEN, F_BLACK);
	}
	else
	{
		//Create Colour Buttons
			//DARK colours
		Button bRed = Button(2, CANVAS_HEIGHT + 8, 5, 3, "D", 1,
						F_RED, F_BLACK | F_BRIGHTEN, F_BLACK);
		buttons[1] = bRed;
		Button bGreen = Button(8, CANVAS_HEIGHT + 8, 5, 3, "D", 1,
						F_GREEN, F_BLACK | F_BRIGHTEN, F_BLACK);
		buttons[2] = bGreen;
		Button bBlue = Button(14, CANVAS_HEIGHT + 8, 5, 3, "D", 1,
						F_BLUE, F_BLACK | F_BRIGHTEN, F_BLACK);
		buttons[3] = bBlue;
			//LIGHT colours
		Button bRedLIGHT = Button(2, CANVAS_HEIGHT + 12, 5, 3, "L", 1,
						F_RED | F_BRIGHTEN, F_BLACK | F_BRIGHTEN, F_BLACK);
		buttons[4] = bRedLIGHT;
		Button bGreenLIGHT = Button(8, CANVAS_HEIGHT + 12, 5, 3, "L", 1,
						F_GREEN | F_BRIGHTEN, F_BLACK | F_BRIGHTEN, F_BLACK);
		buttons[5] = bGreenLIGHT;
		Button bBlueLIGHT = Button(14, CANVAS_HEIGHT + 12, 5, 3, "L", 1,
						F_BLUE | F_BRIGHTEN, F_BLACK | F_BRIGHTEN, F_BLACK);
		buttons[6] = bBlueLIGHT;
	}


	fillMode = false;
}
void NewCanvas(bool border)
{
	for(int x = 0; x < CANVAS_WIDTH; ++x)
	{
		for(int y = 0; y < CANVAS_HEIGHT; ++y)
		{

			int colorB = B_BLACK;
			int colorF = F_BLACK;
			char text = ' ';

			//Add Border
			if((x == 0 || y == 0 || x == CANVAS_WIDTH - 1 || y == CANVAS_HEIGHT - 1) && border)
			{
				colorF = F_RED | F_BLUE | F_GREEN;
				text = 'S';
			}

			brushPoints[x][y] = BrushPoint(x * 1, y * 1, colorF, colorB, text);
		}
	}
}
void Update()
{
	POINT mousePOS = GetMousePos();

	mouseLeftClicked = IsMouseLeftClicked();

	//Update buttons
	for(int i = 0; i < buttonTotalCount; ++i)
	{
		buttons[i].Update(mousePOS);
	}

	if(currentMenu == MENU_INTRO)
	{
		if(IsMouseDownLeft())
		{
			PlayClickSound();
			TransitionMenu(MENU_MAIN);
		}
	}	
	else if(currentMenu == MENU_MAIN || currentMenu == MENU_DLC)
	{
		//PLAY pushed
		isFreeDraw = false;

		//Switch menu -> DLC and DLC -> menu
		if((currentMenu == MENU_MAIN && buttons[16].state == 2)
			|| (currentMenu == MENU_DLC && buttons[6].state == 2))
		{
			if(currentMenu == MENU_MAIN)
				TransitionMenu(MENU_DLC);
			else
				TransitionMenu(MENU_MAIN);
		}

		LaunchLevels();
	}
	else if(currentMenu == MENU_GAME)
	{
		//Blink end button
		if(shotRemaining == 0 && blinkEndButton == -1)
			blinkEndButton = 2000;
		if(blinkEndButton > 0)
			blinkEndButton--;
		if(blinkEndButton > 0)
		{
			//Make button blink
			int rate = 15;
			if(blinkEndButton % (rate * 3) == 0)
				buttons[0].colorWord = F_GREEN | F_BRIGHTEN;
			else if(blinkEndButton % (rate * 2) == 0)
				buttons[0].colorWord = F_RED | F_BRIGHTEN;
			else if(blinkEndButton % (rate) == 0)
				buttons[0].colorWord = F_BLUE | F_GREEN | F_BRIGHTEN;

			buttons[0].stateLast = blinkEndButton;
		}

		//if(buttons[0].state == 2)
		//{
		//	//DONE button clicked
		//	ScoreLevel();
		//}

		//Reset shot data
		shotRemainingLast = shotRemaining;

		if(recentSplatter)
		{
			PlaySplatterSound();
			recentSplatter = false;
		}

		/*ClearText(cursor.X(), cursor.Y());
		cursor.X(mousePOS.x);
		cursor.Y(mousePOS.y);*/

		if(/*IsMouseDownLeft()*/buttons[0].state == 2 && delayToTraceMode && !isFreeDraw)
		{
			PlayClickSound();

			buttons[0].text = "DONE!";
			buttons[0].state = 0;

			MakeTraceMode();
			delayToTraceMode = false;

			ScoreLevel();
		}
		else if(IsMouseDownLeft() && (lastPos.x != mousePOS.x || lastPos.y != mousePOS.y) && !delayToTraceMode)
		{
			lastPos.x = mousePOS.x;
			lastPos.y = mousePOS.y;
			/*BrushPoint bPoint = BrushPoint(mousePOS.x, mousePOS.y,
											F_GREEN, 'A');*/

			int posX = Clamp(mousePOS.x, 0, CANVAS_WIDTH - 1);
			int posY = Clamp(mousePOS.y, 0, CANVAS_HEIGHT - 1);

			if((GetAsyncKeyState(VK_DOWN) || !isFreeDraw) && mouseLeftClicked && shotRemaining != 0
				&& mousePOS.x < CANVAS_WIDTH + 2 && mousePOS.y < CANVAS_HEIGHT + 2)
			{
				//Shot used
				if(!isFreeDraw)
					shotRemaining--;

				/*splatterOn = true;
				splatterPos.x = posX;
				splatterPos.y = posY;*/
				int splatterSize = Random(10, 15);
				int splatterSizeHalf = splatterSize / 2;

				int minX = max(0, posX - splatterSizeHalf);
				int maxX = min(CANVAS_WIDTH - 1, posX + splatterSizeHalf);
				int minY = max(0, posY - splatterSizeHalf);
				int maxY = min(CANVAS_HEIGHT - 1, posY + splatterSizeHalf);
				for(int x = minX; x < maxX; ++x)
				{
					for(int y = minY; y < maxY; ++y)
					{
						if((brushPoints[x][y].IsSketchPoint() && !brushPoints[x][y].ColorBack() == B_BLACK) ||
										(!brushPoints[x][y].IsSketchPoint() && !fillMode))
						{
							int distX = x - posX;
							int distY = y - posY;
							int length = sqrt(distX*distX + distY*distY);
							int accuracyMeasure = (int)((float)length / 1.5f);
							if(length < splatterSizeHalf && Random(0, accuracyMeasure) == 0)
							{
								if(brushPoints[x][y].ColorBack() == B_BRIGHTEN)
								{
									brushPoints[x][y].Color(mouseColor);
									brushPoints[x][y].ColorBack(mouseColorBack);
								}
								else
								{
									brushPoints[x][y].Color(brushPoints[x][y].Color() | mouseColor);
									brushPoints[x][y].ColorBack(brushPoints[x][y].ColorBack() | mouseColorBack);
								}

								brushPoints[x][y].Updated(true);
							}
						}
					}
				}

				//PlaySplatterSound();
				recentSplatter = true;

			}
			else if(((brushPoints[posX][posY].IsSketchPoint() && !brushPoints[posX][posY].ColorBack() == B_BLACK) ||
							(!brushPoints[posX][posY].IsSketchPoint() && !fillMode)) && isFreeDraw)
			{
				brushPoints[posX][posY].Color(mouseColor);
				brushPoints[posX][posY].ColorBack(mouseColorBack);
				brushPoints[posX][posY].Updated(true);
			}

			//if((brushPoints[posX][posY].IsSketchPoint() && !brushPoints[posX][posY].ColorBack() == B_BLACK) ||
			//	(!brushPoints[posX][posY].IsSketchPoint() && !fillMode))
			//{
			//	//if(GetAsyncKeyState(VK_DOWN))
				//{
				//	/*splatterOn = true;
				//	splatterPos.x = posX;
				//	splatterPos.y = posY;*/
				//	int splatterSize = 10;
				//	int splatterSizeHalf = splatterSize / 2;
				//	for(int x = posX - splatterSizeHalf; x < posX + splatterSizeHalf; ++x)
				//	{
				//		for(int y = posY - splatterSizeHalf; y < posY + splatterSizeHalf; ++y)
				//		{
				//			int distX = x - posX;
				//			int distY = y - posY;
				//			int length = sqrt(distX*distX + distY*distY);
				//			if(length < splatterSizeHalf && Random(0, 5) != 1)
				//			{
				//				brushPoints[x][y].Color(mouseColor);
				//				brushPoints[x][y].ColorBack(mouseColorBack);
				//				brushPoints[x][y].Updated(true);
				//			}
				//		}
				//	}
				//}
				//else
				//{
				//	brushPoints[posX][posY].Color(mouseColor);
				//	brushPoints[posX][posY].ColorBack(mouseColorBack);
				//	brushPoints[posX][posY].Updated(true);
				//}
			//}

			ScoreLevel();
		}
		//Delete
		else if(IsMouseDownRight() && isFreeDraw)
		{
			ClearText(mousePOS.x, mousePOS.y);
			/*brushPoints[Clamp(mousePOS.x, 0, CANVAS_WIDTH - 1)][Clamp(mousePOS.y, 0, CANVAS_HEIGHT - 1)].Color(F_BLACK);
			brushPoints[Clamp(mousePOS.x, 0, CANVAS_WIDTH - 1)][Clamp(mousePOS.y, 0, CANVAS_HEIGHT - 1)].ColorBack(B_BLACK);
			brushPoints[Clamp(mousePOS.x, 0, CANVAS_WIDTH - 1)][Clamp(mousePOS.y, 0, CANVAS_HEIGHT - 1)].Updated(true);*/
		}

		//Keyboard Check
		if(GetAsyncKeyState(VK_TAB) != 0 && !tabDown)
		{
			PlayClickSound();

			switch(mouseColor)
			{
				default:
					mouseColor = F_RED;
					mouseColorBack = B_RED;
					break;
				case F_RED:
					mouseColor = F_BLUE;
					mouseColorBack = B_BLUE;
					break;
				case F_BLUE:
					mouseColor = F_GREEN;
					mouseColorBack = B_GREEN;
					break;
				case F_GREEN:
					mouseColor = F_RED | F_BRIGHTEN;
					mouseColorBack = B_RED | B_BRIGHTEN;
					break;
				case F_RED | F_BRIGHTEN:
					mouseColor = F_BLUE | F_BRIGHTEN;
					mouseColorBack = B_BLUE | B_BRIGHTEN;
					break;
				case F_BLUE | F_BRIGHTEN:
					mouseColor = F_GREEN | F_BRIGHTEN;
					mouseColorBack = B_GREEN | B_BRIGHTEN;
					break;
				case F_GREEN | F_BRIGHTEN:
					if(isFreeDraw)
					{
						//Free draw mode gives ALL colours
						mouseColor = F_RED | F_BLUE;
						mouseColorBack = B_RED | B_BLUE;
					}
					else
					{
						//Acctual levels can only use light/dark primaries
						mouseColor = F_RED;
						mouseColorBack = B_RED;
					}
					break;
				case F_RED | F_BLUE:
					mouseColor = F_RED | F_BLUE | F_BRIGHTEN;
					mouseColorBack = B_RED | B_BLUE | B_BRIGHTEN;
					break;
				case F_RED | F_BLUE | F_BRIGHTEN:
					mouseColor = F_RED | F_GREEN;
					mouseColorBack = B_RED | B_GREEN;
					break;
				case F_RED | F_GREEN:
					mouseColor = F_RED | F_GREEN | F_BRIGHTEN;
					mouseColorBack = B_RED | B_GREEN | B_BRIGHTEN;
					break;
				case F_RED | F_GREEN | F_BRIGHTEN:
					mouseColor = F_BLUE | F_GREEN;
					mouseColorBack = B_BLUE | B_GREEN;
					break;
				case F_BLUE | F_GREEN:
					mouseColor = F_BLUE | F_GREEN | F_BRIGHTEN;
					mouseColorBack = B_BLUE | B_GREEN | B_BRIGHTEN;
					break;
				case F_BLUE | F_GREEN | F_BRIGHTEN:
					mouseColor = F_BLUE | F_GREEN | F_RED;
					mouseColorBack = B_BLUE | B_GREEN | B_RED;
					break;
				case F_BLUE | F_GREEN | F_RED:
					mouseColor = F_BLUE | F_GREEN | F_RED | F_BRIGHTEN;
					mouseColorBack = B_BLUE | B_GREEN | B_RED | B_BRIGHTEN;
					break;
			}

			PlaceText("TAB: change colour", 0, CANVAS_HEIGHT + 5, 
						mouseColor, F_BLACK);

			tabDown = true;
		}
		else if(GetAsyncKeyState(VK_TAB) == 0 && tabDown)
		{
			tabDown = false;
		}

		if(!isFreeDraw)
			ColourButtons();

		//Save
		if(GetAsyncKeyState(VK_RSHIFT) != 0)
		{
			PlayClickSound();
			SaveScreen();
		}
		if(GetAsyncKeyState(VK_RETURN) != 0)
		{
			PlayClickSound();
			LoadScreen("LevelTEMP.txt");
		}
		if(GetAsyncKeyState(VK_DELETE) != 0)
		{
			PlayClickSound();
			InitializeCanvas();
		}
		/*if(GetAsyncKeyState(VK_LCONTROL) != 0)
		{
			MakeTraceMode();
		}*/
		if(GetAsyncKeyState(VK_ESCAPE) != 0 || buttons[0].state == 2) //ESC or Done! Button clicked
		{
			PlayClickSound();

			//Save Level Score if higher
			if(!isFreeDraw)
			{
				if(levelScoresBest[currentLevel] < levelScoreCurrent)
				{
					levelScoresBest[currentLevel] = levelScoreCurrent;
					SaveScores();
				}
			}

			TransitionMenu(MENU_MAIN);
		}
		//if(GetAsyncKeyState(VK_LEFT) != 0)
		//{
		//	PlayClickSound();
		//}
	}

}
void Draw()
{
	//Draw Cursor
	/*PlaceText(cursor.Text(), 
					cursor.X(), cursor.Y(),
					cursor.Color(), B_BLACK);*/
	//brushPoints[Clamp((int)lastPos.x, 0, CANVAS_WIDTH - 1)][Clamp((int)lastPos.y, 0, CANVAS_HEIGHT - 1)].Updated(true);

	//Draw buttons
	for(int i = 0; i < buttonTotalCount; ++i)
	{
		int state = buttons[i].state;
		int stateLast = buttons[i].stateLast;

		if(state != stateLast || buttons[i].firstDraw)
		{
			DrawButton(buttons[i]);
			buttons[i].firstDraw = false;
		}
	}

	if(currentMenu == MENU_GAME || currentMenu == MENU_INTRO)
	{
		//Draw all stored points
		/*for(int i = 0; i < brushPoints.length; ++i)
		{
			if(brushPoints.Get(i).Updated())
			{
				PlaceText(brushPoints.Get(i).Text(), 
							brushPoints.Get(i).X(), brushPoints.Get(i).Y(),
							brushPoints.Get(i).Color(), B_BLACK);
				brushPoints.Get(i).Updated(false);
			}
		}*/

		for(int x = 0; x < CANVAS_WIDTH; ++x)
		{
			for(int y = 0; y < CANVAS_HEIGHT; ++y)
			{
				if(brushPoints[x][y].Updated())
				{
					PlaceText(brushPoints[x][y].Text(), 
								brushPoints[x][y].X(), brushPoints[x][y].Y(),
								brushPoints[x][y].Color(), brushPoints[x][y].ColorBack());
					brushPoints[x][y].Updated(false);
				}
			}
		}
	}

	if(currentMenu == MENU_GAME)
	{
		//Draw Shots Remaining
		if(shotRemainingLast != shotRemaining)
		{
			PlaceText("Shots Remaining: ", CANVAS_WIDTH + 2, 4, F_RED | F_GREEN, F_BLACK);
			PlaceText("                                        ", CANVAS_WIDTH + 2, 5, F_GREEN | F_BRIGHTEN, F_BLACK);
			for(int i = 0; i < shotRemaining; ++i)
			{
				PlaceText('X', CANVAS_WIDTH + 2 + i, 5, F_RED | F_GREEN | F_BRIGHTEN, F_BLACK);
			}
		}
	}

}

void LaunchLevels()
{
	bool transitioned = false;

	//Free Draw
		if(buttons[0].state == 2)
		{
			PlayClickSound();
			isFreeDraw = true;
			TransitionMenu(MENU_GAME);
			delayToTraceMode = false;
			transitioned = true;
		}
	//Normal Levels
		else if(buttons[1].state == 2)
		{
			char* lvlName = "Levels/LevelBadPomegranate.txt";
			currentLevel = 0;

			if(currentMenu == MENU_DLC)
			{
				lvlName = "Levels/DessertsDLC/LevelBlueBerryPie.txt";
				currentLevel = 15;
			}

			PlayClickSound();
			TransitionMenu(MENU_GAME);

			LoadScreen(lvlName);

			shotRemaining = 7;
			//shotRemaining = 40;
			transitioned = true;
		}
		else if(buttons[2].state == 2 && 
			((levelScoresBest[0] >= NORMAL_MIN && currentMenu == MENU_MAIN)
			|| (levelScoresBest[15] >= DLC_DESSERT_MIN && currentMenu == MENU_DLC)))
		{
			char* lvlName = "Levels/LevelPear.txt";
			currentLevel = 1;

			if(currentMenu == MENU_DLC)
			{
				lvlName = "Levels/DessertsDLC/LevelCherryPie.txt";
				currentLevel = 16;
			}

			PlayClickSound();
			TransitionMenu(MENU_GAME);
			LoadScreen(lvlName);
			//LoadScreen("Levels/DessertsDLC/LevelCherryPie.txt");
			shotRemaining = 6;
			//shotRemaining = 40;
			transitioned = true;
		}
		else if(buttons[3].state == 2 && 
			((levelScoresBest[1] >= NORMAL_MIN && currentMenu == MENU_MAIN)
			|| (levelScoresBest[16] >= DLC_DESSERT_MIN && currentMenu == MENU_DLC)))
		{
			char* lvlName = "Levels/LevelApple.txt";
			currentLevel = 2;

			if(currentMenu == MENU_DLC)
			{
				lvlName = "Levels/DessertsDLC/LevelKeyLimePie.txt";
				currentLevel = 17;
			}

			PlayClickSound();
			TransitionMenu(MENU_GAME);
			LoadScreen(lvlName);
			//LoadScreen("Levels/DessertsDLC/LevelKeyLimePie.txt");
			shotRemaining = 7;
			//shotRemaining = 40;
			transitioned = true;
		}
		else if(buttons[4].state == 2 && 
			((levelScoresBest[2] >= NORMAL_MIN && currentMenu == MENU_MAIN)
			|| (levelScoresBest[17] >= DLC_DESSERT_MIN && currentMenu == MENU_DLC)))
		{
			char* lvlName = "Levels/LevelStrawberry.txt";
			currentLevel = 3;

			if(currentMenu == MENU_DLC)
			{
				lvlName = "Levels/DessertsDLC/LevelLemonCremePie.txt";
				currentLevel = 18;
			}

			PlayClickSound();
			TransitionMenu(MENU_GAME);
			LoadScreen(lvlName);
			//LoadScreen("Levels/DessertsDLC/LevelLemonCremePie.txt");
			shotRemaining = 5;
			//shotRemaining = 40;
			transitioned = true;
		}
		else if(buttons[5].state == 2 && 
			((levelScoresBest[3] >= NORMAL_MIN && currentMenu == MENU_MAIN)
			|| (levelScoresBest[18] >= DLC_DESSERT_MIN && currentMenu == MENU_DLC)))
		{
			char* lvlName = "Levels/LevelWatermelon.txt";
			currentLevel = 4;

			if(currentMenu == MENU_DLC)
			{
				lvlName = "Levels/DessertsDLC/LevelStrawberryShortcake.txt";
				currentLevel = 19;
			}

			PlayClickSound();
			TransitionMenu(MENU_GAME);
			LoadScreen(lvlName);
			//LoadScreen("Levels/DessertsDLC/LevelStrawberryShortcake.txt");
			shotRemaining = 7;
			//shotRemaining = 40;
			transitioned = true;
		}
	//Level Hard
		else if(buttons[6].state == 2 && levelScoresBest[4] >= NORMAL_MIN)
		{
			PlayClickSound();
			TransitionMenu(MENU_GAME);
			LoadScreen("Levels/LevelBanana.txt");
			shotRemaining = 10;
			currentLevel = 5;
			transitioned = true;
		}
		else if(buttons[7].state == 2 && levelScoresBest[5] >= HARD_MIN)
		{
			PlayClickSound();
			TransitionMenu(MENU_GAME);
			LoadScreen("Levels/LevelBlueBerries.txt");
			shotRemaining = 4;
			currentLevel = 6;
			transitioned = true;
		}
		else if(buttons[8].state == 2 && levelScoresBest[6] >= HARD_MIN)
		{
			PlayClickSound();
			TransitionMenu(MENU_GAME);
			LoadScreen("Levels/LevelCherries.txt");
			shotRemaining = 7;
			currentLevel = 7;
			transitioned = true;
		}
		else if(buttons[9].state == 2 && levelScoresBest[7] >= HARD_MIN)
		{
			PlayClickSound();
			TransitionMenu(MENU_GAME);
			LoadScreen("Levels/LevelPlum.txt");
			shotRemaining = 13;
			currentLevel = 8;
			transitioned = true;
		}
		else if(buttons[10].state == 2 && levelScoresBest[8] >= HARD_MIN)
		{
			PlayClickSound();
			TransitionMenu(MENU_GAME);
			LoadScreen("Levels/LevelLemon.txt");
			shotRemaining = 14;
			currentLevel = 9;
			transitioned = true;
		}
	//Level Austere
		else if(buttons[11].state == 2 && levelScoresBest[9] >= HARD_MIN)
		{
			PlayClickSound();
			TransitionMenu(MENU_GAME);
			LoadScreen("Levels/LevelKiwi.txt");
			shotRemaining = 4;
			currentLevel = 10;
			transitioned = true;
		}
		else if(buttons[12].state == 2 && levelScoresBest[10] >= AUSTERE_MIN)
		{
			PlayClickSound();
			TransitionMenu(MENU_GAME);
			LoadScreen("Levels/LevelPassionFruit.txt");
			shotRemaining = 11;
			currentLevel = 11;
			transitioned = true;
		}
		else if(buttons[13].state == 2 && levelScoresBest[11] >= AUSTERE_MIN)
		{
			PlayClickSound();
			TransitionMenu(MENU_GAME);
			LoadScreen("Levels/LevelVanilla.txt");
			shotRemaining = 7;
			currentLevel = 12;
			transitioned = true;
		}
		else if(buttons[14].state == 2 && levelScoresBest[12] >= AUSTERE_MIN)
		{
			PlayClickSound();
			TransitionMenu(MENU_GAME);
			LoadScreen("Levels/LevelPinkSHIT.txt");
			shotRemaining = 12;
			currentLevel = 13;
			transitioned = true;
		}
		else if(buttons[15].state == 2 && levelScoresBest[13] >= AUSTERE_MIN)
		{
			PlayClickSound();
			TransitionMenu(MENU_GAME);
			LoadScreen("Levels/LevelBlueCitrus.txt");
			shotRemaining = 12;
			currentLevel = 14;
			transitioned = true;
		}

		//Transition Result
		//if(transitioned)
		//	PlayClickSound();
}

void ColourButtons()
{
	//DARK red
	if(buttons[1].state == 2)
	{
		mouseColor = F_RED;
		mouseColorBack = B_RED;
	}
	//DARK green
	else if(buttons[2].state == 2)
	{
		mouseColor = F_GREEN;
		mouseColorBack = B_GREEN;
	}
	//DARK blue
	else if(buttons[3].state == 2)
	{
		mouseColor = F_BLUE;
		mouseColorBack = B_BLUE;
	}
	//LIGHT red
	else if(buttons[4].state == 2)
	{
		mouseColor = F_RED | F_BRIGHTEN;
		mouseColorBack = B_RED | B_BRIGHTEN;
	}
	//LIGHT green
	else if(buttons[5].state == 2)
	{
		mouseColor = F_GREEN | F_BRIGHTEN;
		mouseColorBack = B_GREEN | B_BRIGHTEN;
	}
	//LIGHT blue
	else if(buttons[6].state == 2)
	{
		mouseColor = F_BLUE | F_BRIGHTEN;
		mouseColorBack = B_BLUE | B_BRIGHTEN;
	}

	PlaceText("TAB: change colour", 0, CANVAS_HEIGHT + 5, 
						mouseColor, F_BLACK);
}

//Unique draws
void DrawButton(Button b)
{
	int width = b.rectangle.right - b.rectangle.left;
	int height = b.rectangle.bottom - b.rectangle.top;

	for(int y = 0; y < height; ++y)
	{
		for(int x = 0; x < width; ++x)
		{
			if(x == 0 || x == width - 1
				|| y == 0 || y == height - 1)
			{
				PlaceText('x', b.rectangle.left + x, 
							b.rectangle.top + y, b.colorWord, B_BLACK);
			}
		}
	}

	//If button is empty (black), don't draw anything inside
	if(b.Color() != F_BLACK)
	{
		//If has number value, draw number
		if(b.hasNumValue)
		{
			PlaceNum(b.numValue, b.rectangle.left + ((width - b.textSize) / 2), 
				b.rectangle.top + (height / 2), F_RED | F_GREEN | F_BLUE | F_BRIGHTEN,
						B_BLACK);
		}
		//Else draw text
		else
		{
			PlaceText(b.text, b.rectangle.left + ((width - b.textSize) / 2), 
				b.rectangle.top + (height / 2), F_RED | F_GREEN | F_BLUE | F_BRIGHTEN,
						B_BLACK);
		}
	}
}
void DrawColourChart()
{
	//Dark Red + Dark Green = Brown (Dark Yellow)
	DrawColourDefTwo(B_RED, B_GREEN, CANVAS_WIDTH + 2, CANVAS_HEIGHT + 2); 
	//Dark Red + Dark Blue = Purple
	DrawColourDefTwo(B_RED, B_BLUE, CANVAS_WIDTH + 2, CANVAS_HEIGHT + 4); 
	//Dark Blue + Dark Green = Dark Turquiose
	DrawColourDefTwo(B_BLUE, B_GREEN, CANVAS_WIDTH + 2, CANVAS_HEIGHT + 6); 
	//Dark Red + Dark Blue + Dark Green = White
	DrawColourDefThree(B_RED, B_BLUE, B_GREEN, CANVAS_WIDTH + 2, CANVAS_HEIGHT + 8); 

	//Light Red + Light Green = Yellow
	DrawColourDefTwo(B_RED | B_BRIGHTEN, B_GREEN | B_BRIGHTEN, CANVAS_WIDTH + 2, CANVAS_HEIGHT + 10); 
	//Light Red + Light Blue = Pink
	DrawColourDefTwo(B_RED | B_BRIGHTEN, B_BLUE | B_BRIGHTEN, CANVAS_WIDTH + 2, CANVAS_HEIGHT + 12); 
	//Light Blue + Light Green = Turquiose
	DrawColourDefTwo(B_BLUE | B_BRIGHTEN, B_GREEN | B_BRIGHTEN, CANVAS_WIDTH + 2, CANVAS_HEIGHT + 14); 
	//Light Red + Light Blue + Light Green = White
	DrawColourDefThree(B_RED | B_BRIGHTEN, B_BLUE | B_BRIGHTEN, B_GREEN | B_BRIGHTEN, CANVAS_WIDTH + 2, CANVAS_HEIGHT + 16); 
}
void DrawColourDefTwo(int colour1, int colour2, int xPos, int yPos)
{
	PlaceText(' ', xPos, yPos, F_BLACK, colour1);
	PlaceText('+', xPos + 2, yPos, F_RED | F_BLUE | F_GREEN, B_BLACK);
	PlaceText(' ', xPos + 4, yPos, F_BLACK, colour2);
	PlaceText('=', xPos + 6, yPos, F_RED | F_BLUE | F_GREEN, B_BLACK);
	PlaceText(' ', xPos + 8, yPos, F_BLACK, colour1 | colour2);
}
void DrawColourDefThree(int colour1, int colour2, int colour3, int xPos, int yPos)
{
	PlaceText(' ', xPos, yPos, F_BLACK, colour1);
	PlaceText('+', xPos + 2, yPos, F_RED | F_BLUE | F_GREEN, B_BLACK);
	PlaceText(' ', xPos + 4, yPos, F_BLACK, colour2);
	PlaceText('+', xPos + 6, yPos, F_RED | F_BLUE | F_GREEN, B_BLACK);
	PlaceText(' ', xPos + 8, yPos, F_BLACK, colour3);
	PlaceText('=', xPos + 10, yPos, F_RED | F_BLUE | F_GREEN, B_BLACK);
	PlaceText(' ', xPos + 12, yPos, F_BLACK, colour1 | colour2 | colour3);
}

void MakeTraceMode()
{
	for(int x = 0; x < CANVAS_WIDTH; ++x)
	{
		for(int y = 0; y < CANVAS_HEIGHT; ++y)
		{
			if(brushPoints[x][y].ColorBack() != B_BLACK)
			{
				brushPoints[x][y].SketchColor(brushPoints[x][y].ColorBack());
				brushPoints[x][y].Color(F_BLACK | F_BRIGHTEN);
				brushPoints[x][y].ColorBack(B_BLACK | B_BRIGHTEN);
				brushPoints[x][y].IsSketchPoint(true);
				brushPoints[x][y].Updated(true);
			}
		}
	}

	fillMode = true;
}

void TransitionMenu(int menu)
{
	currentMenu = menu;

	if(!firstLoad)
		WipeScreen();
	else
		firstLoad = false;

	switch(currentMenu)
	{
	case MENU_INTRO:
		InitializeIntro();
		break;
	case MENU_MAIN:
	default:
		InitializeMenu();
		break;
	case MENU_DLC:
		InitializeDLCMenu();
		break;
	case MENU_GAME:
		delayToTraceMode = true;
		shotRemaining = 10;
		shotRemainingLast = 0;
		levelScoreCurrent = 0;
		blinkEndButton = -1;
		InitializeCanvas();

		//Draw Colour Chart
		DrawColourChart();
		break;
	}
}
void WipeScreen()
{
	for(int y = 70; y >= 0; --y)
	{
		PlaceText("                                                                                                               ", 0, y, F_BLACK, B_BLACK);
	}
}

//Score Level
void ScoreLevel()
{
	int blocksTotal = 0;
	int blocksPainted = 0;
	int blocksCorrectlyPainted = 0;

	float percentMult = 1;

	for(int x = 0; x < CANVAS_WIDTH; ++x)
	{
		for(int y = 0; y < CANVAS_HEIGHT; ++y)
		{
			if(brushPoints[x][y].IsSketchPoint())
			{
				blocksTotal++;

				if(brushPoints[x][y].Color() != F_BRIGHTEN)
				{
					blocksPainted++;
					if(brushPoints[x][y].IsCorrectlyColored())
						blocksCorrectlyPainted++;

					float percentCorrectBlock = /*(float)(*/brushPoints[x][y].ColorCorrectValue() / brushPoints[x][y].SketchColor(); /// (float)brushPoints[x][y].SketchColor());
					percentMult += percentCorrectBlock;
				}
			}
		}
	}

	percentMult /= blocksTotal;

	//Painted Blocks
	PlaceText("Painted Blocks: ", CANVAS_WIDTH + 2, 10, F_RED, F_BLACK);
	PlaceNum(blocksPainted, CANVAS_WIDTH + 2, 11, F_RED | F_BRIGHTEN, F_BLACK);
	PlaceText("of", CANVAS_WIDTH + 6, 11, F_RED, F_BLACK);
	PlaceNum(blocksTotal, CANVAS_WIDTH + 9, 11, F_RED | F_BRIGHTEN, F_BLACK);

	//Correctly Coloured Blocks
	PlaceText("Correctly Coloured Blocks:", CANVAS_WIDTH + 2, 13, F_RED, F_BLACK);
	PlaceNum(blocksCorrectlyPainted, CANVAS_WIDTH + 2, 14, F_RED | F_BRIGHTEN, F_BLACK);
	PlaceText("of", CANVAS_WIDTH + 6, 14, F_RED, F_BLACK);
	PlaceNum(blocksTotal, CANVAS_WIDTH + 9, 14, F_RED | F_BRIGHTEN, F_BLACK);

	//Correctly Coloured Blocks
	PlaceText("Colour Percent Average:", CANVAS_WIDTH + 2, 16, F_RED, F_BLACK);
	PlaceNum((int)(percentMult * 100.0f), CANVAS_WIDTH + 2, 17, F_RED | F_BRIGHTEN, F_BLACK);

	////Unused Paints
	//PlaceText("Unused Paints:", CANVAS_WIDTH + 2, 19, F_RED, F_BLACK);
	//PlaceNum(0, CANVAS_WIDTH + 2, 20, F_RED | F_BRIGHTEN, F_BLACK);
	//PlaceText("of", CANVAS_WIDTH + 6, 20, F_RED, F_BLACK);
	//PlaceNum(6, CANVAS_WIDTH + 9, 20, F_RED | F_BRIGHTEN, F_BLACK);

	//Total Score
	PlaceText("Score", CANVAS_WIDTH + 2, 20, F_GREEN, F_BLACK);
	float pBlocksScore = (float)blocksPainted / (float)blocksTotal;
	float pBlockCorrectScore = (float)blocksCorrectlyPainted / (float)blocksTotal;
	levelScoreCurrent = (int)(pBlocksScore * pBlockCorrectScore * 100.0f * percentMult);
	PlaceNum(levelScoreCurrent, CANVAS_WIDTH + 2, 21, F_GREEN | F_BRIGHTEN, F_BLACK);

	//Normal
	if(currentLevel >= 0 && currentLevel < 5
		&& 	levelScoresBest[currentLevel] < NORMAL_MIN)
	{
		PlaceText("Unlock next level at:", CANVAS_WIDTH + 5, 23, F_RED | F_GREEN, F_BLACK);
		PlaceNum(NORMAL_MIN, CANVAS_WIDTH + 5, 24, F_RED | F_GREEN | F_BRIGHTEN, F_BLACK);
	}
	//Hard
	else if(currentLevel >= 5 && currentLevel < 10
		&& 	levelScoresBest[currentLevel] < HARD_MIN)
	{
		PlaceText("Unlock next level at:", CANVAS_WIDTH + 5, 23, F_RED | F_GREEN, F_BLACK);
		PlaceNum(HARD_MIN, CANVAS_WIDTH + 5, 24, F_RED | F_GREEN | F_BRIGHTEN, F_BLACK);
	}//Austere
	else if(currentLevel >= 10 && currentLevel < 15
		&& 	levelScoresBest[currentLevel] < AUSTERE_MIN)
	{
		PlaceText("Unlock next level at:", CANVAS_WIDTH + 5, 23, F_RED | F_GREEN, F_BLACK);
		PlaceNum(AUSTERE_MIN, CANVAS_WIDTH + 5, 24, F_RED | F_GREEN | F_BRIGHTEN, F_BLACK);
	}

	//Best Score
	PlaceText("Best", CANVAS_WIDTH + 2, 26, F_RED | F_BLUE, F_BLACK);
	PlaceNum(levelScoresBest[currentLevel], CANVAS_WIDTH + 2, 27, F_RED | F_BLUE | F_BRIGHTEN, F_BLACK);
}

//Places text based on a coordinate position, a text character and a colour (foreground/background)
void PlaceText(char text, int posX, int posY, int colorFore, int colorBack)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	//Set new position
	COORD cursorPlacePos;
	cursorPlacePos.X = posX + 1;
	cursorPlacePos.Y = posY + 1;
	//Place cursor position
	SetConsoleCursorPosition(hConsole, cursorPlacePos);
	//Draw character
	DrawChar(text, colorFore, colorBack);
}
void PlaceText(char* text, int posX, int posY, int colorFore, int colorBack)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	//Set new position
	COORD cursorPlacePos;
	//+1 = offset whole screen by one to add border
	cursorPlacePos.X = posX + 1;
	cursorPlacePos.Y = posY + 1;
	//Place cursor position
	SetConsoleCursorPosition(hConsole, cursorPlacePos);
	//Draw character
	DrawChar(text, colorFore, colorBack);
}
void PlaceNum(int num, int posX, int posY, int colorFore, int colorBack)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	//Set new position
	COORD cursorPlacePos;
	cursorPlacePos.X = posX + 1;
	cursorPlacePos.Y = posY + 1;
	//Place cursor position
	SetConsoleCursorPosition(hConsole, cursorPlacePos);
	//Draw character
	DrawNum(num, colorFore, colorBack);
}
void PlaceNum(float num, int posX, int posY, int colorFore, int colorBack)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	//Set new position
	COORD cursorPlacePos;
	cursorPlacePos.X = posX + 1;
	cursorPlacePos.Y = posY + 1;
	//Place cursor position
	SetConsoleCursorPosition(hConsole, cursorPlacePos);
	//Draw character
	DrawNum(num, colorFore, colorBack);
}
//Draws a character at the cursor position based on a text character and a colour (foreground/background)
void DrawChar(char charToDraw, int colorFore, int colorBack)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	//Set Color
	//   NOTE: Background colour of world is constant unless otherwise
	SetConsoleTextAttribute(hConsole, colorFore|colorBack);
	//Print character with colour
	_cprintf("%c", charToDraw);
}
void DrawChar(char* charToDraw, int colorFore, int colorBack)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	//Set Color
	//   NOTE: Background colour of world is constant unless otherwise
	SetConsoleTextAttribute(hConsole, colorFore|colorBack);
	//Print character with colour
	_cprintf("%s", charToDraw);
}
void DrawNum(int numToDraw, int colorFore, int colorBack)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	//Set Color
	//   NOTE: Background colour of world is constant unless otherwise
	SetConsoleTextAttribute(hConsole, colorFore|colorBack);
	//Print character with colour
	_cprintf("%d", numToDraw);
}
void DrawNum(float numToDraw, int colorFore, int colorBack)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	//Set Color
	//   NOTE: Background colour of world is constant unless otherwise
	SetConsoleTextAttribute(hConsole, colorFore|colorBack);
	//Print character with colour
	_cprintf("%f", numToDraw);
}
//Clears the text at a point
void ClearText(int posX, int posY)
{
	//PlaceText(' ', posX, posY, F_BLACK, B_BLACK);
	posX = Clamp(posX, 0, CANVAS_WIDTH - 1);
	posY = Clamp(posY, 0, CANVAS_HEIGHT - 1);

	if(brushPoints[posX][posY].IsSketchPoint())
	{
		brushPoints[posX][posY].Color(F_BLACK | F_BRIGHTEN);
		brushPoints[posX][posY].ColorBack(B_BLACK | B_BRIGHTEN);
		brushPoints[posX][posY].Updated(true);
	}
	else if(brushPoints[posX][posY].Color() != F_BRIGHTEN
		&& brushPoints[posX][posY].ColorBack() != B_BRIGHTEN)
	{
		brushPoints[posX][posY].Color(F_BLACK);
		brushPoints[posX][posY].ColorBack(B_BLACK);
		brushPoints[posX][posY].Updated(true);
	}
}

//SOUND LOOP
void SoundLoop(string soundName)
{
	//PlaySound(TEXT(soundName.c_str()), NULL, SND_FILENAME);
}
//SOUND
void PlayClickSound()
{
	thread sT(SoundLoop, "Sound/cursorClick.wav");
	sT.join();
}
void PlaySplatterSound()
{
	int randNum = Random(0, 3);

	switch(randNum)
	{
	case 0:
	default:
		{
		thread sT(SoundLoop, "Sound/splatter1.wav");
		sT.join();
		//PlaySound(TEXT("Sound/splatter1.wav"), NULL, SND_FILENAME);
		}
		break;
	case 1:
		{
		thread sT(SoundLoop, "Sound/splatter2.wav");
		sT.join();
		//PlaySound(TEXT("Sound/splatter2.wav"), NULL, SND_FILENAME);
		}
		break;
	case 2:
		{
		thread sT(SoundLoop, "Sound/splatter3.wav");
		sT.join();
		//PlaySound(TEXT("Sound/splatter3.wav"), NULL, SND_FILENAME);
		}
		break;
	}

}

//HELPERS---------------------------------------------
POINT GetMousePos()
{
	POINT posOrigin;
	WINDOWPLACEMENT window = GetWindowData();
	posOrigin.x = window.rcNormalPosition.left;
	posOrigin.y = window.rcNormalPosition.top;

	POINT pos;

	GetCursorPos(&pos);

	pos.x -= posOrigin.x;
	pos.y -= posOrigin.y;

	pos.x = Clamp(pos.x, 5, window.rcNormalPosition.right - posOrigin.x - 60);
	pos.y = Clamp(pos.y, 5, window.rcNormalPosition.bottom - posOrigin.y - 20);

	pos.x /= 8.25f;
	pos.y /= 12.65f; //13.25

	return pos;
}
WINDOWPLACEMENT GetWindowData()
{
	WINDOWPLACEMENT p;
	HWND foreGroundWindow = GetForegroundWindow();
	GetWindowPlacement(foreGroundWindow, &p);

	return p;
}
void SetWindowData(RECT windowRectangle)
{
	WINDOWPLACEMENT p = GetWindowData();
	p.rcNormalPosition = windowRectangle;

	HWND foreGroundWindow = GetForegroundWindow();
	SetWindowPlacement(foreGroundWindow, &p);
}
bool IsMouseDownLeft()
{
	return GetAsyncKeyState(VK_LBUTTON) != 0;
}
bool IsMouseDownRight()
{
	return GetAsyncKeyState(VK_RBUTTON) != 0;
}
bool IsMouseUpLeft()
{
	return GetAsyncKeyState(VK_LBUTTON) == 0;
}
bool IsMouseLeftClicked()
{
	if(IsMouseDownLeft() && !mouseDown)
	{
		mouseDown = true;
		return true;
	}
	else if(IsMouseUpLeft() && mouseDown)
	{
		mouseDown = false;
	}

	return false;
}

//MATH--------------------------------------------------------
int Clamp(int value, int minV, int maxV)
{
	return max(min(value, maxV), minV);
}
int Random(int base, int max)
{
	//Get random integer (something like 0 to 80000000)
	int randomValue = rand();

	//If max value is zero, return zero because you cannot divide by zero
	if(max == 0)
		return 0;
	//Else return modulo of randomnumber plus the base
	else
		return randomValue % max + base;
}
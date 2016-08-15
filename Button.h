#include <Windows.h>

#pragma once
enum ButttonState
{
	EMPTY, HOVERED, PRESSED, RELEASED
};

class Button
{
private:
	int color;
	int colorHover;
	int colorClick;
public:
	//Params
	int Color() { return this->color; }
	void Color(int value) { this->color = value; }

	Button();
	Button(int posX, int posY, int width, int height, 
			char* Text, int TextSize, int Color,
			int ColorHover, int ColorClick);
	~Button();

	void Update(POINT mousePos);
	bool Contains(RECT rec, POINT pos);
	void ChangeState(int stateNew);
	void SetValue(int value) { this->numValue = value;
								this->hasNumValue = true; }

	//Variables
	RECT rectangle;
	int colorWord;
	char* text;
	int textSize;

	int numValue;
	bool hasNumValue;

	int state;
	int stateLast;
	bool firstDraw;
};


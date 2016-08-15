#include "stdafx.h"

#include <Windows.h>

#include "Button.h"

Button::Button()
{
	this->state = EMPTY;
	this->stateLast = HOVERED;
	this->colorWord = 0;
	this->rectangle.left = 0;
	this->rectangle.top = 0;
	this->rectangle.right = 10;
	this->rectangle.bottom = 10;
	this->text = "!!";
	this->textSize = 2;
	this->firstDraw = true;
	
	this->color = 0;
	this->colorHover = 0;
	this->colorClick = 0;

	this->hasNumValue = false;
}
Button::Button(int posX, int posY, int width, int height, 
			   char* Text, int TextSize, int Color,
			   int ColorHover, int ColorClick)
{
	this->state = EMPTY;
	this->stateLast = HOVERED;
	this->colorWord = Color;
	this->rectangle.left = posX;
	this->rectangle.top = posY;
	this->rectangle.right = posX + width;
	this->rectangle.bottom = posY + height;
	this->text = Text;
	this->textSize = TextSize;
	this->firstDraw = true;
	
	this->color = Color;
	this->colorHover = ColorHover;
	this->colorClick = ColorClick;

	this->hasNumValue = false;
}
Button::~Button(void)
{
}

void Button::Update(POINT mousePos)
{
	this->stateLast = this->state;

	if(Contains(this->rectangle, mousePos))
	{
		if(GetAsyncKeyState(VK_LBUTTON) != 0 && this->state == HOVERED)
			ChangeState(PRESSED);
		else if(GetAsyncKeyState(VK_LBUTTON) == 0 && this->state != HOVERED)
			ChangeState(HOVERED);
	}
	else if(!Contains(this->rectangle, mousePos) && this->state != EMPTY)
		ChangeState(EMPTY);
}
void Button::ChangeState(int stateNew)
{
	this->state = stateNew;

	switch(this->state)
	{
		case EMPTY:
			this->colorWord = this->color;
			break;
		case HOVERED:
			this->colorWord = this->colorHover;
			break;
		case PRESSED:
			this->colorWord = this->colorClick;
			break;
	}
}
bool Button::Contains(RECT rec, POINT pos)
{
	if(pos.x >= rec.left && pos.x <= rec.right
		&& pos.y >= rec.top && pos.y <= rec.bottom)
		return true;

	return false;
}

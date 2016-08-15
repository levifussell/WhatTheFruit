#include "stdafx.h"
#include "BrushPoint.h"


//Constructs
BrushPoint::BrushPoint(float PosX, float PosY, int Color, int ColorBack, char Text)
{
	this->x = PosX;
	this->y = PosY;
	this->color = Color;
	this->colorBack = ColorBack;
	this->text = Text;
	this->updated = true;
	this->isSketchPoint = false;
	this->sketchColor = Color;
}
BrushPoint::BrushPoint(void)
{
	this->x = 0;
	this->y = 0;
	this->color = 0;
	this->colorBack = 0;
	this->text = '!';
	this->updated = true;
	this->isSketchPoint = false;
	this->sketchColor = 0;
}
BrushPoint::~BrushPoint(void)
{
}
	
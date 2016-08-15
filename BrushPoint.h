#pragma once
class BrushPoint
{
private:
	float x;
	float y;
	int color;
	int colorBack;
	char text;
	bool updated;
	
	int sketchColor;
	bool isSketchPoint;

public:
	//Params
	float X() { return this->x; }
	void X(float value) { this->x = value; }
	float Y() { return this->y; }
	void Y(float value) { this->y = value; }
	int Color() { return this->color; }
	void Color(int value) { this->color = value; }
	int ColorBack() { return this->colorBack; }
	void ColorBack(int value) { this->colorBack = value; }
	char Text() { return this->text; }
	void Text(char value) { this->text = value; }
	bool Updated() { return this->updated; }
	void Updated(bool value) { this->updated = value; }
	bool IsSketchPoint() { return this->isSketchPoint; }
	void IsSketchPoint(bool value) { this->isSketchPoint = value; }
	int SketchColor() { return this->sketchColor; }
	void SketchColor(int value) { this->sketchColor = value; }
	bool IsCorrectlyColored() { return this->colorBack == this->sketchColor; }
	int ColorCorrectValue() { return this->colorBack & this->sketchColor; }

	//Constructs
	BrushPoint();
	BrushPoint(float PosX, float PosY, int Color, int ColorBack, char Text);
	~BrushPoint(void);
};


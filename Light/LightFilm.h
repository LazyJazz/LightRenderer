#pragma once
#include"LightGeometry.h"
class LColor
{
public:
	double r, g, b;
	LColor();
	LColor(double R, double G, double B);
	LColor operator + (const LColor& color) const;
	LColor operator * (const LColor& color) const;
	LColor operator * (const double& scale) const;
	D2D1_COLOR_F GetColor() const;
};

class LFilm
{
public:
	UINT width, height;
	LColor* pColor;
	double* pScale;
	LFilm();
	void Release();
	void Resize(UINT Width, UINT Height);
	void AddColor(UINT x, UINT y, LColor color, double scale);
	void SetColor(UINT x, UINT y, LColor color, double scale);
	void GetImage(Image* img);
};
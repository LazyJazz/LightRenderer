#include "LightFilm.h"

LColor::LColor()
{
	r = g = b = 0.0;
}

LColor::LColor(double R, double G, double B)
{
	r = R;
	g = G;
	b = B;
}

LColor LColor::operator+(const LColor& color) const
{
	return LColor(r + color.r, g + color.g, b + color.b);
}

LColor LColor::operator*(const LColor& color) const
{
	return LColor(r * color.r, g * color.g, b * color.b);
}

LColor LColor::operator*(const double& scale) const
{
	return LColor(r * scale, g * scale, b * scale);
}

D2D1_COLOR_F LColor::GetColor() const
{
	double scale = 1.0 / max(1.0, max(r, max(g, b)));
	return D2D1::ColorF(r * scale, g * scale, b * scale);
}

LFilm::LFilm()
{
	width = height = 0;
	pColor = NULL;
	pScale = NULL;
}

void LFilm::Release()
{
	if (pColor)delete[]pColor;
	if (pScale)delete[]pScale;
	width = height = 0;
	pColor = NULL;
	pScale = NULL;
}

void LFilm::Resize(UINT Width, UINT Height)
{
	Release();
	width = Width;
	height = Height;
	pColor = new LColor[(ULONG64)width * height];
	pScale = new double[(ULONG64)width * height];
	memset(pScale, 0, sizeof(double) * width * height);
}

void LFilm::AddColor(UINT x, UINT y, LColor color, double scale)
{
	UINT id = y * width + x;
	pColor[id] = pColor[id] + color;
	pScale[id] = pScale[id] + scale;
}

void LFilm::SetColor(UINT x, UINT y, LColor color, double scale)
{
	UINT id = y * width + x;
	pColor[id] = color * scale;
	pScale[id] = scale;
}

void LFilm::GetImage(Image* img)
{
	if (width != img->GetWidth() || height != img->GetHeight())img->Resize(width, height);
	img->Lock();
	for(UINT x=0;x<width;x++)
		for (UINT y = 0; y < height; y++)
		{
			LColor color = pColor[y * width + x] * (1.0 / pScale[y * width + x]);
			img->PutPixel(x, y, color.GetColor());
		}
	img->Unlock();
}

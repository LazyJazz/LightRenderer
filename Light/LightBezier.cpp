#include "LightBezier.h"

LBezier::LBezier()
{
	n = 0; m = 0;
	pVec = NULL;
}

LBezier::LBezier(size_t width, size_t height)
{
	n = width; m = height;
	pVec = new LVec[width * height];
}

LBezier::~LBezier()
{
	Release();
}

void LBezier::Release()
{
	n = 0; m = 0;
	if (pVec)
	{
		delete[]pVec;
		pVec = NULL;
	}
}

void LBezier::Resize(size_t width, size_t height)
{
	Release();
	n = width; m = height;
	pVec = new LVec[n * m];
}

LVec* LBezier::operator[](const size_t x) const
{
	return pVec + x * m;
}

LVec& LBezier::GetVec(size_t x, size_t y) const
{
	return pVec[x * m + y];
}

LVec LBezier::GetBezier(size_t x0, size_t x1, size_t y0, size_t y1, double x, double y) const
{
	x1 -= x0;
	y1 -= y0;
	double scalex = 1.0;
	LVec ansx = LVec();
	for (size_t dx = 0; dx <= x1; dx++)
	{
		double scaley = 1.0;
		LVec ansy = LVec();
		for (size_t dy = 0; dy <= y1; dy++)
		{
			ansy += GetVec(dx + x0, dy + y0) * scaley * pow(1.0 - y, y1 - dy) * pow(y, dy);
			scaley /= dy + 1;
			scaley *= y1 - dy;
		}
		ansx += ansy * scalex * pow(1.0 - x, x1 - dx) * pow(x, dx);
		scalex /= dx + 1;
		scalex *= x1 - dx;
	}
	return ansx;
}

LVec LBezier::GetPos(double x, double y) const
{
	return GetBezier(0, n - 1, 0, m - 1, x, y);
}

LVec LBezier::GetNormal(double x, double y) const
{
	LVec v00, v01, v10, v11;
	LVec m02, m12, m20, m21;
	v00 = GetBezier(0, n - 2, 0, m - 2, x, y);
	v01 = GetBezier(0, n - 2, 1, m - 1, x, y);
	v10 = GetBezier(1, n - 1, 0, m - 2, x, y);
	v11 = GetBezier(1, n - 1, 1, m - 1, x, y);
	m02 = v00 * y + v01 * (1 - y);
	m12 = v10 * y + v11 * (1 - y);
	m20 = v00 * x + v10 * (1 - x);
	m21 = v01 * x + v11 * (1 - x);
	return ((m21 - m20) ^ (m12 - m02)).zoom(1.0);
}

#pragma once
#include "LightGeometry.h"

class LBezier
{
public:
	size_t n,m;
	LVec *pVec;
	LBezier();
	LBezier(size_t width, size_t height);
	~LBezier();
	void Release();
	void Resize(size_t width, size_t height);
	LVec* operator [] (const size_t x) const;
	LVec& GetVec(size_t x, size_t y) const;
	LVec GetBezier(size_t x0, size_t x1, size_t y0, size_t y1, double x, double y) const;
	LVec GetPos(double x, double y) const;
	LVec GetNormal(double x, double y) const;
};
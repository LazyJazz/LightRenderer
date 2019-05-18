#pragma once
#include"LightGeometry.h"

class LCamara
{
public:
	virtual LRay GenerateRay(UINT x, UINT y, double time, RandomDevice& rd) = 0;
};

class LCamaraFlat:public LCamara
{
public:
	LCoordinate coord;
	UINT film_width, film_height;
	double view_width, view_height;
	LCamaraFlat();
	virtual LRay GenerateRay(UINT x, UINT y, double time, RandomDevice& rd);
	void SetCoordinate(LCoordinate Coordinate);
	void SetFilmSize(UINT Width, UINT Height);
	void SetViewSize(double Width, double Height);
};

class LCamaraAperture :public LCamaraFlat
{
public:
	double aperture = 0.0;
	double focal_length = 1.0;
	virtual LRay GenerateRay(UINT x, UINT y, double time, RandomDevice& rd);
	void SetAperture(double aperture);
	void SetFocalLength(double focal_length);
};
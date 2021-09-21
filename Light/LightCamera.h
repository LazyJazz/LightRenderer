#pragma once
#include"LightGeometry.h"

class LCamera
{
public:
	virtual LRay GenerateRay(UINT x, UINT y, double time, RandomDevice& rd) = 0;
};

class LCameraFlat:public LCamera
{
public:
	LCoordinate coord;
	UINT film_width, film_height;
	double view_width, view_height;
	LCameraFlat();
	virtual LRay GenerateRay(UINT x, UINT y, double time, RandomDevice& rd);
	void SetCoordinate(LCoordinate Coordinate);
	void SetFilmSize(UINT Width, UINT Height);
	void SetViewSize(double Width, double Height);
};

class LCameraAperture :public LCameraFlat
{
public:
	double aperture = 0.0;
	double focal_length = 1.0;
	virtual LRay GenerateRay(UINT x, UINT y, double time, RandomDevice& rd);
	void SetAperture(double aperture);
	void SetFocalLength(double focal_length);
};
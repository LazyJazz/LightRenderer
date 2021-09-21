#include "LightCamera.h"

LCameraFlat::LCameraFlat()
{
	coord = LCoordinate();
	film_width = 0;
	film_height = 0;
	view_width = 0.0;
	view_height = 0.0;
}

LRay LCameraFlat::GenerateRay(UINT x, UINT y, double time, RandomDevice& rd)
{
	double xu = view_width * (1.0 / film_width);
	double yu = view_height * (1.0 / film_height);
	double l = -view_width * 0.5, r, u = view_height * 0.5, d;
	l += x * xu;
	r = l + xu;
	u -= y * yu;
	d = u - yu;
	return LRay(coord.o, coord.PutIn(LVec(
		rd.rand_double(l, r),
		rd.rand_double(d, u),
		1.0
	)) - coord.o);
}

void LCameraFlat::SetCoordinate(LCoordinate Coordinate)
{
	coord = Coordinate;
}

void LCameraFlat::SetFilmSize(UINT Width, UINT Height)
{
	film_width = Width;
	film_height = Height;
}

void LCameraFlat::SetViewSize(double Width, double Height)
{
	view_width = Width;
	view_height = Height;
}

LRay LCameraAperture::GenerateRay(UINT x, UINT y, double time, RandomDevice& rd)
{
	double xu = view_width * (1.0 / film_width);
	double yu = view_height * (1.0 / film_height);
	double l = -view_width * 0.5, r, u = view_height * 0.5, d;
	l += x * xu;
	r = l + xu;
	u -= y * yu;
	d = u - yu;
	LVec o;
	double radian = rd.rand_double(-L_PI, L_PI);
	o.z = 0.0;
	o.x = sin(radian);
	o.y = cos(radian);
	o *= rd.rand_double(0, aperture);
	o = coord.PutIn(o);
	return LRay(o, coord.PutIn(LVec(
		rd.rand_double(l, r),
		rd.rand_double(d, u),
		1.0
	)* focal_length) - o);
}

void LCameraAperture::SetAperture(double aperture)
{
	this->aperture = aperture;
}

void LCameraAperture::SetFocalLength(double focal_length)
{
	this->focal_length = focal_length;
}

#include "LightCamara.h"

LCamaraFlat::LCamaraFlat()
{
	coord = LCoordinate();
	film_width = 0;
	film_height = 0;
	view_width = 0.0;
	view_height = 0.0;
}

LRay LCamaraFlat::GenerateRay(UINT x, UINT y, double time, RandomDevice& rd)
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

void LCamaraFlat::SetCoordinate(LCoordinate Coordinate)
{
	coord = Coordinate;
}

void LCamaraFlat::SetFilmSize(UINT Width, UINT Height)
{
	film_width = Width;
	film_height = Height;
}

void LCamaraFlat::SetViewSize(double Width, double Height)
{
	view_width = Width;
	view_height = Height;
}

LRay LCamaraAperture::GenerateRay(UINT x, UINT y, double time, RandomDevice& rd)
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

void LCamaraAperture::SetAperture(double aperture)
{
	this->aperture = aperture;
}

void LCamaraAperture::SetFocalLength(double focal_length)
{
	this->focal_length = focal_length;
}

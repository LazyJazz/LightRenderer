#include "my_rasterize_lib.h"

Image * working_img = NULL;
D2D1_COLOR_F working_color = D2D1::ColorF(D2D1::ColorF::Black);
int origin_x = 0, origin_y = 0;
bool swapxy = false; int scalex = 1, scaley = 1;

void SetOrigin(int x = 0, int y = 0);
void SetTransform(bool swapxy = false, int scalex = 1, int scaley = 1);
void PutTransformedPixel(int x, int y);
void PutRelativePixel(int x, int y);
void PutSymmetricPixel(int x, int y);

void SetWorkingImage(Image * img)
{
	if (working_img)ReleaseWorkingImage();
	img->Lock();
	working_img = img;
}

void ReleaseWorkingImage()
{
	working_img->Unlock();
	working_img = 0;
}

void SetColor(D2D1_COLOR_F color)
{
	working_color = color;
}

void SetColor(float r, float g, float b, float a)
{
	SetColor(D2D1::ColorF(r, g, b, a));
}

void PutPixel(int x, int y)
{
	working_img->PutPixel(x, y, working_color);
}

void DrawLine(int x0, int y0, int x1, int y1)
{
	x1 -= x0;
	y1 -= y0;
	SetOrigin(x0, y0);
	bool swapxy = false;
	int scalex=1, scaley=1;
	if (x1 < 0)scalex = -1, x1 = -x1;
	if (y1 < 0)scaley = -1, y1 = -y1;
	if (x1 <= y1)swap(x1, y1), swapxy = true;
	SetTransform(swapxy, scalex, scaley);
	int e = -x1, y = 0;
	for (int x = 0; x <= x1; x++)
	{
		PutTransformedPixel(x,y);
		e+=2*y1;
		if (e >= 0)
		{
			y++;
			e -= 2 * x1;
		}
	}

}

void DrawCircle(int x, int y, int r)
{
	SetOrigin(x, y);
	x = 0; y = r;
	int d = 5 - r * 4;
	while (x <= y)
	{
		PutSymmetricPixel(x, y);
		if (d < 0)d += 8 * x + 12;
		else
		{
			d += 8 * (x - y) + 20;
			y--;
		}
		x++;
	}
}
void FloodFill8(int x, int y, DWORD origin)
{
	if (working_img->illegal(x, y))return;
	if (working_img->GetPixel(x, y) != origin)return;
	PutPixel(x, y);
	for (int px = -1; px <= 1; px++)
		for (int py = -1; py <= 1; py++)
			if (px != py)
				FloodFill8(x + px, y + py, origin);
}

void FloodFill8(int x, int y)
{
	DWORD origin_color = working_img->GetPixel(x, y);
	UINT A = 255 * working_color.a, R = 255 * working_color.r, G = 255 * working_color.g, B = 255 * working_color.b;
	DWORD targ_color = (A << 24) | (R << 16) | (G << 8) | B;
	if (targ_color == origin_color)return;
	FloodFill8(x, y, origin_color);
}

void FloodFill4(int x, int y, DWORD origin)
{
	if (working_img->illegal(x, y))return;
	if (working_img->GetPixel(x, y) != origin)return;
	PutPixel(x, y);
	FloodFill4(x + 1, y, origin);
	FloodFill4(x - 1, y, origin);
	FloodFill4(x, y + 1, origin);
	FloodFill4(x, y - 1, origin);
}

void FloodFill4(int x, int y)
{
	DWORD origin_color = working_img->GetPixel(x, y);
	UINT A = 255 * working_color.a, R = 255 * working_color.r, G = 255 * working_color.g, B = 255 * working_color.b;
	DWORD targ_color = (A << 24) | (R << 16) | (G << 8) | B;
	if (targ_color == origin_color)return;
	FloodFill4(x, y, origin_color);
}

void SetOrigin(int x, int y)
{
	origin_x = x;
	origin_y = y;
}

void SetTransform(bool swapxy, int scalex, int scaley)
{
	::swapxy = swapxy;
	::scalex = scalex;
	::scaley = scaley;
}

void PutTransformedPixel(int x, int y)
{
	if (swapxy)swap(x, y);
	x *= scalex;
	y *= scaley;
	PutRelativePixel(x, y);
}

void PutRelativePixel(int x, int y)
{
	PutPixel(x + origin_x, y + origin_y);
}

void PutSymmetricPixel3(int x, int y)
{
	PutRelativePixel(x, y);
	if (x != y)PutRelativePixel(y, x);
}

void PutSymmetricPixel2(int x, int y)
{
	PutSymmetricPixel3(x, y);
	if (y)PutSymmetricPixel3(x, -y);
}

void PutSymmetricPixel(int x, int y)
{
	PutSymmetricPixel2(x, y);
	if (x)PutSymmetricPixel2(-x, y);
}

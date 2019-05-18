#pragma once
#include"LibZJShared.h"
#include<Windows.h>
#include<wincodec.h>
#include<d2d1.h>
#include<cmath>
#include<cstdio>
#pragma comment(lib, "d2d1")
#pragma comment(lib, "Windowscodecs.lib" )
using namespace std;

HRESULT InitImageFactory();

HRESULT InitD2DFactory();

void InitWindow(UINT width, UINT height, LPCWSTR title, bool fullscreen, int nCmdShow);

void CloseWindow();

class Image
{
private:
	IWICBitmap				*pBitmap;
	IWICBitmapLock			*pBitmapLock;
	ID2D1RenderTarget		*pRenderTarget;
	DWORD * pBuffer;
	UINT width, height;
public:
	Image() :pBitmap(NULL), pBitmapLock(NULL), pRenderTarget(NULL), pBuffer(NULL) { width = height = 0; }
	~Image();
	void Release();
	void Resize(UINT width, UINT height);
	void Lock();
	DWORD * GetBuffer();
	void Unlock();
	void BeginDraw();
	void EndDraw();
	void FillEllipse(const D2D1_ELLIPSE &ellipse, const D2D1_COLOR_F &color);
	void FillRectangle(const D2D1_RECT_F &rectangle, const D2D1_COLOR_F &color);
	void DrawLine(const D2D1_POINT_2F &p1, const D2D1_POINT_2F &p2, const D2D1_COLOR_F &color, float strokeWidth = 1.0);
	void Clear(D2D1_COLOR_F color = D2D1::ColorF(D2D1::ColorF::Black));
	HRESULT LoadFromFile(PCWSTR uri);
	HRESULT SaveToFile(PCWSTR uri);
	HRESULT MakeD2DBitmap(ID2D1Bitmap **ppD2DBitmap);
	void PutPixel(int x, int y, UINT R, UINT G, UINT B, UINT A = 255);
	void PutPixel(int x, int y, DWORD Color);
	void PutPixel(int x, int y, D2D1_COLOR_F color);
	void PutPixel(int x, int y, float r, float g, float b, float a = 1.0);
	DWORD GetPixel(int x, int y) const;
	bool illegal(int x, int y) const;
	UINT GetWidth() const;
	UINT GetHeight() const;
	void GetSize(UINT * Width,UINT * Height) const;
	void GenerateRenderTarget();
};

HWND GetHWnd();

void SetFrameImage(Image * pimg);

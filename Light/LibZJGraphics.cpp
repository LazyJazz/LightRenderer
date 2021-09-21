#include"LibZJGraphics.h"
#include"LibZJFPSCounter.h"
#include"LibZJTimer.h"

HWND					g_hwnd;
ID2D1Factory			*g_pFactory = NULL;
ID2D1HwndRenderTarget	*g_pRenderTarget = NULL;
IWICImagingFactory		*g_pImgFactory = NULL;
ID2D1Bitmap				*g_pBitmap = NULL;
UINT					g_width, g_height;
HANDLE					g_hThread;
HANDLE					g_Mutex;
TCHAR					g_title[128];
bool					g_fullscreen;
bool					g_finished;
int						g_nCmdShow;

void CreateRT();//CreateRenderTarget

void OnDestroy();

LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{	
	switch (uMsg)
	{
	case WM_CREATE:
	{
		HRESULT res = InitImageFactory();
		if (SUCCEEDED(res))res = InitD2DFactory();
		//CreateRT();
		WaitForSingleObject(g_Mutex, INFINITE);
		g_finished = true;
		ReleaseMutex(g_Mutex);
		if (FAILED(res))
			MessageBox(NULL, L"Create Failed", L"Error!", MB_OK);
		return res;
	}

	case WM_CLOSE:
		if (MessageBox(g_hwnd, L"This will TERMINATE the whole program!", L"Really quit?", MB_OKCANCEL) == IDOK)
		{
			DestroyWindow(g_hwnd);
			//PostQuitMessage(0);
		}
		return 0;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);
			
			EndPaint(hWnd, &ps);
			//RedrawWindow(g_hwnd, NULL, NULL, RDW_INTERNALPAINT);
			//OnPaint();
			return 0;//*/
		}
	case WM_SIZE:
		{
			if (g_pRenderTarget != NULL)
			{
				RECT rc;
				GetClientRect(g_hwnd, &rc);

				D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

				g_pRenderTarget->Resize(size);
				//CalculateLayout();
				InvalidateRect(g_hwnd, NULL, FALSE);
			}
			return 0;
		}
	case WM_DESTROY:
		{
			OnDestroy();
			PostQuitMessage(0);
			return 0;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void Create(PCWSTR lpWindowName,
	DWORD dwStyle,
	DWORD dwExStyle = 0,
	int x = CW_USEDEFAULT,
	int y = CW_USEDEFAULT,
	int nWidth = CW_USEDEFAULT,
	int nHeight = CW_USEDEFAULT,
	HWND hWndParent = 0,
	HMENU hMenu = 0)
{
	WNDCLASS wndclass = {};
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = L"WindowClass";
	wndclass.hInstance = GetModuleHandle(NULL);
	//wndclass.style = WS_OVERLAPPEDWINDOW;
	if (!RegisterClass(&wndclass))
	{
		TCHAR message[128];
		wsprintf(message, L"RegisterClass Failed:%d", GetLastError());
		MessageBox(NULL, message, L"Error!", MB_OK);
		exit(0);
	}
	g_hwnd = CreateWindowEx(dwExStyle, L"WindowClass", lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), NULL);
	if (!g_hwnd)
	{
		TCHAR message[128];
		wsprintf(message, L"CreateWindow Failed:%d", GetLastError());
		MessageBox(NULL, message, L"Error!", MB_OK);
		exit(0);
	}
}

DWORD WINAPI WindowThread(LPVOID lpvoid)
{
	DWORD dwStyle = WS_OVERLAPPEDWINDOW;
	if (g_fullscreen)
	{
		Create(g_title, WS_POPUPWINDOW & ~(WS_DLGFRAME | WS_THICKFRAME), NULL, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	}
	else
	{
		Create(g_title, WS_OVERLAPPEDWINDOW, NULL, CW_USEDEFAULT, CW_USEDEFAULT, g_width, g_height);
	}
	//MessageBox(NULL, L"Finished Create()", L"Notice", MB_OK);
	ShowWindow(g_hwnd, g_nCmdShow);
	//MessageBox(NULL, L"Finished ShowWindow()", L"Notice", MB_OK);

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	exit(0);
}

void InitWindow(UINT width, UINT height, LPCWSTR title, bool fullscreen, int nCmdShow)
{
	g_width = width;
	g_height = height;
	wsprintf(g_title, L"%ls", title);
	g_fullscreen = fullscreen;
	g_nCmdShow = nCmdShow;
	CreateMutex(NULL, FALSE, L"LibZJWindowResult");
	g_finished = NULL;
	g_hThread = CreateThread(NULL, NULL, WindowThread, NULL, NULL, NULL);
	while (WaitForSingleObject(g_Mutex, INFINITE))
	{
		if (g_finished)break;
		ReleaseMutex(g_Mutex);
	}
	ReleaseMutex(g_Mutex);
	//CloseHandle(g_Mutex);
}

void CloseWindow()
{
	//PostQuitMessage();
	SendMessage(g_hwnd, WM_DESTROY, NULL, NULL);
}

HWND GetHWnd()
{
	return g_hwnd;
}

void SetFrameImage(Image * pimg)
{
	static TCHAR title[128];
	static FPSCounter fc;
	RECT rect;
	GetClientRect(g_hwnd, &rect);
	wsprintf(title, L"Client width: %d height: %d FPS:%d", rect.right - rect.left, rect.bottom - rect.top, fc.FPS());
	SetWindowText(GetHWnd(), title);
	CreateRT();
	pimg->MakeD2DBitmap(&g_pBitmap);
	g_pRenderTarget->BeginDraw();
	/*ID2D1SolidColorBrush *pBrush;
	g_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::GreenYellow));
	g_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.0f, 0.0f, 1.0f), &pBrush);
	D2D1_SIZE_F size = g_pRenderTarget->GetSize();
	g_pRenderTarget->FillRectangle(D2D1::Rect(0.0f, 0.0f, size.width*0.5f, size.height*0.5f), pBrush);
	SafeRelease(&pBrush);//*/
	if (g_pBitmap)
	{
		D2D1_SIZE_F size = g_pRenderTarget->GetSize();
		g_pRenderTarget->DrawBitmap(g_pBitmap, D2D1::RectF(0.0f, 0.0f, size.width, size.height));
	}
	else
	{
		g_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
	}
	g_pRenderTarget->EndDraw();
}

bool g_InitializedImageFactory = false;

HRESULT InitImageFactory()
{
	if (g_InitializedImageFactory)return S_OK;
	g_InitializedImageFactory = true;
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if(!g_pImgFactory)
	{
		if (FAILED(CoCreateInstance(
				CLSID_WICImagingFactory,
				NULL,
				CLSCTX_INPROC_SERVER,
				IID_IWICImagingFactory,
				(LPVOID*)&g_pImgFactory)))
		{
			MessageBox(NULL, L"WIC Create Failed", L"Error!", MB_OK);
			return -1;
		}
	}
	return 0;
}

HRESULT InitD2DFactory()
{
	if (!g_pFactory)
	{
		if (FAILED(D2D1CreateFactory(
			D2D1_FACTORY_TYPE_MULTI_THREADED, &g_pFactory)))
		{
			MessageBox(NULL, L"D2D Init Failed", L"Error!", MB_OK);
			return -1;  // Fail CreateWindowEx.
		}
	}
	return 0;
}

void CreateRT()
{
	if (g_pRenderTarget == NULL)
	{
		RECT rc;
		GetClientRect(g_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
		//pFactory->CreateWicBitmapRenderTarget()

		HRESULT hr = g_pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(g_hwnd, size),
			&g_pRenderTarget);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Failed Creating RenderTarget", L"Error!", MB_OK);
		}
	}
}

void OnDestroy()
{
	SafeRelease(&g_pBitmap);
	SafeRelease(&g_pRenderTarget);
	SafeRelease(&g_pFactory);
	SafeRelease(&g_pImgFactory);
	CoUninitialize();
	CloseHandle(g_Mutex);
}

Image::~Image()
{
	SafeRelease(&pRenderTarget);
	SafeRelease(&pBitmapLock);
	SafeRelease(&pBitmap);
	width = height = 0;
}

void Image::Release()
{
	SafeRelease(&pRenderTarget);
	SafeRelease(&pBitmapLock);
	SafeRelease(&pBitmap);
	width = height = 0;
}

void Image::Resize(UINT width, UINT height)
{
	Release();
	g_pImgFactory->CreateBitmap(width, height, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnDemand, &pBitmap);
	if(g_pFactory)g_pFactory->CreateWicBitmapRenderTarget(pBitmap, D2D1::RenderTargetProperties(), &pRenderTarget);
	this->width = width;
	this->height = height;
}

void Image::ResizeFloat(UINT width, UINT height)
{
	Release();
	g_pImgFactory->CreateBitmap(width, height, GUID_WICPixelFormat128bppPRGBAFloat, WICBitmapCacheOnDemand, &pBitmap);
	if (g_pFactory)g_pFactory->CreateWicBitmapRenderTarget(pBitmap, D2D1::RenderTargetProperties(), &pRenderTarget);
	this->width = width;
	this->height = height;
}

void Image::Lock()
{
	WICRect wicrect;
	wicrect.X = 0;
	wicrect.Y = 0;
	wicrect.Width = width;
	wicrect.Height = height;
	pBitmap->Lock(&wicrect, WICBitmapLockWrite, &pBitmapLock);
	if (pBitmapLock)
	{
		UINT size;
		WICInProcPointer WIPP;
		pBitmapLock->GetDataPointer(&size, &WIPP);
		pBuffer = (DWORD*)WIPP;
	}
}

void Image::LockFloat()
{
	WICRect wicrect;
	wicrect.X = 0;
	wicrect.Y = 0;
	wicrect.Width = width;
	wicrect.Height = height;
	pBitmap->Lock(&wicrect, WICBitmapLockWrite, &pBitmapLock);
	if (pBitmapLock)
	{
		UINT size;
		WICInProcPointer WIPP;
		pBitmapLock->GetDataPointer(&size, &WIPP);
		pBufferFloat = (FLOAT*)WIPP;
	}
}

DWORD * Image::GetBuffer()
{
	if (pBitmapLock)
	{
		/*UINT size;
		WICInProcPointer WIPP;
		pBitmapLock->GetDataPointer(&size, &WIPP);//*/
		return pBuffer;
	}
	return nullptr;
}

void Image::Unlock()
{
	SafeRelease(&pBitmapLock);
	pBuffer = NULL;
	pBufferFloat = NULL;
}

void Image::BeginDraw()
{
	//GenerateRenderTarget();
	pRenderTarget->BeginDraw();
}

void Image::EndDraw()
{
	pRenderTarget->EndDraw();
}

void Image::FillEllipse(const D2D1_ELLIPSE &ellipse, const D2D1_COLOR_F &color)
{
	ID2D1SolidColorBrush *pBrush;
	HRESULT hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);
	if (FAILED(hr))return;
	pRenderTarget->FillEllipse(ellipse, pBrush);
	SafeRelease(&pBrush);
}

void Image::FillRectangle(const D2D1_RECT_F &rectangle, const D2D1_COLOR_F &color)
{
	ID2D1SolidColorBrush *pBrush;
	HRESULT hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);
	if (FAILED(hr))return;
	pRenderTarget->FillRectangle(rectangle, pBrush);
	SafeRelease(&pBrush);
}

void Image::DrawLine(const D2D1_POINT_2F & p1, const D2D1_POINT_2F & p2, const D2D1_COLOR_F & color, float strokeWidth)
{
	ID2D1SolidColorBrush *pBrush;
	HRESULT hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);
	if (FAILED(hr))return;
	pRenderTarget->DrawLine(p1, p2, pBrush, strokeWidth);
	SafeRelease(&pBrush);
}

void Image::Clear(D2D1_COLOR_F color)
{
	pRenderTarget->Clear(color);
}

HRESULT Image::LoadFromFile(PCWSTR uri)
{
	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pSource = NULL;
	IWICFormatConverter *pConverter = NULL;
	HRESULT hr = g_pImgFactory->CreateDecoderFromFilename(
		uri,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);
	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}
	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = g_pImgFactory->CreateFormatConverter(&pConverter);
	}
	if (SUCCEEDED(hr))
	{
		hr = pConverter->Initialize(
			pSource,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut
		);
	}
	UINT Size, Stride,Width,Height;
	WICInProcPointer WIPP = NULL;
	if (SUCCEEDED(hr))
		hr = pConverter->GetSize(&Width, &Height);
	if (SUCCEEDED(hr))
		Resize(Width, Height);
	Lock();
	if (SUCCEEDED(hr))
		hr = pBitmapLock->GetDataPointer(&Size, &WIPP);
	if (SUCCEEDED(hr))
		hr = pBitmapLock->GetStride(&Stride);
	if (SUCCEEDED(hr) && WIPP)
		hr = pConverter->CopyPixels(NULL, Stride, Size, WIPP);
	Unlock();
	SafeRelease(&pDecoder);
	SafeRelease(&pSource);
	SafeRelease(&pConverter);
	return hr;
}

HRESULT Image::SaveToFile(PCWSTR uri)
{
	IWICStream *pStream = NULL;
	IWICBitmapEncoder *pEncoder = NULL;
	IWICBitmapFrameEncode *pFEncoder = NULL;
	HRESULT hr;
	hr = g_pImgFactory->CreateStream(&pStream);
	if (SUCCEEDED(hr))
	{
		hr = pStream->InitializeFromFilename(uri, GENERIC_WRITE);
	}
	if (SUCCEEDED(hr))
	{
		hr = g_pImgFactory->CreateEncoder(GUID_ContainerFormatPng, NULL, &pEncoder);
	}
	if (SUCCEEDED(hr))
	{
		hr = pEncoder->Initialize(pStream, WICBitmapEncoderNoCache);
	}
	if (SUCCEEDED(hr))
	{
		hr = pEncoder->CreateNewFrame(&pFEncoder, nullptr);
	}
	if (SUCCEEDED(hr))
	{
		hr = pFEncoder->Initialize(nullptr);
	}
	UINT width, height;
	if (SUCCEEDED(hr))
	{
		hr = pBitmap->GetSize(&width, &height);
	}
	if (SUCCEEDED(hr))
	{
		hr = pFEncoder->SetSize(width, height);
	}
	WICPixelFormatGUID format = GUID_WICPixelFormatDontCare;
	if (SUCCEEDED(hr))
	{
		hr = pFEncoder->SetPixelFormat(&format);
	}
	if (SUCCEEDED(hr))
	{
		hr = pFEncoder->WriteSource(pBitmap, nullptr);
	}
	if (SUCCEEDED(hr))
	{
		hr = pFEncoder->Commit();
	}
	if (SUCCEEDED(hr))
	{
		hr = pEncoder->Commit();
	}
	SafeRelease(&pFEncoder);
	SafeRelease(&pEncoder);
	SafeRelease(&pStream);
	return hr;
}

HRESULT Image::SaveToFileFloat(PCWSTR uri)
{
	IWICStream* pStream = NULL;
	IWICBitmapEncoder* pEncoder = NULL;
	IWICBitmapFrameEncode* pFEncoder = NULL;
	HRESULT hr;
	hr = g_pImgFactory->CreateStream(&pStream);
	if (SUCCEEDED(hr))
	{
		hr = pStream->InitializeFromFilename(uri, GENERIC_WRITE);
	}
	if (SUCCEEDED(hr))
	{
		hr = g_pImgFactory->CreateEncoder(GUID_ContainerFormatTiff, NULL, &pEncoder);
	}
	if (SUCCEEDED(hr))
	{
		hr = pEncoder->Initialize(pStream, WICBitmapEncoderNoCache);
	}
	if (SUCCEEDED(hr))
	{
		hr = pEncoder->CreateNewFrame(&pFEncoder, nullptr);
	}
	if (SUCCEEDED(hr))
	{
		hr = pFEncoder->Initialize(nullptr);
	}
	UINT width, height;
	if (SUCCEEDED(hr))
	{
		hr = pBitmap->GetSize(&width, &height);
	}
	if (SUCCEEDED(hr))
	{
		hr = pFEncoder->SetSize(width, height);
	}
	WICPixelFormatGUID format = GUID_WICPixelFormat64bppPRGBAHalf;
	if (SUCCEEDED(hr))
	{
		hr = pFEncoder->SetPixelFormat(&format);
	}
	if (SUCCEEDED(hr))
	{
		hr = pFEncoder->WriteSource(pBitmap, nullptr);
	}
	if (SUCCEEDED(hr))
	{
		hr = pFEncoder->Commit();
	}
	if (SUCCEEDED(hr))
	{
		hr = pEncoder->Commit();
	}
	SafeRelease(&pFEncoder);
	SafeRelease(&pEncoder);
	SafeRelease(&pStream);
	return hr;
}

HRESULT Image::MakeD2DBitmap(ID2D1Bitmap ** ppD2DBitmap)
{
	if (g_pRenderTarget)
	{
		SafeRelease(ppD2DBitmap);
		HRESULT hr = g_pRenderTarget->CreateBitmapFromWicBitmap(pBitmap, NULL, ppD2DBitmap);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"MakeD2DBitmap Failed", L"Error!", MB_OK);
		}
		return hr;
	}
	return -1;
}

void Image::PutPixel(int x, int y, UINT R, UINT G, UINT B, UINT A)
{
	if (x < 0 || y < 0 || x >= width || y >= height)return;
	UINT node = y * width + x;
	UINT a = pBuffer[node] >> 24, r = (pBuffer[node] >> 16) & 255, g = (pBuffer[node] >> 8) & 255, b = pBuffer[node] & 255;
	A = 255 - A;
	a = (a*A) >> 8;
	r = (r*A) >> 8;
	g = (g*A) >> 8;
	b = (b*A) >> 8;
	A = 255 - A;
	a += A; a = min(a, 255);
	r += R; r = min(r, 255);
	g += G; g = min(g, 255);
	b += B; b = min(b, 255);
	pBuffer[node] = (a << 24) | (r << 16) | (g << 8) | b;
}

void Image::PutPixel(int x, int y, DWORD Color)
{
	UINT A = Color >> 24, R = (Color >> 16) & 255, G = (Color >> 8) & 255, B = Color & 255;
	PutPixel(x, y, R, G, B, A);
}

void Image::PutPixel(int x, int y, D2D1_COLOR_F color)
{
	int a = 255 * color.a, r = 255 * color.r* color.a, g = 255 * color.g* color.a, b = 255 * color.b* color.a;
	return PutPixel(x, y, (a << 24) | (r << 16) | (g << 8) | b);
}

void Image::PutPixel(int x, int y, float R, float G, float B, float A)
{
	int a = 255 * A, r = 255 * R*A, g = 255 * G*A, b = 255 * B*A;
	return PutPixel(x, y, (a << 24) | (r << 16) | (g << 8) | b);
}

void Image::PutPixelFloat(int x, int y, D2D1_COLOR_F color)
{
	int index = (y * width + x) * 4;
	pBufferFloat[index] = color.r;
	pBufferFloat[index+1] = color.g;
	pBufferFloat[index+2] = color.b;
	pBufferFloat[index+3] = color.a;
}

DWORD Image::GetPixel(int x, int y) const
{
	if (illegal(x, y))return 0;
	return pBuffer[y*width + x];
}

bool Image::illegal(int x, int y) const
{
	return (x < 0 || y < 0 || x >= width || y >= height);
}

UINT Image::GetWidth() const
{
	return width;
}

UINT Image::GetHeight() const
{
	return height;
}

void Image::GetSize(UINT * Width, UINT * Height) const
{
	*Width = width;
	*Height = height;
}

void Image::GenerateRenderTarget()
{
	if (g_pFactory&&!pRenderTarget)g_pFactory->CreateWicBitmapRenderTarget(pBitmap, D2D1::RenderTargetProperties(), &pRenderTarget);
}

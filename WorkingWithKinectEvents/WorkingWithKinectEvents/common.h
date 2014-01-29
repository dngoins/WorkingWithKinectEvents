#pragma once
#include <wrl.h>
#include <algorithm>
#include <d2d1_1.h>
#include <d3d11.h>
#include <wincodec.h>

#pragma comment (lib, "d2d1")
#pragma comment (lib, "d3d11")
#pragma comment (lib, "dxgi")

#pragma warning(disable: 4706)	
#pragma warning(disable: 4127)

namespace wrl = Microsoft::WRL;
namespace d2d = D2D1;

using namespace std;
using namespace d2d;
using namespace wrl;



#define ASSERT(expression) _ASSERTE(expression)
D2D1_COLOR_F	const COLOR_BLUE = { 0.26f, 0.56f, 0.87f, 1.0f };
D2D1_COLOR_F	const COLOR_DARKBLUE = { 0.0f, 0.0f, 1.0f, 1.0f };
D2D1_COLOR_F	const COLOR_YELLOW = { 0.99f, 0.85f, 0.0f, 1.0f };
D2D1_COLOR_F	const COLOR_BLACK = { 0.0f, 0.0f, 0.07f, 1.0f };
D2D1_COLOR_F	const COLOR_WHITE = { 1.0f, 1.0f, 1.0f, 1.0f };
D2D1_COLOR_F	const COLOR_GREEN = { 0.0f, 1.0f, 0.0f, 1.0f };
D2D1_COLOR_F	const COLOR_RED = { 1.0f, 0.0f, 0.0f, 1.0f };

#ifdef _DEBUG
#define VERIFY(expression)	ASSERT(expression)
#define HR(expression)	ASSERT(S_OK == (expression	))


inline void TRACE(WCHAR const * const format, ...)
{
	va_list args;
	va_start(args, format);

	WCHAR output[512];
	vswprintf_s(output, format, args);

	OutputDebugString(output);

	va_end(args);
}

#else
#define VERIFY(expression) (expression)

struct ComException
{
	HRESULT const hr;
	ComException(HRESULT const value) :hr(value) {}

};

inline void HR(HRESULT const hr)
{
	if (S_OK != hr) throw ComException(hr);

}

#define TRACE __noop
#endif


#if WINAPI_FAMILY_DESKTOP_APP == WINAPI_FAMILY
#include <atlbase.h>
#include <atlwin.h>

using namespace ATL;

template <typename T>
void CreateInstance(REFCLSID clsid, wrl::ComPtr<T> & ptr)
{
	_ASSERT(!ptr);
	CoCreateInstance(clsid, nullptr, CLSCTX_INPROC_SERVER,
		__uuidof(T), reinterpret_cast<void **>(ptr.GetAddressOf()));

}



struct ComInitialize
{
	ComInitialize()
	{
		CoInitialize(nullptr);
	}

	~ComInitialize()
	{
		CoUninitialize();
	}


};

//// Safe release for interfaces
//template<class Interface>
//inline void SafeRelease(CComPtr<Interface> pInterfaceToRelease)
//{
//	if (pInterfaceToRelease)
//	{
//		pInterfaceToRelease = nullptr;
//		pInterfaceToRelease.Release();
//	}
//}

// Safe release for interfaces
template<class Interface>
inline void SafeRelease(ComPtr<Interface> pInterfaceToRelease)
{
	if (pInterfaceToRelease)
	{

		pInterfaceToRelease.Reset();
		pInterfaceToRelease = nullptr;
	}
}


// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != nullptr)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = nullptr;
	}
}

template <typename T>
struct DesktopWindow : CWindowImpl<DesktopWindow<T>, CWindow, CWinTraits<WS_POPUPWINDOW | WS_VISIBLE | WS_SYSMENU>>
{
	//wrl::ComPtr<IWICImagingFactory>			m_imageFactory;
	wrl::ComPtr<ID2D1Factory>				m_factory;
	wrl::ComPtr<ID2D1HwndRenderTarget>		m_target;
	wrl::ComPtr<ID2D1SolidColorBrush>		m_brush;
	wrl::ComPtr<ID2D1LinearGradientBrush>	m_linBrush;
	wrl::ComPtr<ID2D1RadialGradientBrush>	m_radBrush;
	wrl::ComPtr<ID2D1StrokeStyle>			m_style;
	wrl::ComPtr<ID2D1PathGeometry>			m_path;
	wrl::ComPtr<ID2D1PathGeometry>			m_arc1;
	wrl::ComPtr<ID2D1PathGeometry>			m_arc2;
	wrl::ComPtr<ID2D1PathGeometry>			m_arc3;
	wrl::ComPtr<ID2D1PathGeometry>			m_arc4;
	D2D1_POINT_2F							m_begin;
	D2D1_POINT_2F							m_end;
	D2D1_POINT_2F							m_c1;
	D2D1_POINT_2F							m_c2;
	wrl::ComPtr<IWICFormatConverter>		m_image;
	wrl::ComPtr<ID2D1Bitmap>				m_bitmap;
	wrl::ComPtr<ID2D1BitmapBrush>			m_bmBrush;
	wrl::ComPtr<ID2D1SolidColorBrush>		m_pBrushJointTracked;
	wrl::ComPtr<ID2D1SolidColorBrush>		m_pBrushJointInferred;
	wrl::ComPtr<ID2D1SolidColorBrush>		m_pBrushBoneTracked;
	wrl::ComPtr<ID2D1SolidColorBrush>		m_pBrushBoneInferred;
	wrl::ComPtr<ID2D1SolidColorBrush>		m_pBrushHandClosed;
	wrl::ComPtr<ID2D1SolidColorBrush>		m_pBrushHandOpen;
	wrl::ComPtr<ID2D1SolidColorBrush>		m_pBrushHandLasso;
	ComInitialize							comInit;
	bool									m_isClosing;
	D2D1_COLOR_F							m_backgroundColor;

	static CWndClassInfo& GetWndClassInfo()
	{

		// a manual DECLARE_WND_CLASS macro expansion
		// modified to specify an application-defined cursor:
		static CWndClassInfo wc =
		{

			{ sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW,
			StartWndProc,
			0, 0, NULL, NULL, NULL, NULL, NULL,
			L"HeartRateWindow", NULL },
			NULL, NULL, NULL, FALSE, 0, _T("")
		};
		return wc;
		//	/*
		//	struct WNDCLASSEX
		//	{
		//	UINT        cbSize;
		//	UINT        style;
		//	WNDPROC     lpfnWndProc;
		//	int         cbClsExtra;
		//	int         cbWndExtra;
		//	HINSTANCE   hInstance;
		//	HICON       hIcon;
		//	HCURSOR     hCursor;
		//	HBRUSH      hbrBackground;
		//	LPCSTR      lpszMenuName;
		//	LPCSTR      lpszClassName;
		//	HICON       hIconSm;
		//	} m_wc;
		//	*/
	}

	//DECLARE_WND_CLASS_EX(L"HearRateWindow", CS_HREDRAW | CS_VREDRAW, -1);


	BEGIN_MSG_MAP(c)
		MESSAGE_HANDLER(WM_PAINT, PaintHandler)
		MESSAGE_HANDLER(WM_CLOSE, CloseHandler)
		MESSAGE_HANDLER(WM_DESTROY, DestroyHandler)
		MESSAGE_HANDLER(WM_SIZE, SizeHandler)
		MESSAGE_HANDLER(WM_DISPLAYCHANGE, DisplayChangeHandler)
		MESSAGE_HANDLER(WM_MOUSEMOVE, MouseMoveHandler)
		MESSAGE_HANDLER(WM_CREATE, CreateHandler)
		MESSAGE_HANDLER(WM_QUIT, DestroyHandler)
	END_MSG_MAP()



	LRESULT MouseMoveHandler(UINT, WPARAM wparam, LPARAM lparam, BOOL &)
	{
		float x = (LOWORD(lparam));
		float y = (HIWORD(lparam));
		MouseMoved(x, y, wparam);
		return 0;
	}

	LRESULT CloseHandler(UINT, WPARAM, LPARAM, BOOL &)
	{
		m_isClosing = true;
		DestroyWindow();
		Detach();
		//		__super::DestroyWindow();

		return 0;
	}

	LRESULT PaintHandler(UINT, WPARAM, LPARAM, BOOL &)
	{
		PAINTSTRUCT ps;
		VERIFY(BeginPaint(&ps));
		Render();
		EndPaint(&ps);
		return 0;

	}

	LRESULT DisplayChangeHandler(UINT, WPARAM, LPARAM, BOOL &)
	{
		Invalidate();
		return 0;
	}

	LRESULT SizeHandler(UINT, WPARAM, LPARAM lparam, BOOL &)
	{
		if (m_target)
		{
			HR(m_target->Resize(SizeU(LOWORD(lparam), HIWORD(lparam))));
			SafeRelease(m_target);
		}

		return 0;
	}

	void Invalidate()
	{
		InvalidateRect(nullptr, false);
	}

	HWND Create()
	{
		D2D1_FACTORY_OPTIONS fo = {};

#ifndef _DEBUG
		fo.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

		HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
			fo,
			m_factory.GetAddressOf()));

		CreateDeviceIndependentResources();


		return __super::Create(NULL, NULL, L"HeartRateWindow", WS_OVERLAPPEDWINDOW | WS_VISIBLE, NULL, 0U, reinterpret_cast<LPVOID>(this));

	}


	virtual void Initialize(WPARAM, LPARAM) = 0;

	LRESULT CreateHandler(UINT, WPARAM wparam, LPARAM, BOOL &)
	{
		//std::shared_ptr<DesktopWindow<T>> pThis = nullptr;

		//pThis = reinterpret_cast<std::shared_ptr<DesktopWindow<T>>>(lparam);
		//SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));

		//pThis = reinterpret_cast<std::shared_ptr<DesktopWindow<T>>>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

		Initialize(wparam, reinterpret_cast<LPARAM>(this));
		return 0;
	}


	virtual void Update()
	{
	}

	static LRESULT CALLBACK StartWndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		MSG message;
		BOOL result;


		result = PeekMessageW(&message, NULL, 0, 0, PM_REMOVE);
		{
			//Update();
			if (-1 != result)
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		}

		//if (result == 0) return 0;
		//		if (msg == WM_CLOSE) return 0;

		auto ret = __super::StartWindowProc(hWnd, msg, wparam, lparam);//static_cast<LRESULT>(message.wParam); // __super::StartWindowProc(hWnd, msg, wparam, lparam);
		return 0;
	}


	void CreateDeviceIndependentResources()
	{
		//CreateInstance(CLSID_WICImagingFactory, m_imageFactory);

	}

	void CreateDeviceResources()
	{
		OutputDebugString(L"Create Device Resources");

		m_target->CreateSolidColorBrush(m_backgroundColor,
			BrushProperties(0.5f),
			m_brush.ReleaseAndGetAddressOf());

		OutputDebugString(L"Create Device Resources End");

		D2D1_GRADIENT_STOP stops[] =
		{
			{ 0.0f, COLOR_WHITE },
			{ 1.0f, COLOR_BLUE }

		};

		D2D1_GRADIENT_STOP radStops[] =
		{
			{ 0.0f, COLOR_BLUE },
			{ 0.7f, COLOR_YELLOW },
			{ 1.0f, COLOR_WHITE }
		};


		ComPtr<ID2D1GradientStopCollection> collection;
		ComPtr<ID2D1GradientStopCollection> radCollection;

		m_target->CreateGradientStopCollection(stops, _countof(stops), collection.GetAddressOf());

		m_target->CreateGradientStopCollection(radStops, _countof(radStops), radCollection.GetAddressOf());

		D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES	props = {};

		m_target->CreateLinearGradientBrush(props, collection.Get(), m_linBrush.ReleaseAndGetAddressOf());

		D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES	 radProps = {};
		m_target->CreateRadialGradientBrush(radProps, radCollection.Get(), m_radBrush.ReleaseAndGetAddressOf());


		// light green
		m_target->CreateSolidColorBrush(D2D1::ColorF(0.27f, 0.75f, 0.27f), &m_pBrushJointTracked);

		m_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow, 1.0f), &m_pBrushJointInferred);
		m_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green, 1.0f), &m_pBrushBoneTracked);
		m_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray, 1.0f), &m_pBrushBoneInferred);

		m_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red, 0.5f), &m_pBrushHandClosed);
		m_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green, 0.5f), &m_pBrushHandOpen);
		m_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue, 0.5f), &m_pBrushHandLasso);
	}

	virtual void Render()
	{
		//OutputDebugString(L"Render Start");

		if (!m_target && m_factory)
		{
			//need size of rect and window handle
			//GetClientRect passed wnd handle
			RECT rect;
			GetClientRect(&rect);

			OutputDebugString(L"Retrieved Client Rect");

			auto size = d2d::SizeU(rect.right, rect.bottom);

			if SUCCEEDED(m_factory->CreateHwndRenderTarget(d2d::RenderTargetProperties(),
				d2d::HwndRenderTargetProperties(m_hWnd, size),
				m_target.GetAddressOf()))
			{
				OutputDebugString(L"Created HwndRenderTarget ");
				CreateDeviceResources();
			}

			OutputDebugString(L"Render End");



		}

		//Check to see if windows is not hidden and 
		if (m_target)
		{

			if (!(D2D1_WINDOW_STATE_OCCLUDED & m_target->CheckWindowState()))
			{
				//m_target->BeginDraw();

				Draw();

				/*if (D2DERR_RECREATE_TARGET == m_target->EndDraw())
				{
				m_target.Reset();
				}
				*/
			}
		}
	}

	virtual void Draw()
	{
		m_target->BeginDraw();

		m_target->Clear(COLOR_WHITE);

		auto size = m_target->GetSize();
		auto rect = RectF(0.0f, 0.0f, size.width, size.height);

		//	m_target->DrawBitmap(m_bitmap.Get(), rect, 0.6f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
		m_target->FillRectangle(rect, m_linBrush.Get());
		//Update();

		if (D2DERR_RECREATE_TARGET == m_target->EndDraw())
		{
			SafeRelease(m_target);
		}


	}

	void MouseMoved(float /*x*/, float /*y*/, WPARAM)
	{
	}

	LRESULT DestroyHandler(UINT, WPARAM, LPARAM, BOOL &)
	{
		m_isClosing = true;
		DestroyWindow();
		PostQuitMessage(0);
		return 0;

	}

	DesktopWindow<T>() : m_factory(nullptr),
		m_target(nullptr),
		m_brush(nullptr),
		m_linBrush(nullptr),
		m_radBrush(nullptr),
		m_style(nullptr),
		m_path(nullptr),
		m_arc1(nullptr),
		m_arc2(nullptr),
		m_arc3(nullptr),
		m_arc4(nullptr),
		m_image(nullptr),
		m_bitmap(nullptr),
		m_bmBrush(nullptr),
		m_pBrushJointTracked(nullptr),
		m_pBrushJointInferred(nullptr),
		m_pBrushBoneTracked(nullptr),
		m_pBrushBoneInferred(nullptr),
		m_pBrushHandClosed(nullptr),
		m_pBrushHandOpen(nullptr),
		m_pBrushHandLasso(nullptr),
		m_isClosing(false),
		m_backgroundColor(COLOR_WHITE)
	{

	}

	DesktopWindow<T>(D2D1_COLOR_F color) : m_factory(nullptr),
		m_target(nullptr),
		m_brush(nullptr),
		m_linBrush(nullptr),
		m_radBrush(nullptr),
		m_style(nullptr),
		m_path(nullptr),
		m_arc1(nullptr),
		m_arc2(nullptr),
		m_arc3(nullptr),
		m_arc4(nullptr),
		m_image(nullptr),
		m_bitmap(nullptr),
		m_bmBrush(nullptr),
		m_pBrushJointTracked(nullptr),
		m_pBrushJointInferred(nullptr),
		m_pBrushBoneTracked(nullptr),
		m_pBrushBoneInferred(nullptr),
		m_pBrushHandClosed(nullptr),
		m_pBrushHandOpen(nullptr),
		m_pBrushHandLasso(nullptr),
		m_isClosing(false),
		m_backgroundColor(color)
	{

	}

	virtual ~DesktopWindow<T>() {

		SafeRelease(m_path);

		SafeRelease(m_arc1);
		SafeRelease(m_arc2);
		SafeRelease(m_arc3);
		SafeRelease(m_arc4);

		SafeRelease(m_target);
		SafeRelease(m_bitmap);
		SafeRelease(m_bmBrush);
		SafeRelease(m_factory);
		SafeRelease(m_image);
		SafeRelease(m_linBrush);

		SafeRelease(m_pBrushBoneInferred);
		SafeRelease(m_pBrushBoneTracked);
		SafeRelease(m_pBrushHandClosed);
		SafeRelease(m_pBrushHandLasso);
		SafeRelease(m_pBrushHandOpen);
		SafeRelease(m_pBrushJointInferred);
		SafeRelease(m_pBrushJointTracked);


	}

};


template <typename T>
struct WorkerThreadController
{
public:
	WorkerThreadController() {

	}
	~WorkerThreadController() {  }


	static DWORD WINAPI StartMainLoop(LPVOID pwindow)
	{
		MSG msg = { 0 };

		while (pwindow)
		{

			T * pSkeleton = reinterpret_cast<T *>(pwindow);
			TRACE(L"Calling Update in worker thread main loop");

			pSkeleton->Update();
			Sleep(10);
		}


		return 0;
	}

};

#endif




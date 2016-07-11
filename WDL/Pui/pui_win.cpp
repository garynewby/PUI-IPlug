
// pui_win.cpp
// PUI
// Created by Gary Newby on 12/30/15.
// Based on by PuGL by David Robillard <http://drobilla.net>


#include <windows.h>
#include <windowsx.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include "pui.hpp"

#ifndef WM_MOUSEWHEEL
#    define WM_MOUSEWHEEL 0x020A
#endif
#ifndef WM_MOUSEHWHEEL
#    define WM_MOUSEHWHEEL 0x020E
#endif
#ifndef WHEEL_DELTA
#    define WHEEL_DELTA 120
#endif
#ifdef _WIN64
#    ifndef GWLP_USERDATA
#        define GWLP_USERDATA (-21)
#    endif
#else
#    ifndef GWL_USERDATA
#        define GWL_USERDATA (-21)
#    endif
#endif

#define PUI_LOCAL_CLOSE_MSG (WM_USER + 50)

struct PuiInternalsImpl {
	HWND     hwnd;
	HDC      hdc;
	HGLRC    hglrc;
	WNDCLASS wc;
};

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

PuiView* puiInit() {
	PuiView*      view = (PuiView*)calloc(1, sizeof(PuiView));
	PuiInternals* impl = (PuiInternals*)calloc(1, sizeof(PuiInternals));
	if (!view || !impl) {
		return NULL;
	}

	view->impl   = impl;
	view->width  = 640;
	view->height = 480;

	return view;
}

PuiInternals* puiInitInternals() {
	return (PuiInternals*)calloc(1, sizeof(PuiInternals));
}

void puiEnterContext(PuiView* view) {
#ifdef PUI_HAVE_GL
	if (view->ctx_type == PUI_GL) {
		wglMakeCurrent(view->impl->hdc, view->impl->hglrc);
	}
#endif
}

void puiLeaveContext(PuiView* view, bool flush)
{
#ifdef PUI_HAVE_GL
	if (view->ctx_type == PUI_GL && flush) {
		glFlush();
		SwapBuffers(view->impl->hdc);
	}
#endif
}

int puiCreateWindow(PuiView* view, const char* title)
{
	PuiInternals* impl = view->impl;

	if (!title) {
		title = "Window";
	}

	// FIXME: This is nasty, and pui should not have static anything.
	// Should class be a parameter?  Does this make sense on other platforms?
	static int wc_count = 0;
	char classNameBuf[256];
	_snprintf(classNameBuf, sizeof(classNameBuf), "x%d%s", wc_count++, title);
	classNameBuf[sizeof(classNameBuf) - 1] = '\0';

	impl->wc.style         = CS_OWNDC;
	impl->wc.lpfnWndProc   = wndProc;
	impl->wc.cbClsExtra    = 0;
	impl->wc.cbWndExtra    = 0;
	impl->wc.hInstance     = 0;
	impl->wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	impl->wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	impl->wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	impl->wc.lpszMenuName  = NULL;
	impl->wc.lpszClassName = strdup(classNameBuf);

	if (!RegisterClass(&impl->wc)) {
		free((void*)impl->wc.lpszClassName);
		free(impl);
		free(view);
		return NULL;
	}

	int winFlags = WS_POPUPWINDOW | WS_CAPTION;
	if (view->resizable) {
		winFlags |= WS_SIZEBOX;
		if (view->min_width || view->min_height) {
			// Adjust the minimum window size to accomodate requested view size
			RECT mr = { 0, 0, view->min_width, view->min_height };
			AdjustWindowRectEx(&mr, winFlags, FALSE, WS_EX_TOPMOST);
			view->min_width  = mr.right - mr.left;
			view->min_height = wr.bottom - mr.top;
		}
	}

	// Adjust the window size to accomodate requested view size
	RECT wr = { 0, 0, view->width, view->height };
	AdjustWindowRectEx(&wr, winFlags, FALSE, WS_EX_TOPMOST);

	impl->hwnd = CreateWindowEx(
		WS_EX_TOPMOST,
 		classNameBuf, title,
		(view->parent ? WS_CHILD : winFlags),
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right-wr.left, wr.bottom-wr.top,
		(HWND)view->parent, NULL, NULL, NULL);

	if (!impl->hwnd) {
		free((void*)impl->wc.lpszClassName);
		free(impl);
		free(view);
		return 1;
	}

#ifdef _WIN64
	SetWindowLongPtr(impl->hwnd, GWLP_USERDATA, (LONG_PTR)view);
#else
 	SetWindowLongPtr(impl->hwnd, GWL_USERDATA, (LONG)view);
#endif

	impl->hdc = GetDC(impl->hwnd);

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize      = sizeof(pfd);
	pfd.nVersion   = 1;
	pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int format = ChoosePixelFormat(impl->hdc, &pfd);
	SetPixelFormat(impl->hdc, format, &pfd);

	impl->hglrc = wglCreateContext(impl->hdc);
	if (!impl->hglrc) {
		ReleaseDC(impl->hwnd, impl->hdc);
		DestroyWindow(impl->hwnd);
		UnregisterClass(impl->wc.lpszClassName, NULL);
		free((void*)impl->wc.lpszClassName);
		free(impl);
		free(view);
		return NULL;
	}
	wglMakeCurrent(impl->hdc, impl->hglrc);

	return 0;
}

void puiShowWindow(PuiView* view)
{
	PuiInternals* impl = view->impl;
	ShowWindow(impl->hwnd, SW_SHOWNORMAL);
}

void puiHideWindow(PuiView* view)
{
	PuiInternals* impl = view->impl;
	ShowWindow(impl->hwnd, SW_HIDE);
}

void puiDestroy(PuiView* view)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(view->impl->hglrc);
	ReleaseDC(view->impl->hwnd, view->impl->hdc);
	DestroyWindow(view->impl->hwnd);
	UnregisterClass(view->impl->wc.lpszClassName, NULL);
	free(view->impl);
	free(view);
}

static void puiReshape(PuiView* view, int width, int height)
{
	puiEnterContext(view);
	if (view->reshapeFunc) {
		view->reshapeFunc(view, width, height);
	}

	view->width  = width;
	view->height = height;
}

static void puiDisplay(PuiView* view)
{
	puiEnterContext(view);
	if (view->displayFunc) {
		view->displayFunc(view);
	}

	puiLeaveContext(view, true);
	view->redisplay = false;
}

static PuiKey keySymToSpecial(int sym)
{
	switch (sym) {
	case VK_F1:      return PUI_KEY_F1;
	case VK_F2:      return PUI_KEY_F2;
	case VK_F3:      return PUI_KEY_F3;
	case VK_F4:      return PUI_KEY_F4;
	case VK_F5:      return PUI_KEY_F5;
	case VK_F6:      return PUI_KEY_F6;
	case VK_F7:      return PUI_KEY_F7;
	case VK_F8:      return PUI_KEY_F8;
	case VK_F9:      return PUI_KEY_F9;
	case VK_F10:     return PUI_KEY_F10;
	case VK_F11:     return PUI_KEY_F11;
	case VK_F12:     return PUI_KEY_F12;
	case VK_LEFT:    return PUI_KEY_LEFT;
	case VK_UP:      return PUI_KEY_UP;
	case VK_RIGHT:   return PUI_KEY_RIGHT;
	case VK_DOWN:    return PUI_KEY_DOWN;
	case VK_PRIOR:   return PUI_KEY_PAGE_UP;
	case VK_NEXT:    return PUI_KEY_PAGE_DOWN;
	case VK_HOME:    return PUI_KEY_HOME;
	case VK_END:     return PUI_KEY_END;
	case VK_INSERT:  return PUI_KEY_INSERT;
	case VK_SHIFT:   return PUI_KEY_SHIFT;
	case VK_CONTROL: return PUI_KEY_CTRL;
	case VK_MENU:    return PUI_KEY_ALT;
	case VK_LWIN:    return PUI_KEY_SUPER;
	case VK_RWIN:    return PUI_KEY_SUPER;
	}
	return (PuiKey)0;
}

static void processMouseEvent(PuiView* view, int button, bool press, LPARAM lParam)
{
	view->event_timestamp_ms = GetMessageTime();
	if (press) {
		SetCapture(view->impl->hwnd);
	} else {
		ReleaseCapture();
	}

	if (view->mouseFunc) {
		view->mouseFunc(view, button, press,
		                GET_X_LPARAM(lParam),
		                GET_Y_LPARAM(lParam));
	}
}

static void setModifiers(PuiView* view)
{
	view->mods = 0;
	view->mods |= (GetKeyState(VK_SHIFT)   < 0) ? PUI_MOD_SHIFT  : 0;
	view->mods |= (GetKeyState(VK_CONTROL) < 0) ? PUI_MOD_CTRL   : 0;
	view->mods |= (GetKeyState(VK_MENU)    < 0) ? PUI_MOD_ALT    : 0;
	view->mods |= (GetKeyState(VK_LWIN)    < 0) ? PUI_MOD_SUPER  : 0;
	view->mods |= (GetKeyState(VK_RWIN)    < 0) ? PUI_MOD_SUPER  : 0;
}

static LRESULT handleMessage(PuiView* view, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	PuiKey     key;
	RECT        rect;
	MINMAXINFO* mmi;

	setModifiers(view);
	switch (message) {
	case WM_CREATE:
	case WM_SHOWWINDOW:
	case WM_SIZE:
		GetClientRect(view->impl->hwnd, &rect);
		puiReshape(view, rect.right, rect.bottom);
		view->width = rect.right;
		view->height = rect.bottom;
		break;
	case WM_GETMINMAXINFO:
		mmi = (MINMAXINFO*)lParam;
		mmi->ptMinTrackSize.x = view->min_width;
		mmi->ptMinTrackSize.y = view->min_height;
		break;
	case WM_PAINT:
		BeginPaint(view->impl->hwnd, &ps);
		puiDisplay(view);
		EndPaint(view->impl->hwnd, &ps);
		break;
	case WM_MOUSEMOVE:
		if (view->motionFunc) {
			view->event_timestamp_ms = GetMessageTime();
			view->motionFunc(view, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		}
		break;
	case WM_LBUTTONDOWN:
		processMouseEvent(view, 1, true, lParam);
		break;
	case WM_MBUTTONDOWN:
		processMouseEvent(view, 2, true, lParam);
		break;
	case WM_RBUTTONDOWN:
		processMouseEvent(view, 3, true, lParam);
		break;
	case WM_LBUTTONUP:
		processMouseEvent(view, 1, false, lParam);
		break;
	case WM_MBUTTONUP:
		processMouseEvent(view, 2, false, lParam);
		break;
	case WM_RBUTTONUP:
		processMouseEvent(view, 3, false, lParam);
		break;
	case WM_MOUSEWHEEL:
		if (view->scrollFunc) {
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ScreenToClient(view->impl->hwnd, &pt);
			view->event_timestamp_ms = GetMessageTime();
			view->scrollFunc(
				view, pt.x, pt.y,
				0.0f, GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA);
		}
		break;
	case WM_MOUSEHWHEEL:
		if (view->scrollFunc) {
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ScreenToClient(view->impl->hwnd, &pt);
			view->event_timestamp_ms = GetMessageTime();
			view->scrollFunc(
				view, pt.x, pt.y,
				GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA, 0.0f);
		}
		break;
	case WM_KEYDOWN:
		if (view->ignoreKeyRepeat && (lParam & (1 << 30))) {
			break;
		} // else nobreak
	case WM_KEYUP:
		view->event_timestamp_ms = GetMessageTime();
		if ((key = keySymToSpecial(wParam))) {
			if (view->specialFunc) {
				view->specialFunc(view, message == WM_KEYDOWN, key);
			}
		} else if (view->keyboardFunc) {
			view->keyboardFunc(view, message == WM_KEYDOWN, wParam);
		}
		break;
	case WM_QUIT:
	case PUI_LOCAL_CLOSE_MSG:
		if (view->closeFunc) {
			view->closeFunc(view);
			view->redisplay = false;
		}
		break;
	default:
		return DefWindowProc(
			view->impl->hwnd, message, wParam, lParam);
	}

	return 0;
}

void puiGrabFocus(PuiView* view)
{
	// TODO
}

PuiStatus puiProcessEvents(PuiView* view)
{
	MSG msg;
	while (PeekMessage(&msg, view->impl->hwnd, 0, 0, PM_REMOVE)) {
		handleMessage(view, msg.message, msg.wParam, msg.lParam);
	}


	if (view->redisplay) {
		InvalidateRect(view->impl->hwnd, NULL, FALSE);
	}

	return PUI_SUCCESS;
}

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN64
	PuiView* view = (PuiView*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
#else
	PuiView* view = (PuiView*)GetWindowLongPtr(hwnd, GWL_USERDATA);
#endif

	switch (message) {
	case WM_CREATE:
		PostMessage(hwnd, WM_SHOWWINDOW, TRUE, 0);
		return 0;
	case WM_CLOSE:
		PostMessage(hwnd, PUI_LOCAL_CLOSE_MSG, wParam, lParam);
		return 0;
	case WM_DESTROY:
		return 0;
	default:
		if (view && hwnd == view->impl->hwnd) {
			return handleMessage(view, message, wParam, lParam);
		} else {
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
	}
}

void puiPostRedisplay(PuiView* view)
{
	view->redisplay = true;
}

PuiNativeWindow puiGetNativeWindow(PuiView* view)
{
	return (PuiNativeWindow)view->impl->hwnd;
}

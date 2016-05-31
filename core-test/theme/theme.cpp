#include <main>

COLORREF theme::backColor = RGB(45, 45, 48);
HBRUSH theme::backBrush = CreateSolidBrush(backColor);

void theme::setWindowStyle(HWND& wnd) {
	SetClassLongPtr(wnd, GCLP_HBRBACKGROUND, (LONG)backBrush);
}

void theme::setStatusBarStyle(HWND& wnd) {
	SendMessage(wnd, SB_SETBKCOLOR, NULL, backColor);
}



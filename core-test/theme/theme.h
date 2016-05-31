#pragma once
class theme {
public:

	static COLORREF backColor;
	static HBRUSH backBrush;

	static void setWindowStyle(HWND& wnd);
	static void setStatusBarStyle(HWND& wnd);
};

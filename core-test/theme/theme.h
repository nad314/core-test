#pragma once
class theme {
public:

	static COLORREF backColor;
	static COLORREF textColor;
	static COLORREF controlColor;
	static COLORREF controlHoverColor;
	static COLORREF controlClickedColor;
	static HBRUSH backBrush;
	static HBRUSH controlBrush;
	static HBRUSH controlHoverBrush;
	static HBRUSH controlClickedBrush;
	static HFONT font;
	static HPEN pen;
	static HPEN controlBorderPen;

	static void initStyles();
	static void setWindowStyle(HWND& wnd);
	static void setStatusBarStyle(HWND& wnd);
	static bool drawControl(DRAWITEMSTRUCT *ds);
};

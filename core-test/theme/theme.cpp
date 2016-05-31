#include <main>

COLORREF theme::backColor = RGB(51, 51, 54);
COLORREF theme::textColor = RGB(255, 242, 230);
COLORREF theme::controlColor = RGB(45, 45, 48);
COLORREF theme::controlHoverColor = RGB(51, 51, 54);
COLORREF theme::controlClickedColor = RGB(45, 45, 48);
HBRUSH theme::backBrush = CreateSolidBrush(backColor);
HBRUSH theme::controlBrush = CreateSolidBrush(controlColor);
HBRUSH theme::controlHoverBrush = CreateSolidBrush(controlHoverColor);
HBRUSH theme::controlClickedBrush = CreateSolidBrush(controlClickedColor);
HFONT theme::font = (HFONT)DEFAULT_GUI_FONT;
HPEN theme::pen = CreatePen(0, 1, backColor);
HPEN theme::controlBorderPen = CreatePen(0, 1, RGB(119, 136, 153));

void theme::initStyles() {
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));
	logFont.lfHeight = -11; // see PS
	logFont.lfWeight = FW_NORMAL;
	strcpy(logFont.lfFaceName, "Segoe UI");
	font = CreateFontIndirect(&logFont);
}

void theme::setWindowStyle(HWND& wnd) {
	SetClassLongPtr(wnd, GCLP_HBRBACKGROUND, (LONG)backBrush);
}

void theme::setStatusBarStyle(HWND& wnd) {
	SendMessage(wnd, SB_SETBKCOLOR, NULL, backColor);
}

bool theme::drawControl(DRAWITEMSTRUCT *ds) {
	if (!ds) return false;
	HWND &hWnd = ds->hwndItem;
	core::windowObject* wo = static_cast<core::windowObject*>((void*)GetWindowLongPtrA(hWnd, GWL_USERDATA));
	if (!wo) return false;
	switch (ds->CtlType) {
	case ODT_BUTTON: {
		buttonWindow* btn = static_cast<buttonWindow*>(wo);
		if (btn) btn->onDraw(ds);
		break;
	}
	default: break;
	}
	return true;
}



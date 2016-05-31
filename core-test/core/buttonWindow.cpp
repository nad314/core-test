#include <main>
void buttonWindow::setText(const char* caption) {
	strcpy(text, caption);
	SetWindowTextA(hWnd, text);
}

std::string buttonWindow::getText() { return text; }

void buttonWindow::open(const int& posx, const int& posy, const int& sizex, const int& sizey, windowObject& wParent) {
	parent = &wParent;
	x = posx;
	y = posy;
	width = sizex;
	height = sizey;
	hWnd = CreateWindowExW(NULL, L"BUTTON", L"Button", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, x, y, width, height, wParent, NULL, hInstance, NULL);
	SetWindowLongPtrA(hWnd, GWL_USERDATA, (LONG)this);
	SendMessage(hWnd, WM_SETFONT, (WPARAM)DEFAULT_GUI_FONT, MAKELPARAM(TRUE, 0));
	setText("Button");
}

void buttonWindow::open(const int& posx, const int& posy, const int& sizex, const int& sizey, windowObject& wParent, char* caption) {
	open(posx, posy, sizex, sizey, wParent);
	setText(caption);
}

void buttonWindow::onBeginDraw(DRAWITEMSTRUCT* ds) {
	hDC = ds->hDC;
	SetTextColor(hDC, theme::textColor);
	SelectObject(hDC, theme::font);
	if (ds->itemState&ODS_SELECTED) {
		SelectObject(hDC, theme::controlHoverBrush);
		SelectObject(hDC, theme::controlBorderPen);
	}
	else {
		SelectObject(hDC, theme::controlBrush);
		SelectObject(hDC, theme::controlBorderPen);
	}
	SetBkMode(hDC, TRANSPARENT);
}

void buttonWindow::onDraw(DRAWITEMSTRUCT* ds) {
	onBeginDraw(ds);
	PAINTSTRUCT ps;
	RECT rect;
	GetClientRect(hWnd, &rect);
	BeginPaint(hWnd, &ps);
	Rectangle(hDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top);
	EndPaint(hWnd, &ps);
	DrawText(hDC, text, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}

#include <main>

void RenderWindow::onOpening() {
	setTitle("Core Frame");
	setClass("CoreFrame");
	setStyle(WS_CHILD | WS_VISIBLE);
	setExStyle(WS_EX_CLIENTEDGE);
	setFlags(0);
}

void RenderWindow::move(int xw, int yw) {
	int borderSize = 0;
	xw -= borderSize*2;
	yw -= borderSize*2;
	MoveWindow(hWnd, borderSize, borderSize, xw, yw, true);
	width = xw; 
	height = yw;
}

void RenderWindow::moveMouse(const int& x, const int& y) {
	mouse = core::vec2i(x, height-y);
}

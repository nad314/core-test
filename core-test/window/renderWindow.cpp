#include <main>

void renderWindow::onOpening() {
	setTitle("Core Frame");
	setClass("CoreFrame");
	setStyle(WS_CHILD | WS_VISIBLE);
	setExStyle(WS_EX_CLIENTEDGE);
	setFlags(0);
}

void renderWindow::move(int xw, int yw) {
	yw -= 22;
	MoveWindow(hWnd, 0, 0, xw, yw, true);
	width = xw; 
	height = yw;
}

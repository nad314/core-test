#include <main>

void renderWindow::onOpening() {
	setTitle("Core Frame");
	setClass("CoreFrame");
	setStyle(WS_CHILD | WS_VISIBLE);
	setExStyle(NULL);
	setFlags(0);
}

void renderWindow::move(int xw, int yw) {
	int borderSize = 4;
	yw -= 22;
	xw -= borderSize*2;
	yw -= borderSize*2;
	MoveWindow(hWnd, borderSize, borderSize, xw, yw, true);
	width = xw; 
	height = yw;
}

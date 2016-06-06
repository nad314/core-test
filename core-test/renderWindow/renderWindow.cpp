#include <main>

void RenderWindow::onOpening() {
	setTitle("Core Frame");
	setClass("CoreFrame");
	setStyle(WS_CHILD | WS_VISIBLE);
	setFlags(0);
}

void RenderWindow::onOpened() {
	if (width == 0 || height == 0) 
		return;
	view.make(width, height);
	view.home();
}

int RenderWindow::onResize(const core::eventInfo& e) {
	Window::onResize(e);
	if (width == 0 || height == 0)
		return e;
	view.make(width, height);
	view.home();
	GL::init(*this);
}


void RenderWindow::move(int xw, int yw) {
	int borderSize = 8;
	xw -= borderSize*2;
	yw -= borderSize*2+32;
	MoveWindow(hWnd, borderSize, borderSize+32, xw, yw, true);
	width = xw; 
	height = yw;
}


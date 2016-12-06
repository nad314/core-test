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
	GL::createContext(*this);
	GL::init(*this);
}

int RenderWindow::onResize(const core::eventInfo& e) {
	Window::onResize(e);
	if (width == 0 || height == 0)
		return e;
	view.make(width, height);
	//view.home();
	view.updateMatrix();
	GL::ortho(*this);

	Controller& c = Controller::get();
	for (int i = 0; i < c.threads; ++i)
		c.thread[i].push(new core::msRenderTask(&c.storage->pbvh, &view, c.samples));
	c.invalidate();
}


void RenderWindow::move(int xw, int yw) {
	int borderSize = 8;
	xw -= borderSize*2;
	yw -= borderSize*2+32;
	MoveWindow(hWnd, borderSize, borderSize+32, xw, yw, true);
	width = xw; 
	height = yw;
}



#include <main>

void MainWindow::onOpening() {
	Form::onOpening();
	setStyle( WS_POPUP );
	setTitle("Core Renderer");
	setClass("CoreRenderer");
}

void MainWindow::onOpened() {
	Form::onOpened();
	rwnd.setParent(this);
	rwnd.open();

	GL::createContext(rwnd);
	GL::init(rwnd);
	GL::setVsync(0);
	Reshape();

	closeButton.make(core::vec4i(width-85, 5, width-5, 25), "Close", *this);
	//push(closeButton);
}

void MainWindow::onClosing() {
	Form::onClosing();
	rwnd.close();
}

int MainWindow::onResize(const core::eventInfo &e) {
	Form::onResize(e);
	if (width < 1 || height < 1)
		return e;
	core::vec4i r = getClientRect();
	rwnd.move(r.z-r.x, r.w-r.y);
	//MoveWindow(hStatusBar, 0, r.bottom-20, r.right-r.left, 20, true);
	return e;
}

int MainWindow::onDefault(const core::eventInfo &e) {
	switch (e.msg) {
	case WM_LBUTTONDBLCLK: if (LOWORD(e.lP) < 25 && HIWORD(e.lP) < 25)PostQuitMessage(0); return e;
	case WM_LBUTTONDOWN: SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0); return e;
	case WM_NCHITTEST: return onNcHitTest(e);
	}
	return e;
}

void MainWindow::onEndPaint(const core::eventInfo& e) {
	core::Renderer::fillRect(core::vec4i(1, 1, width-100, 30), core::vec4b(45, 45, 48, 255), img);
	core::Renderer::print("Core Renderer", img, 12, 12);
	core::Renderer::drawRect(getClientRect(), core::vec4b(0, 122, 204, 255), *this);
}



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

	closeButton.make(core::vec4i(width - 67, 5, width - 7, 25), "Close", *this, [](Form& f)->void { PostQuitMessage(0); });
	push(closeButton);

	Theme::setFormColor(*this);
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
	//onPaint(e);
	return e;
}

int MainWindow::onDefault(const core::eventInfo &e) {
	switch (e.msg) {
	case WM_LBUTTONDBLCLK: if (LOWORD(e.lP) < 25 && HIWORD(e.lP) < 25)PostQuitMessage(0); return e;
	case WM_NCHITTEST: return onNcHitTest(e);
	}
	return e;
}

int MainWindow::onLButtonDown(const core::eventInfo &e) {
	Form::onLButtonDown(e);
	if (LOWORD(e.lP)<700) 
		SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	return 0;
}

void MainWindow::onEndPaint(const core::eventInfo& e) {
	core::Renderer::fillRect(core::vec4i(1, 1, 300, 30), core::vec4b(45, 45, 48, 255), img);
	core::Renderer::print("Core Renderer", img, 12, 12);
	core::Renderer::drawRect(getClientRect(), core::vec4b(0, 122, 204, 255), *this);
	core::Renderer::drawRect(rwnd.getChildRect().expand(1), core::Color(31, 31, 31, 255), *this);
}

int MainWindow::onPaint(const core::eventInfo& e) {
	Form::onPaint(e);
	//GL::swapBuffers(rwnd);
	return 0;
}

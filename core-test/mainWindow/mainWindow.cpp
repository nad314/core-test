#include <main>

void MainWindow::onOpening() {
	setStyle( WS_OVERLAPPED | WS_SYSMENU | WS_CLIPCHILDREN );
	setTitle("Core Renderer");
	setClass("CoreRenderer");
}

void MainWindow::onOpened() {
	rwnd.setParent(this);
	rwnd.open();
	hStatusBar = CreateStatusWindowA(WS_VISIBLE | WS_CHILD, "Status Bar", *this, 0);

	GL::createContext(rwnd);
	GL::init(rwnd);
	GL::setVsync(0);

}

void MainWindow::onClosing() {
	CloseWindow(hStatusBar);
	rwnd.close();
}

int MainWindow::onResize(const core::eventInfo &e) {
	RECT r, sr;
	if (!GetClientRect(hWnd, &r) )
		return e;
	rwnd.move(r.right-r.left, r.bottom-r.top-20);
	MoveWindow(hStatusBar, 0, r.bottom-20, r.right-r.left, 20, true);
	CoreTest* ct = static_cast<CoreTest*>(mp);
	if (ct) ct->onResize();
	return e;
}

void MainWindow::setStatusbarText(const char* text) {
	SetWindowTextA(hStatusBar, text);
}


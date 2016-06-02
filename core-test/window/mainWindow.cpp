#include <main>

void mainWindow::onOpening() {
	setStyle( WS_OVERLAPPED | WS_SYSMENU | WS_CLIPCHILDREN );
	setTitle("Core Renderer");
	setClass("CoreRenderer");
}

void mainWindow::onOpened() {
	rwnd.setParent(this);
	rwnd.open();
	hStatusBar = CreateStatusWindowA(WS_VISIBLE | WS_CHILD | SBARS_SIZEGRIP, "Status Bar", *this, 0);

	coreTest* ct = static_cast<coreTest*>(mp);
	if (ct) {
		core::glDevice& gl = ct->gl;
		gl.createContext(rwnd);
		gl.init(rwnd);
		gl.setVsync(0);
	}
}

void mainWindow::onClosing() {
	CloseWindow(hStatusBar);
	rwnd.close();
}

int mainWindow::onResize(const core::eventInfo &e) {
	RECT r, sr;
	if (!GetClientRect(hWnd, &r) )
		return e;
	rwnd.move(r.right-r.left, r.bottom-r.top-20);
	MoveWindow(hStatusBar, 0, r.bottom-20, r.right-r.left, 20, true);
	coreTest* ct = static_cast<coreTest*>(mp);
	if (ct) ct->onResize();
	return e;
}

void mainWindow::setStatusbarText(const char* text) {
	SetWindowTextA(hStatusBar, text);
}


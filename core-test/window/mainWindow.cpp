#include <main>

void mainWindow::onOpening() {
	setStyle(WS_OVERLAPPED | WS_SYSMENU | WS_CLIPCHILDREN);
	setTitle("Core Renderer");
	setClass("CoreRenderer");
}

void mainWindow::onOpened() {
	rwnd.setParent(this);
	rwnd.open(inputProcChild);
	hStatusBar = CreateStatusWindow(WS_CHILD|WS_VISIBLE, "Status Bar", hWnd, 1);
}

void mainWindow::onClosing() {
	CloseWindow(hStatusBar);
	rwnd.close();
}

void mainWindow::onClosed() {

}

void mainWindow::onResize() {
	RECT r, sr;
	if (!GetClientRect(hWnd, &r) )
		return;
	printf("%d, %d\n", r.right-r.left, r.bottom - r.top);
	rwnd.move(r.right-r.left, r.bottom-r.top);
}

void mainWindow::setStatusbarText(const char* text) {
	SetWindowTextA(hStatusBar, text);
}


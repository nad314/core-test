#include <main>

LRESULT CALLBACK inputProc(HWND hwnd, UINT msg, WPARAM wP, LPARAM lP) {
	if (msg == WM_CLOSE) {
		PostQuitMessage(0);
		return 0;
	}
	coreTest* ptr = static_cast<coreTest*>(reinterpret_cast<void*>(GetWindowLongA(hwnd, GWL_USERDATA)));
	if (!ptr)
		return DefWindowProcW(hwnd, msg, wP, lP);
	mainWindow &wnd = ptr->getWnd();
	switch (msg) {
	case WM_SIZE: wnd.width = LOWORD(lP); wnd.height = HIWORD(lP); ptr->onResize(); break;
	}
	return DefWindowProcW(hwnd, msg, wP, lP);
}

LRESULT CALLBACK inputProcChild(HWND hwnd, UINT msg, WPARAM wP, LPARAM lP) {
	switch (msg) {
	case WM_CLOSE: PostQuitMessage(0); return 0;
	}
	return DefWindowProcW(hwnd, msg, wP, lP);
}


void coreTest::onResize() {
	wnd.onResize();
	renderWindow& rwnd = static_cast<renderWindow&>(wnd.getRenderWindow());
	if (!rwnd)
		return;
	img.make(rwnd.getX(), rwnd.getY(), 32);
	view.make(rwnd.getX(), rwnd.getY());
	view.home();
}

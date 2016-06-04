#include <main>

void MainWindow::onOpening() {
	Form::onOpening();
	setStyle( WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_CLIPCHILDREN );
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



#include <main>

void MainWindow::onOpening() {
	Form::onOpening();
	setStyle( WS_POPUP );
	setTitle("Core Renderer");
	setClass("CoreRenderer");
}

void MainWindow::onOpened() {
	WindowForm::onOpened();

	rwnd.setParent(this);
	rwnd.open();

	GL::createContext(rwnd);
	GL::init(rwnd);
	GL::setVsync(0);
	Reshape();

	Theme::setFormColor(*this);
}

void MainWindow::onClosing() {
	WindowForm::onClosing();
	rwnd.close();
}

int MainWindow::onResize(const core::eventInfo &e) {
	WindowForm::onResize(e);
	if (width < 1 || height < 1)
		return e;
	core::vec4i r = getClientRect();
	rwnd.move(r.z-r.x, r.w-r.y);
	//MoveWindow(hStatusBar, 0, r.bottom-20, r.right-r.left, 20, true);
	//onPaint(e);
	return e;
}

void MainWindow::onEndPaint(const core::eventInfo& e) {
	core::Renderer::drawRect(getClientRect(), active?core::vec4b(0, 122, 204, 255): core::vec4b(84, 84, 84, 255), *this);
	core::Renderer::drawRect(rwnd.getChildRect().expand(1), core::Color(31, 31, 31, 255), *this);
}

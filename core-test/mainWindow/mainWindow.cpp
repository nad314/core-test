#include <main>

void MainWindow::onOpening() {
	WindowForm::onOpening();
	setTitle("Core Renderer");
	setClass("CoreRenderer");
	setSize(640, 480);
}

void MainWindow::onOpened() {
	WindowForm::onOpened();

	rwnd.setParent(this);
	rwnd.open();
	core::vec4i r = getClientRect();
	rwnd.move(r.z - r.x, r.w - r.y);
	//Reshape();
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
	return e;
}

void MainWindow::onEndPaint(const core::eventInfo& e) {
	WindowForm::onEndPaint(e);
	core::Renderer::drawRect(rwnd.getChildRect().expand(1), core::Color(31, 31, 31, 255), *this);
}

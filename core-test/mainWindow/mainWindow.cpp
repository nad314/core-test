#include <main>

void MainWindow::onOpening() {
	WindowForm::onOpening();
	setTitle("Core Renderer");
	setSize(1280, 720);
	addExStyle(WS_EX_ACCEPTFILES);
}

void MainWindow::onOpened() {
	WindowForm::onOpened();

	rwnd.setParent(this);
	rwnd.open();
	core::vec4i r = getClientRect();
	rwnd.move(r.z - r.x, r.w - r.y);
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

int MainWindow::onDropFiles(const core::eventInfo& e) {
	char path[256];
	if (DragQueryFileA((HDROP)e.wP, 0, path, 256)) {
		DragFinish((HDROP)e.wP);
		Storage::get().load(path);
		rwnd.view.home();
		Controller& c = Controller::get();
		for (int i = 0; i < c.threads; ++i)
			c.thread[i].push(new core::msRenderTask(&c.storage->pbvh, &rwnd.view, c.samples));
		c.invalidate();
	}
	return e;
}

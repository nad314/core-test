#include <main>

void coreTest::onResize() {
	renderWindow& rwnd = static_cast<renderWindow&>(wnd.getRenderWindow());
	if (!rwnd)
		return;
	view.make(rwnd.getX(), rwnd.getY());
	view.home();
	gl.init(rwnd);
}

#include <main>

int coreTest::onLoad() {
	InitCommonControls();
	if (!wnd.goToHomeDirectory())
		return 1;
	mesh.importgdev("data/wyvern-low.gdev");
	mesh.normalize();
	if (font.load("C:\\Windows\\Fonts\\segoeui.ttf", 10))
		return 1;
	font.setColor(core::vector4<byte>(255, 255, 255, 255));
	wnd.setModule(this);
	return 0;
}

int coreTest::onDispose() {
	mesh.free();
	return 0;
}

int coreTest::onStart() {
	wnd.open();
	wnd.onResize(core::eventInfo(NULL, 0, NULL, NULL));
	return 0;
}

int coreTest::onStop() {
	gl::deleteContext(wnd);
	wnd.close();
	return 0;
}

void coreTest::onResize() {
	renderWindow& rwnd = static_cast<renderWindow&>(wnd.getRenderWindow());
	if (!rwnd)
		return;
	view.make(rwnd.getX(), rwnd.getY());
	view.home();
	gl::init(rwnd);
}

int coreTest::main() {
	char text[256];
	globalTimer.start();
	int nframes(0);
	float renderTime(0);

	memset(text, 0, 256);

	while (!done) {
		if (wnd.peekMessage(done) || wnd.getRenderWindow().peekMessage())
			continue;

		view.rotation.init().rotate(globalTimer.update()*0.05f, 0.0f, 1.0f, 0.0f);
		view.updateMatrix();
		view.clear();

		timer.start();
		rend.drawPoints(mesh, &view);
		timer.stop();

		renderTime += timer;
		++nframes;
		sprintf(text, "%.3fms avg, %.3fms cur", renderTime / nframes, timer.ms());
		gl::drawImageInverted(view.img);
		gl::swapBuffers(wnd.getRenderWindow());
		wnd.setStatusbarText(text);
	}
	return 0;
}



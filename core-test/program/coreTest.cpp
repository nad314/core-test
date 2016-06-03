#include <main>

int CoreTest::onLoad() {
	printf("CoreTest::onLoad()\n");
	InitCommonControls();
	if (!wnd.goToHomeDirectory())
		return 1;
	mesh.importgdev("data/wyvern-low.gdev");
	mesh.normalize();
	wnd.setModule(this);
	return 0;
}

int CoreTest::onDispose() {
	mesh.dispose();
	return 0;
}

int CoreTest::onStart() {
	wnd.open();
	wnd.onResize(core::eventInfo(NULL, 0, NULL, NULL));
	return 0;
}

int CoreTest::onStop() {
	GL::deleteContext(wnd);
	wnd.close();
	return 0;
}

void CoreTest::onResize() {
	RenderWindow& rwnd = static_cast<RenderWindow&>(wnd.getRenderWindow());
	if (!rwnd)
		return;
	view.make(rwnd.getX(), rwnd.getY());
	view.home();
	GL::init(rwnd);
}

int CoreTest::main() {
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
		core::Renderer::drawPoints(mesh, &view);
		timer.stop();

		renderTime += timer;
		++nframes;
		sprintf(text, "%.3fms avg, %.3fms cur", renderTime / nframes, timer.ms());
		core::Font::get().print("Hello World", view.img, 20, 20);
		GL::drawImageInverted(view.img);
		GL::swapBuffers(wnd.getRenderWindow());
		wnd.setStatusbarText(text);
	}
	return 0;
}



#include <main>

int CoreTest::onLoad() {
	if (!wnd.goToHomeDirectory())
		return 1;
	mesh.importgdev("data/wyvern-low.gdev");
	mesh.normalize();
	return 0;
}

int CoreTest::onDispose() {
	mesh.dispose();
	return 0;
}

int CoreTest::onStart() {
	wnd.open();
	GL::setVsync(0);
	return 0;
}

int CoreTest::onStop() {
	wnd.close();
	return 0;
}

int CoreTest::main() {
	char text[256];
	bool done = 0;
	globalTimer.start();
	int nframes(0);
	float renderTime(0);

	memset(text, 0, 256);

	RenderWindow& rw = static_cast<RenderWindow&>(wnd.getRenderWindow());
	if (!rw)done = 1;

	while (!done) {
		if (wnd.peekMessage(done) || wnd.getRenderWindow().peekMessage())
			continue;

		if (GetAsyncKeyState(VK_ESCAPE))
			done = true;
		
		rw.view.rotation.init().rotate(globalTimer.update()*0.05f, 0.0f, 1.0f, 0.0f);
		rw.view.updateMatrix();
		rw.view.clear();

		timer.start();
		core::Renderer::drawPoints(mesh, &rw.view);
		timer.stop();

		renderTime += timer;
		++nframes;
		sprintf(text, "%.3fms avg, %.3fms cur", renderTime / nframes, timer.ms());
		core::Renderer::print(text, rw, 10, rw.height - 10 - core::Font::get().height());
		

		GL::drawImageInverted(rw);
		GL::swapBuffers(rw);
	}
	return 0;
}



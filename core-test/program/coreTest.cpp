#include <main>


int coreTest::onLoad() {
	InitCommonControls();
	if (!wnd.goToHomeDirectory())
		return 1;
	mesh.importgdev("data/wyvern-low.gdev");
	mesh.normalize();
	img.make(wnd.getX(), wnd.getY(), 32);
	if (font.load("C:\\Windows\\Fonts\\segoeuib.ttf", 9.6))
		return 1;
	font.setColor(core::vector4<byte>(255, 255, 255, 255));
	return 0;
}

int coreTest::onDispose() {
	img.free();
	mesh.free();
	return 0;
}

int coreTest::onStart() {
	wnd.open(inputProc);
	SetWindowLongA(wnd.hWnd, GWL_USERDATA, (LONG)this);

	renderWindow& rwnd = static_cast<renderWindow&>(wnd.getRenderWindow());
	if (rwnd == NULL)
		return 1;

	gl.createContext(rwnd);
	gl.init(rwnd);
	gl.setVsync(0);
	onResize();
	return 0;
}

int coreTest::onStop() {
	gl.deleteContext(wnd);
	wnd.close();
	return 0;
}

int coreTest::main() {
	char text[256];
	core::timer<float> timer;
	core::timer<float> globalTimer;
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
		timer.start();
		font.render(text, view.img, 10, 10);
		timer.stop();
		gl.drawImage(view.img);
		gl.swapBuffers(wnd.getRenderWindow());

		sprintf(text, "%.3fms font render", timer.ms());
		wnd.setStatusbarText(text);
	}
	return 0;
}



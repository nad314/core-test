#include <main>


int coreTest::onLoad() {
	InitCommonControls();
	theme::initStyles();
	if (!wnd.goToHomeDirectory())
		return 1;
	mesh.importgdev("data/wyvern-low.gdev");
	mesh.normalize();
	img.make(wnd.getX(), wnd.getY(), 32);
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
	theme::setWindowStyle(wnd);
	
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
	core::timer<int> timer;
	core::timer<float> globalTimer;
	globalTimer.start();
	int nframes(0);
	int renderTime(0);

	memset(text, 0, 256);

	while (!done) {
		if (wnd.peekMessage(done)||wnd.getRenderWindow().peekMessage())
			continue;

		view.rotation.init().rotate(globalTimer.update()*0.05f, 0.0f, 1.0f, 0.0f);
		view.updateMatrix();
		view.clear();

		timer.start();
		rend.drawPoints(mesh, &view);
		timer.stop();

		renderTime += timer;
		++nframes;
		gl.drawImage(view.img);
		gl.swapBuffers(wnd.getRenderWindow());

		sprintf(text, "%dms avg, %dms cur", renderTime / nframes, timer.ms());
		wnd.setStatusbarText(text);
	}
	return 0;
}



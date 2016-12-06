#include <main>

int CoreTest::loadData() {
	/*
	cloud.loadObj("data/pantherUniformHigh.obj");
	cloud.normalize();
	cloud.saveRaw("data/pantherUniformHigh.cloud");
	*//*
	if (!cloud.loadRaw("data/testSample.cloud"))
		return 1;
	cloud.bbox(p, q);
	cloudTree.build(cloud);
	pbvh.build(cloudTree);
	pbvh.setRadius(pbvh.estimateRadius()*0.9f);
	pbvh.pointsPerBox();
	//cleanup
	cloudTree.dispose();
	//cloud.dispose();*/

	return storage->load("data/testSample.cloud");
	return 0;
}

int CoreTest::onLoad() {
	if (!wnd.goToHomeDirectory())
		return 1;
	//core::Debug::enable();
	return 0;
}

int CoreTest::onDispose() {
	return 0;
}

int CoreTest::onStart() {
	wnd.open();
	GL::setVsync(0);
	core::eventInfo e(wnd, WM_PAINT, wnd.width, wnd.height);
	wnd.onPaint(e);
	glClear(GL_COLOR_BUFFER_BIT);
	GL::swapBuffers(wnd.getRenderWindow());
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

	storage = new Storage;
	/*
	if (storage->load("data/testSample.cloud"))
		done = 1;
		*/
	controller = new Controller(&rw, storage);
	rw.attach(controller);

	glClear(GL_COLOR_BUFFER_BIT);
	GL::swapBuffers(rw);

	rw.view.rotation.init();
	rw.view.updateMatrix();

	while (!done) {
		if (wnd.peekMessageAsync(done))
			continue;

		if (controller->valid)
			continue;

		rw.view.clear();

		timer.start();
		//Render Multithreaded
		core::Renderer::invalidate();
		/*
		{
			std::lock_guard<std::mutex> lg(thread[0].mutex);
			core::Renderer::Worker::go = 0;
			core::Renderer::Worker::stop = 0;
		}
		core::Renderer::Worker::cv.notify_all();
		thread[threads - 1].render(pbvh, &rw.view);
		if (core::Renderer::Worker::stop < threads-1) {
			std::unique_lock<std::mutex> lk(thread[0].mutex);
			while (core::Renderer::Worker::stop < threads - 1)
				core::Renderer::Worker::cv.wait(lk);
		}
		*/
		controller->render();
		timer.stop();

		renderTime += timer;
		++nframes;
		sprintf(text, "[Points: %d] [Atom Radius: %.6f] [Avg: %.2fms (%.2f FPS)] [Cur: %.2fms (%.2f FPS)]", storage->cloud.points.count(), sqrt(storage->pbvh.radiusSquared), renderTime / nframes, (1000.0f*nframes)/renderTime, timer.ms(), 1000.0f/timer.ms());
		core::Renderer::print(text, rw, 10, rw.height - 10 - core::Font::get().height());
		core::Renderer::print("Left Click to Rotate", rw, 10, rw.height - 42 - 3 * core::Font::get().height());
		core::Renderer::print("F11 to reset camera", rw, 10, rw.height - 38 - 2 * core::Font::get().height());
		core::Renderer::print("F12 to render x16 MSAA", rw, 10, rw.height - 34 - core::Font::get().height());

		GL::drawImageInverted(rw);
		GL::swapBuffers(rw);
		controller->validate();
	}
	rw.detach();
	delete controller;
	delete storage;

	return 0;
}



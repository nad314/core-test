#include <main>

int CoreTest::loadData() {
	/*
	cloud.loadObj("data/subsampledLow2.obj");
	cloud.normalize();
	cloud.saveRaw("data/subsampledLow2.glc"); //GeoLogCloud...because I can 8D
	*/
	if (!cloud.loadRaw("data/testSample.cloud"))
		return 1;
	cloud.bbox(p, q);
	cloudTree.build(cloud);
	pbvh.build(cloudTree);
	pbvh.setRadius(pbvh.estimateRadius()*1.1f);
	pbvh.pointsPerBox();
	//cleanup
	cloudTree.dispose();
	cloud.dispose();
	return 0;
}

int CoreTest::onLoad() {
	if (!wnd.goToHomeDirectory())
		return 1;
	//core::Debug::enable();
	/*
	mesh.importgdev("data/panther.gdev");
	mesh.normalize();
	mesh.bbox(p, q);
	octree.build(mesh);
	bvh.build(octree);*/
	return 0;
}

int CoreTest::onDispose() {
	//in case I decide to comment the dispose in onLoad()
	cloudTree.dispose();
	cloud.dispose();
	mesh.dispose();
	octree.dispose();
	return 0;
}

int CoreTest::onStart() {
	wnd.open();
	GL::setVsync(0);
	core::eventInfo e(wnd, WM_PAINT, wnd.width, wnd.height);
	wnd.onPaint(e);
	glClear(GL_COLOR_BUFFER_BIT);
	GL::swapBuffers(wnd.getRenderWindow());
	if (loadData())
		return 1;
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

	controller = new Controller(&rw, &pbvh);
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
		sprintf(text, "[Points: %d] [Avg: %.2fms (%.2f FPS)] [Cur: %.2fms (%.2f FPS)]", mesh.vecs.count(), renderTime / nframes, (1000.0f*nframes)/renderTime, timer.ms(), 1000.0f/timer.ms());
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

	return 0;
}



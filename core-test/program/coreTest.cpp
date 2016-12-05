#include <main>

int CoreTest::onLoad() {
	if (!wnd.goToHomeDirectory())
		return 1;
	core::Debug::enable();
	/*
	mesh.importgdev("data/panther.gdev");
	mesh.normalize();
	mesh.bbox(p, q);
	octree.build(mesh);
	bvh.build(octree);*/
	/*
	cloud.loadObj("data/cleaned.obj");
	cloud.normalize();
	cloud.saveRaw("data/cleaned.glc"); //GeoLogCloud...because I can 8D
	*/
	if (!cloud.loadRaw("data/cleaned.glc"))
		return 1;
	cloud.bbox(p, q);
	cloudTree.build(cloud);
	pbvh.build(cloudTree);
	pbvh.setRadius(pbvh.estimateRadius()*1.2f);
	pbvh.pointsPerBox();
	//cleanup
	cloudTree.dispose();
	cloud.dispose();
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

	//const int threads = std::thread::hardware_concurrency();
	const int threads = 4;
	core::Renderer::Worker::go = threads;
	core::Renderer::Worker *thread = new core::Renderer::Worker[threads];
	for (int i = 0; i < threads-1; ++i)
		thread[i].create(pbvh, &rw.view, i, threads);
	thread[threads - 1].threadNumber = threads-1;
	thread[threads - 1].threadCount = threads;
	int step = 0;

	glClear(GL_COLOR_BUFFER_BIT);
	GL::swapBuffers(rw);

	

	while (!done) {
		if (wnd.peekMessageAsync(done))
			continue;

		rw.view.rotation.init().rotate(globalTimer.update()*0.0125f, 0.0f, 1.0f, 0.0f);
		rw.view.updateMatrix();
		rw.view.clear();

		timer.start();
		//Render Multithreaded
		core::Renderer::invalidate();
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
		timer.stop();

		renderTime += timer;
		++nframes;
		sprintf(text, "[Points: %d] [Avg: %.2fms (%.2f FPS)] [Cur: %.2fms (%.2f FPS)]", mesh.vecs.count(), renderTime / nframes, (1000.0f*nframes)/renderTime, timer.ms(), 1000.0f/timer.ms());
		core::Renderer::print(text, rw, 10, rw.height - 10 - core::Font::get().height());

		GL::drawImageInverted(rw);
		GL::swapBuffers(rw);
	}
	for (int i = 0; i < threads - 1; ++i)
		thread[i].join();
	delete[] thread;

	return 0;
}



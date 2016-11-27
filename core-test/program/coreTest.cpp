#include <main>

int CoreTest::onLoad() {
	if (!wnd.goToHomeDirectory())
		return 1;

	mesh.importgdev("data/panther.gdev");
	mesh.normalize();
	mesh.bbox(p, q);
	octree.build(mesh);
	bvh.build(octree);
	return 0;
}

int CoreTest::onDispose() {
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

	int threads = std::thread::hardware_concurrency();
	//const int threads = 2;
	core::Renderer::Worker::go = threads;
	core::Renderer::Worker *thread = new core::Renderer::Worker[threads];
	for (int i = 0; i < threads-1; ++i)
		thread[i].create(bvh, &rw.view, i, threads);
	thread[threads - 1].threadNumber = threads-1;
	thread[threads - 1].threadCount = threads;

	int step = 0;

	glClear(GL_COLOR_BUFFER_BIT);
	GL::swapBuffers(rw);

	

	while (!done) {
		if (wnd.peekMessageAsync(done))
			continue;
		/*
		if (GetAsyncKeyState(VK_ESCAPE))
			done = true;*/
		
		rw.view.rotation.init().rotate(globalTimer.update()*0.05f, 0.0f, 1.0f, 0.0f);
		rw.view.updateMatrix();
		rw.view.clear();

		timer.start();
		core::Renderer::invalidate();
		{
			std::lock_guard<std::mutex> lg(thread[0].mutex);
			core::Renderer::Worker::go = 0;
			core::Renderer::Worker::stop = 0;
		}
		core::Renderer::Worker::cv.notify_all();
		thread[threads - 1].render(bvh, &rw.view);
		if (core::Renderer::Worker::stop < threads-1) {
			//MessageBox(NULL, "", "", MB_OK);
			std::unique_lock<std::mutex> lk(thread[0].mutex);
			while (core::Renderer::Worker::stop < threads - 1)
				core::Renderer::Worker::cv.wait(lk);
			/*
			core::Renderer::Worker::cv.wait(lk, 
				[&] { return core::Renderer::Worker::go>=threads-1; });
				*/
		}
		//core::Renderer::raytrace(bvh, &rw.view);
		//core::Renderer::raytrace(octree, &rw.view);
		//core::Renderer::drawPointRange(mesh, &rw.view, (threads - 1)*step, mesh.vecs.count());
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



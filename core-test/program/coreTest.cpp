#include <main>

inline void printvec(const vec4s& v) {
	printf("%.2f %.2f %.2f %.2f\n", v.m.m128_f32[0], v.m.m128_f32[1], v.m.m128_f32[2], v.m.m128_f32[3]);
}

int CoreTest::onLoad() {
	if (!wnd.goToHomeDirectory())
		return 1;

	mesh.importgdev("data/panther.gdev");
	mesh.normalize();
	mesh.bbox(p, q);
	octree.build(mesh);

	/*
	const vec4s v0 = vec4(1.0f, 2.0f, 3.0f, 4.0f); printf("vec0 "); printvec(v0);
	const vec4s v1 = _mm_permute_ps(v0, 0b00110110); printf("vec1 "); printvec(v1); //3 2 4 1
	system("pause");
	return 1;
	*/
	
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

	/*
	int threads = std::thread::hardware_concurrency();
	std::thread *thread = new std::thread[threads];
	int step = mesh.vecs.count() / threads;
	
	for (int i = 0; i < threads - 1; ++i) {
		thread[i] = std::thread(core::Renderer::drawPointThread, mesh, &rw.view, i*step, (i + 1)*step, &done);
	}*/

	int threads = 1;
	int step = 0;

	glClear(GL_COLOR_BUFFER_BIT);
	GL::swapBuffers(rw);

	while (!done) {
		if (wnd.peekMessageAsync(done))
			continue;

		if (GetAsyncKeyState(VK_ESCAPE))
			done = true;
		
		rw.view.rotation.init().rotate(globalTimer.update()*0.05f, 0.0f, 1.0f, 0.0f);
		rw.view.updateMatrix();
		rw.view.clear();

		timer.start();
		core::Renderer::invalidate();
		core::Renderer::raytrace(octree, &rw.view);
		//core::Renderer::drawPointRange(mesh, &rw.view, (threads - 1)*step, mesh.vecs.count());
		timer.stop();

		renderTime += timer;
		++nframes;
		sprintf(text, "%d Points: %.3fms avg, %.3fms cur (%.2f hitsPerBox avg)", mesh.vecs.count(), renderTime / nframes, timer.ms());
		core::Renderer::print(text, rw, 10, rw.height - 10 - core::Font::get().height());
		
		GL::drawImageInverted(rw);
		GL::swapBuffers(rw);
	}

	/*
	for (int i = 0; i < threads - 1; ++i)
		thread[i].join();

	delete[] thread;
	*/

	return 0;
}



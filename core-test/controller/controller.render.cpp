#include <main>

void Controller::render() {
	for (int i = 0; i < threads; ++i)
		thread[i].push(new core::RenderTask(pbvh, view));
	core::Renderer::Worker::cv.notify_all();
	thread[threads - 1].execute();
	for (int i = 0; i < threads - 1; ++i)
		thread[i].wait();
}

#include <main>

Controller* core::Getter<Controller>::getter = NULL;

Controller::Controller(core::Window* p, core::PBVH* lpbvh) {
	set(*this);
	parent = p;
	pbvh = lpbvh;
	view = &(static_cast<RenderWindow*>(parent))->view;
	threads = 4;
	thread = new core::Renderer::Worker[threads];
	for (int i = 0; i < threads - 1; ++i)
		thread[i].create(*pbvh, view, i, threads);
	thread[threads - 1].threadNumber = threads - 1;
	thread[threads - 1].threadCount = threads;
}

Controller::~Controller() {
	for (int i = 0; i < threads - 1; ++i)
		thread[i].join();
	delete[] thread;
}

int Controller::onLButtonDown(const core::eventInfo& e) {
	dragging = 1;
	invalidate();
	SetCapture(*parent);
	return e;
}

int Controller::onLButtonUp(const core::eventInfo& e) {
	dragging = 0;
	invalidate();
	ReleaseCapture();
	return e;
}

int Controller::onMouseMove(const core::eventInfo& e) {
	if (dragging) {
		View& view = (static_cast<RenderWindow*>(parent))->view;
		matrixf mat;
		mat.rotate(0.2f*(e.y() - mouse.y), 1.0f, 0.0f, 0.0f);
		mat.rotate(0.2f*(e.x() - mouse.x), 0.0f, 1.0f, 0.0f);
		view.rotation = view.rotation*mat;
		view.updateMatrix();
		invalidate();
	}
	mouse = core::vec2i(e.x(), e.y());
	return e;
}

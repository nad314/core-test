#include <main>

Controller* core::Getter<Controller>::getter = NULL;

Controller::Controller(core::Window* p, core::PBVH* lpbvh) {
	set(*this);
	parent = p;
	pbvh = lpbvh;
	view = &(static_cast<RenderWindow*>(parent))->view;
	samples = 2;
	threads = std::thread::hardware_concurrency();
	//threads = 4;
	thread = new core::Renderer::Worker[threads];
	for (int i = 0; i < threads - 1; ++i)
		thread[i].create(*pbvh, view, i, threads);
	thread[threads - 1].threadNumber = threads - 1;
	thread[threads - 1].threadCount = threads;
	for (int i = 0; i < threads; ++i)
		thread[i].push(new core::msRenderTask(pbvh, view, samples));
	invalidate();
}

Controller::~Controller() {
	for (int i = 0; i < threads - 1; ++i)
		thread[i].join();
	delete[] thread;
}

int Controller::onLButtonDown(const core::eventInfo& e) {
	dragging = 1;
	getPoint(e.x(), e.y());
	for (int i = 0; i < threads; ++i)
		thread[i].push(new core::subRenderTask(pbvh, view));
	invalidate();
	SetCapture(*parent);
	return e;
}

int Controller::onLButtonUp(const core::eventInfo& e) {
	dragging = 0;
	for (int i = 0; i < threads; ++i)
		thread[i].push(new core::msRenderTask(pbvh, view, samples));
	invalidate();
	ReleaseCapture();
	return e;
}

int Controller::onMouseMove(const core::eventInfo& e) {
	if (dragging) {
		matrixf mat;
		clickPoint.w = 1.0f;
		vec4 point = view->rotation*clickPoint;
		mat.translate(point.x, point.y, point.z);
		mat.rotate(0.2f*(e.y() - mouse.y), 1.0f, 0.0f, 0.0f);
		mat.rotate(0.2f*(e.x() - mouse.x), 0.0f, 1.0f, 0.0f);
		mat.translate(-point.x, -point.y, -point.z);
		view->rotation = view->rotation*mat;
		view->updateMatrix();
		invalidate();
		for (int i = 0; i < threads; ++i)
			thread[i].push(new core::subRenderTask(pbvh, view));
	}
	mouse = core::vec2i(e.x(), e.y());
	return e;
}

int Controller::onKeyDown(const core::eventInfo& e) {
	if (e.wP == VK_F12) {
		for (int i = 0; i < threads; ++i)
			thread[i].push(new core::msRenderTask(pbvh, view, 4));
		invalidate();
	}
	else if (e.wP == VK_F11) {
		view->home();
		view->updateMatrix();
		for (int i = 0; i < threads; ++i)
			thread[i].push(new core::msRenderTask(pbvh, view, samples));
		invalidate();
	}
	return e;
}

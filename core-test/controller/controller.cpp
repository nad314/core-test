#include <main>

Controller* core::Getter<Controller>::getter = NULL;

Controller::Controller(core::Window* p, Storage* st) {
	set(*this);
	storage = st;
	parent = p;
	view = &(static_cast<RenderWindow*>(parent))->view;
	samples = 2;
	threads = std::thread::hardware_concurrency();
	//threads = 4;
	thread = new core::Renderer::Worker[threads];
	for (int i = 0; i < threads - 1; ++i)
		thread[i].create(storage->pbvh, view, i, threads);
	thread[threads - 1].threadNumber = threads - 1;
	thread[threads - 1].threadCount = threads;
	for (int i = 0; i < threads; ++i)
		thread[i].push(new core::msRenderTask(&storage->pbvh, view, samples));
	invalidate();
}

Controller::~Controller() {
	for (int i = 0; i < threads - 1; ++i)
		thread[i].join();
	delete[] thread;
}

int Controller::onLButtonDown(const core::eventInfo& e) {
	if (storage->pbvh.pointCount < 1)
		return e;
	dragging = 1;
	getPoint(e.x(), e.y());
	for (int i = 0; i < threads; ++i)
		thread[i].push(new core::subRenderTask(&storage->pbvh, view));
	invalidate();
	SetCapture(*parent);
	return e;
}

int Controller::onLButtonUp(const core::eventInfo& e) {
	if (storage->pbvh.pointCount < 1)
		return e;
	dragging = 0;
	for (int i = 0; i < threads; ++i)
		thread[i].push(new core::msRenderTask(&storage->pbvh, view, samples));
	invalidate();
	ReleaseCapture();
	return e;
}

int Controller::onMouseMove(const core::eventInfo& e) {
	if (storage->pbvh.pointCount < 1)
		return e;
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
			thread[i].push(new core::subRenderTask(&storage->pbvh, view));
	}
	mouse = core::vec2i(e.x(), e.y());
	return e;
}

int Controller::onKeyDown(const core::eventInfo& e) {
	switch (e.wP) {
	case VK_F12: {
		for (int i = 0; i < threads; ++i)
			thread[i].push(new core::msRenderTask(&storage->pbvh, view, 4));
		invalidate();
		break;
	}
	case VK_F11: {
		view->home();
		view->updateMatrix();
		for (int i = 0; i < threads; ++i)
			thread[i].push(new core::msRenderTask(&storage->pbvh, view, samples));
		invalidate();
		break;
	}
	case '1': {
		storage->pbvh.setRadius(sqrt(storage->pbvh.radiusSquared) * 1.05f);
		for (int i = 0; i < threads; ++i)
			thread[i].push(new core::msRenderTask(&storage->pbvh, view, samples));
		invalidate();
		break;
	}
	case '2': {
		storage->pbvh.setRadius(sqrt(storage->pbvh.radiusSquared) / 1.05f);
		for (int i = 0; i < threads; ++i)
			thread[i].push(new core::msRenderTask(&storage->pbvh, view, samples));
		invalidate();
		break;
	}
	default: break;
	}
	return e;
}

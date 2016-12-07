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
	if (storage->pbvh.pointCount < 1 || dragging)
		return e;
	rotating = 1;
	getPoint(e.x(), e.y());
	mouse.x = e.x();
	mouse.y = e.y();
	for (int i = 0; i < threads; ++i)
		thread[i].push(new core::subRenderTask(&storage->pbvh, view));
	invalidate();
	SetCapture(*parent);
	return e;
}

int Controller::onLButtonUp(const core::eventInfo& e) {
	if (storage->pbvh.pointCount < 1)
		return e;
	rotating = 0;
	for (int i = 0; i < threads; ++i)
		thread[i].push(new core::msRenderTask(&storage->pbvh, view, samples));
	invalidate();
	ReleaseCapture();
	return e;
}

int Controller::onRButtonDown(const core::eventInfo& e) {
	if (storage->pbvh.pointCount < 1 || rotating)
		return e;
	dragging = 1;
	getPoint(e.x(), e.y());
	mouse.x = e.x();
	mouse.y = e.y();
	for (int i = 0; i < threads; ++i)
		thread[i].push(new core::subRenderTask(&storage->pbvh, view));
	invalidate();
	SetCapture(*parent);
	return e;
}

int Controller::onRButtonUp(const core::eventInfo& e) {
	if (storage->pbvh.pointCount < 1)
		return e;
	dragging = 0;
	for (int i = 0; i < threads; ++i)
		thread[i].push(new core::msRenderTask(&storage->pbvh, view, samples));
	invalidate();
	ReleaseCapture();
	return e;
}

int Controller::onMousewheel(const core::eventInfo& e) {
	if (storage->pbvh.pointCount < 1)
		return e;
	if (e.delta() < 0) {
		if (view->fov > 41.5f)
			view->fov += 2.0f;
		else view->fov *= 1.2f;
	}
	else if (view->fov > 41.5f)
		view->fov -= 2.0f;
	else view->fov /= 1.2f;
	view->updateMatrix();
	for (int i = 0; i < threads; ++i)
		thread[i].push(new core::subRenderTask(&storage->pbvh, view));
	invalidate();
	return e;
}

int Controller::onMouseMove(const core::eventInfo& e) {
	if (storage->pbvh.pointCount < 1)
		return e;
	if (rotating) {
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
	else if (dragging) {
		vec4 d;
		//clickPoint.w = 1.0f;
		matrixf rot = view->rotation;
		rot[12] = rot[13] = rot[14] = 0.0f;
		rot.invert();
		vec4 normal = rot*vec4(0.0f, 0.0f, 1.0f, 1.0f);
		vec4 point = clickPoint;
		normal.w = -core::Math::dot3(normal, point);

		core::Ray ray = getRay((float)e.x(), (float)e.y());
		core::Ray lray = getRay((float)mouse.x, (float)mouse.y);
		vec4s t0 = core::SSE::rayPlaneT(ray.sr0, ray.sr1, vec4s(normal));
		vec4s t1 = core::SSE::rayPlaneT(lray.sr0, lray.sr1, vec4s(normal));
		vec4s delta = (ray.sr0 + ray.sr1*t0) - (lray.sr0 + lray.sr1*t1);


		delta.store(d);
		matrixf mat;
		mat.init();
		mat.translate(d.x, d.y, d.z);
		view->rotation = mat*view->rotation;
		view->updateMatrix();
		for (int i = 0; i < threads; ++i)
			thread[i].push(new core::subRenderTask(&storage->pbvh, view));
		invalidate();
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
	case 'S': {
		if (!GetAsyncKeyState(VK_CONTROL))
			break;
		std::string path = core::Path::getSaveFileName("Cloud\0*.cloud\0\0");
		if (path != "") {
			storage->cloud.saveAtomic(path.c_str(), sqrt(storage->pbvh.radiusSquared));
		}
	}
	case 'O': {
		if (!GetAsyncKeyState(VK_CONTROL))
			break;
		std::string path = core::Path::getOpenFileName("Cloud\0*.cloud\0OBJ\0*.obj\0\0");
		if (path != "") {
			storage->load(path.c_str());
		}
	}
	default: break;
	}
	return e;
}

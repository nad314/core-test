#include <main>

Controller* core::Getter<Controller>::getter = NULL;

Controller::Controller(core::Window* p, Storage* st) {
	set(*this);
	storage = st;
	parent = p;
	view = &(static_cast<RenderWindow*>(parent))->view;
	samples = 2;
	wg = new core::Renderer::WorkerGroup;
}

Controller::~Controller() {
	delete wg;
}

int Controller::onLButtonDown(const core::eventInfo& e) {
	if (storage->pbvh.pointCount < 1 || dragging || benchMode)
		return e;
	rotating = 1;
	getPoint(e.x(), e.y());
	mouse.x = e.x();
	mouse.y = e.y();
	wg->pushTask<core::subRenderTask>(&storage->pbvh, view);
	invalidate();
	SetCapture(*parent);
	return e;
}

int Controller::onLButtonUp(const core::eventInfo& e) {
	if (storage->pbvh.pointCount < 1 || benchMode)
		return e;
	rotating = 0; 
	wg->pushTask<core::msRenderTask>(&storage->pbvh, view, samples);
	invalidate();
	ReleaseCapture();
	return e;
}

int Controller::onRButtonDown(const core::eventInfo& e) {
	if (storage->pbvh.pointCount < 1 || rotating || benchMode)
		return e;
	dragging = 1;
	getPoint(e.x(), e.y());
	mouse.x = e.x();
	mouse.y = e.y();
	wg->pushTask<core::subRenderTask>(&storage->pbvh, view);
	invalidate();
	SetCapture(*parent);
	return e;
}

int Controller::onRButtonUp(const core::eventInfo& e) {
	if (storage->pbvh.pointCount < 1 || benchMode)
		return e;
	dragging = 0;
	wg->pushTask<core::msRenderTask>(&storage->pbvh, view, samples);
	invalidate();
	ReleaseCapture();
	return e;
}

int Controller::onMousewheel(const core::eventInfo& e) {
	if (storage->pbvh.pointCount < 1 || benchMode)
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
	wg->pushTask<core::subRenderTask>(&storage->pbvh, view);
	invalidate();
	return e;
}

int Controller::onMouseMove(const core::eventInfo& e) {
	if (storage->pbvh.pointCount < 1 || benchMode)
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
		wg->pushTask<core::subRenderTask>(&storage->pbvh, view);
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
		wg->pushTask<core::subRenderTask>(&storage->pbvh, view);
		invalidate();
	}
	mouse = core::vec2i(e.x(), e.y());
	return e;
}

int Controller::onKeyDown(const core::eventInfo& e) {
	if (benchMode)
		return e;
	switch (e.wP) {
	case VK_F12: {
		bool done = false;
		view->clear();
		core::renderShowTask task(view, 16);
		core::RenderShader shader(*view);
		core::VolumetricShader vs(*view, 64);
		wg->clearTasks().pushTask<core::imageRenderTask>(&storage->pbvh, view, 8, &vs);
		std::thread t0 = std::thread(&core::Renderer::WorkerGroup::executeAsync, wg, &done);
		t0.detach();
		while (true) {
			std::unique_lock<std::mutex> lk(task.mutex);
			if (done == true)
				break;
			task.cv.wait(lk);
			GL::drawImageInverted(task.img);
			GL::swapBuffers(*parent);
		}
		if (t0.joinable())
			t0.join();
		GL::drawImageInverted(view->img);
		GL::swapBuffers(*parent);

		core::Path::pushDir();
		core::Path::goHome();
		view->saveTransform("data/view.transform");
		core::Path::popDir();
		core::Image img = view->img;
		img.flipV();
		/*
		std::string path = core::Path::getSaveFileName("PNG\0*.png\0\0");
		if (path != "") {
			path = core::Path::pushExt("png", path);
			img.savePng(path.c_str());
		}
		*/
		break;
	}
	case VK_F11: {
		view->home();
		view->updateMatrix();
		wg->clearTasks().pushTask<core::msRenderTask>(&storage->pbvh, view, samples);
		invalidate();
		break;
	}
	case VK_F8: {
		view->mode = view->mode == 1 ? 0 : 1;
		view->updateMatrix();
		wg->clearTasks().pushTask<core::msRenderTask>(&storage->pbvh, view, samples);
		invalidate();
	}

	case '1': {
		storage->pbvh.setRadius(sqrt(storage->pbvh.radiusSquared) * 1.05f);
		wg->pushTask<core::msRenderTask>(&storage->pbvh, view, samples);
		invalidate();
		break;
	}
	case '2': {
		storage->pbvh.setRadius(sqrt(storage->pbvh.radiusSquared) / 1.05f);
		wg->pushTask<core::msRenderTask>(&storage->pbvh, view, samples);
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

#include <main>
namespace core {

	void Form::onOpened() {
		Window::onOpened();
		img.make(width, height, 32);
		GL::pushCurrent();
		GL::createContext(*this);
		GL::setVsync(0);
		GL::popCurrent();
		backColor = vec4b(45, 45, 48, 255);
		items.clear();
	}

	void Form::onClosing() {
		items.clear();
		img.dispose();
	}

	int Form::onResize(const eventInfo& e) {
		Window::onResize(e);
		if (width>0&&height>0)
			img.make(width, height, 32);
		img.clear();
		__invalidate();
		for (auto& i : items)
			i->onResize(e);
		return 0;
	}

	int Form::onPaint(const eventInfo& e) {
		Window::onPaint(e);
		onStartPaint(e);
		onControlPaint(e);
		onEndPaint(e);
		GL::pushCurrent();
		GL::makeCurrent(*this);
		GL::init(*this);
		GL::drawImageInverted(img);
		GL::swapBuffers(*this);
		GL::popCurrent();
		return 0;
	}

	int Form::onMouseMove(const eventInfo& e) {
		for (auto& i : items)
			if (i->onMouseMove(e))
				return e;
		return e;
	}

	int Form::onLButtonDown(const eventInfo& e) {
		for (auto& i : items)
			if (i->onLButtonDown(e))
				return e;
		return e;
	}

	int Form::onLButtonUp(const eventInfo& e) {
		for (auto& i : items)
			if (i->onLButtonUp(e))
				return e;
		return e;
	}

	void Form::onStartPaint(const eventInfo& e) {
		if (!valid()) {
			Renderer::clearImage(img, backColor);
		}
	}

	void Form::onControlPaint(const eventInfo& e) {
		for (auto& i : items)
			if (!(i->flags&1)||!valid())
				i->onPaint(e);
	}

	void Form::onEndPaint(const eventInfo& e) {
		validate();
	}

	void Form::setFormTitle(const char* t) {
		setTitle(t);
	}

}

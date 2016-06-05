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
		invalidate();
		return 0;
	}

	int Form::onPaint(const eventInfo& e) {
		Window::onPaint(e);
		GL::pushCurrent();
		GL::makeCurrent(*this);
		GL::init(*this);
		onStartPaint(e);
		onControlPaint(e);
		onEndPaint(e);
		GL::drawImageInverted(img);
		GL::swapBuffers(*this);
		GL::popCurrent();
		return 0;
	}

	void Form::onStartPaint(const eventInfo& e) {
		if (!valid()) {
			validate();
			Renderer::clearImage(img, backColor);
		}
	}

	void Form::onControlPaint(const eventInfo& e) {
		for (auto& i : items)
			i->onPaint(e);
	}

	void Form::onEndPaint(const eventInfo& e) {
	}

}

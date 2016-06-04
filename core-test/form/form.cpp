#include <main>
namespace core {

	void Form::onOpened() {
		Window::onOpened();
		img.make(width, height, 32);
		GL::pushCurrent();
		GL::createContext(*this);
		GL::popCurrent();
		backColor = vec4b(45, 45, 48, 255);
	}

	void Form::onClosing() {
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
		onStartPaint();
		onControlPaint();
		onEndPaint();
		GL::drawImageInverted(img);
		GL::swapBuffers(*this);
		GL::popCurrent();
		return 0;
	}

	void Form::onStartPaint() {
		if (!valid()) {
			validate();
			Renderer::clearImage(img, backColor);
		}
	}

	void Form::onControlPaint() {
	}

	void Form::onEndPaint() {
	}

}

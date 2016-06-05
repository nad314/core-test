#include <main>

namespace core {
	int Button::onMouseMove(const eventInfo& e) { return 0; }
	int Button::onLeftButtonDown(const eventInfo& e) { return 0; }
	int Button::onLeftButtonUp(const eventInfo& e) { return 0; }
	int Button::onRightButtonDown(const eventInfo& e) { return 0; }
	int Button::onRightButtonUp(const eventInfo& e) { return 0; }
	int Button::onPaint(const eventInfo& e) { 
		if (form == NULL) return 1;
		Renderer::fillRect(rect, backColor, form->image());
		Renderer::print(text.c_str(), form->image(), rect.x + 2, rect.y + 2);
		return 0; 
	}
}


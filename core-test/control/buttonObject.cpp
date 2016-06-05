#include <main>

namespace core {
	int ButtonObject::onMouseMove(const eventInfo& e) {
		int f = flags;
		int x = LOWORD(e.lP);
		int y = HIWORD(e.lP);
		if (x > rect.x && x<rect.z && y>rect.y && y < rect.w)
			__hover();
		else __unhover();
		if (f != flags) {
			invalidate();
			return true;
		}
		return false;
	}

	int ButtonObject::onLButtonDown(const eventInfo& e) { if (flags & 2) { __activate(); invalidate(); } return 0; }
	int ButtonObject::onLButtonUp(const eventInfo& e) { if (flags & 2 && onClick)onClick(*form); if (flags & 2 || flags & 4)invalidate();  __deactivate(); return 0; }
	int ButtonObject::onRightButtonDown(const eventInfo& e) { return 0; }
	int ButtonObject::onRightButtonUp(const eventInfo& e) { return 0; }
	int ButtonObject::onPaint(const eventInfo& e) {
		if (form == NULL) return 1;
		__validate();
		Renderer::fillRect(rect, (flags & 4) ? backColorActivated : ((flags & 2) ? backColorHover : backColor), form->image());
		Renderer::drawRect(rect, borderColor, form->image());
		return 0;
	}
}


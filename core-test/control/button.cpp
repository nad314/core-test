#include <main>

namespace core {

	void Button::make(const vec4i& r, const std::string t, Form& f) {
		rect = r;
		text = t;
		form = &f;
		flags = 0;
	}

	void Button::make(const vec4i& r, const std::string t, Form& f, buttonFunc func) {
		make(r, t, f);
		onClick = func;
	}


	int Button::onMouseMove(const eventInfo& e) { 
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
	int Button::onLButtonDown(const eventInfo& e) { if (flags & 2) { if (onClick)onClick();  __activate(); invalidate(); } return 0; }
	int Button::onLButtonUp(const eventInfo& e) { __deactivate(); if(flags&2)invalidate(); return 0; }
	int Button::onRightButtonDown(const eventInfo& e) { return 0; }
	int Button::onRightButtonUp(const eventInfo& e) { return 0; }
	int Button::onPaint(const eventInfo& e) { 
		if (form == NULL) return 1;
		Font& f = Font::get();
		Renderer::fillRect(rect, (flags&4)?backColorActivated:((flags&2)?backColorHover:backColor), form->image());
		Renderer::drawRect(rect, borderColor, form->image());
		Renderer::print(text.c_str(), form->image(), (rect.x + rect.z)/2 - f.width(text.c_str())/2, (rect.y + rect.w)/2 - f.height()/2);
		__validate();
		return 0; 
	}
}


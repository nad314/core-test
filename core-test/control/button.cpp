#include <main>

namespace core {

	void Button::make(const vec4i& r, const std::string t, Form& f) {
		rect = r;
		setText(t);
		form = &f;
		flags = 0;
	}

	void Button::make(const vec4i& r, const std::string t, Form& f, buttonFunc func) {
		make(r, t, f);
		onClick = func;
	}

	void Button::computeTextRect() {
		int w = Font::get().width(text.c_str());
		int h = Font::get().height();
		textRect = Rect(rect.x + rect.z, rect.y + rect.w, rect.x + rect.z, rect.y + rect.w) / 2 + Rect(-w / 2, -h / 2, w / 2, h / 2);
	}

	void Button::setText(const std::string& t) {
		text = t;
		computeTextRect();
	}


	int Button::onPaint(const eventInfo& e) { 
		if (ButtonObject::onPaint(e))
			return 1;
		Font& f = Font::get();
		Renderer::print(text.c_str(), form->image(), textRect.x, textRect.y);
		return 0; 
	}
}


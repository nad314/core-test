#pragma once

namespace core {
	using buttonFunc = void(*)(Form& form);
	class Button : public ButtonObject {
	protected:
		std::string text;
		Rect textRect;

	public:
		Button() :ButtonObject(), text("Button") {}
		Button(const vec4i& r):ButtonObject(r), text("Button") {}
		Button(const vec4i& r, const std::string t, Form& f) : ButtonObject(r, f), text(t) {}

		void make(const vec4i& r, const std::string t, Form& f);
		void make(const vec4i& r, const std::string t, Form& f, buttonFunc func);

		void computeTextRect();
		void setText(const std::string& t);

		virtual int onPaint(const eventInfo& e) override;
	};
}

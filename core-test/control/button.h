#pragma once

namespace core {
	using buttonFunc = void(*)();
	class Button : public Control {
	protected:
		std::string text;
		Form* form;
		vec4i textRect;
		inline void invalidate() { __invalidate();  form->invalidate(); }

	public:
		buttonFunc onClick;
		Button() :Control(), text("Button"), form(NULL), onClick(NULL) {}
		Button(const vec4i& r):Control(r), text("Button"), form(NULL), onClick(NULL) {}
		Button(const vec4i& r, const std::string t, Form& f) : Control(r), text(t), form(&f), onClick(NULL) {}

		void make(const vec4i& r, const std::string t, Form& f);
		void make(const vec4i& r, const std::string t, Form& f, buttonFunc func);

		virtual int onMouseMove(const eventInfo& e) override;
		virtual int onLButtonDown(const eventInfo& e) override;
		virtual int onLButtonUp(const eventInfo& e) override;
		virtual int onRightButtonDown(const eventInfo& e) override;
		virtual int onRightButtonUp(const eventInfo& e) override;
		virtual int onPaint(const eventInfo& e) override;

	};
}

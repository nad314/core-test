#pragma once

namespace core {
	class Button : public Control {
	protected:
		std::string text;
		Form* form;

		Color foreColor;
		Color backColor;
		Color borderColor;
		Color backColorHover;
		Color borderColorHover;
		Color backColorClicked;
		Color borderColorClicked;
		vec4i textRect;

	public:

		Button() :Control(), text("Button"), form(NULL) {}
		Button(const vec4i& r):Control(r), text("Button"), form(NULL) {}
		Button(const vec4i& r, const std::string t, Form& f) : Control(r), text(t), form(&f) {
		}

		inline void make(const vec4i& r, const std::string t, Form& f) {
			rect = r;
			text = t;
			form = &f;
		}

		virtual int onMouseMove(const eventInfo& e) override;
		virtual int onLeftButtonDown(const eventInfo& e) override;
		virtual int onLeftButtonUp(const eventInfo& e) override;
		virtual int onRightButtonDown(const eventInfo& e) override;
		virtual int onRightButtonUp(const eventInfo& e) override;
		virtual int onPaint(const eventInfo& e) override;

	};
}

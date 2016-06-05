#pragma once

namespace core {
	using buttonFunc = void(*)(Form& form);
	class ButtonObject : public Control {
	protected:
		Form* form;
		vec4i textRect;
		inline void invalidate() { __invalidate();  form->invalidate(); }

	public:
		buttonFunc onClick;
		ButtonObject() :Control(), form(NULL), onClick(NULL) {}
		ButtonObject(const vec4i& r) :Control(r), form(NULL), onClick(NULL) {}
		ButtonObject(const vec4i& r, Form& f) : Control(r), form(&f), onClick(NULL) {}

		virtual int onMouseMove(const eventInfo& e) override;
		virtual int onLButtonDown(const eventInfo& e) override;
		virtual int onLButtonUp(const eventInfo& e) override;
		virtual int onRightButtonDown(const eventInfo& e) override;
		virtual int onRightButtonUp(const eventInfo& e) override;
		virtual int onPaint(const eventInfo& e) override;

	};
}


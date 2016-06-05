#pragma once

namespace core {
	class Control {
	protected:
	public:
		vec4i rect;

		Control():rect() {}
		Control(const vec4i& r) :rect(r) {}
		~Control() {}

		virtual int onMouseMove(const eventInfo& e);
		virtual int onLeftButtonDown(const eventInfo& e);
		virtual int onLeftButtonUp(const eventInfo& e);
		virtual int onRightButtonDown(const eventInfo& e);
		virtual int onRightButtonUp(const eventInfo& e);
		virtual int onPaint(const eventInfo& e);
	};
}


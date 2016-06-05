#pragma once

namespace core {
	class Control {
	protected:
		Color foreColor;
		Color backColor;
		Color backColorHover;
		Color foreColorHover;
		Color foreColorActivated;
		Color backColorActivated;
		Color borderColor;

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

		virtual void setForeColor(Color color);
		virtual void setBackColor(Color color);
		virtual void setForeColorHover(Color color);
		virtual void setBackColorHover(Color color);
		virtual void setForeColorActivated(Color color);
		virtual void setBackColorActivated(Color color);
		virtual void setBorderColor(Color color);

	};
}


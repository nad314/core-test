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
		inline void __validate() { flags |= 1; }
		inline void __invalidate() { flags &= ~1; }
		inline void __hover() { flags |= 2; }
		inline void __unhover() { flags &= ~2; }
		inline void __activate() { flags |= 4; }
		inline void __deactivate() { flags &= ~4; }

	public:
		vec4i rect;
		int flags;

		Control():rect(), flags(0) {}
		Control(const vec4i& r) :rect(r), flags(0) {}
		~Control() {}

		virtual inline void move(const vec4i& r) { rect = r; }

		virtual int onMouseMove(const eventInfo& e);
		virtual int onRightButtonDown(const eventInfo& e);
		virtual int onRightButtonUp(const eventInfo& e);
		virtual int onPaint(const eventInfo& e);
		virtual int onLButtonDown(const eventInfo& e);
		virtual int onLButtonUp(const eventInfo& e);
		virtual int onResize(const eventInfo& e);
		virtual int onLButtonDblClk(const eventInfo& e);

		virtual void setForeColor(Color color);
		virtual void setBackColor(Color color);
		virtual void setForeColorHover(Color color);
		virtual void setBackColorHover(Color color);
		virtual void setForeColorActivated(Color color);
		virtual void setBackColorActivated(Color color);
		virtual void setBorderColor(Color color);

	};
}


#pragma once
namespace core {
	class Form : public Window {
	protected:
		Image img;
		buffer<Control*> items;
		int flags;
		Color backColor;
		Color foreColor;
		Color controlBackColor;
		Color controlForeColor;
		Color controlBackColorHover;
		Color controlForeColorHover;
		Color controlForeColorActivated;
		Color controlBackColorActivated;
		Color controlBorderColor;

	public:
		inline Image& image() { return img; }
		inline operator Image&() { return img; }

		void onOpened() override;
		void onClosing() override;
		int onResize (const eventInfo& e) override;
		int onPaint (const eventInfo& e) override;
		int onMouseMove(const eventInfo& e) override;
		int onLButtonDown(const eventInfo& e) override;
		int onLButtonUp(const eventInfo& e) override;


		//validation for redrawing
		inline void __invalidate() { flags &= ~1; } //redraw everything on next draw
		inline void validate() { flags |= 1; }
		inline bool valid() { return flags & 1;  }
		inline void invalidate() { InvalidateRect(hWnd, NULL, false); } //request draw

		inline void push(Control& c) {
			items.push_back(&c);
		}

		virtual void onStartPaint(const eventInfo& e);
		virtual void onControlPaint(const eventInfo& e);
		virtual void onEndPaint(const eventInfo& e);

		void setControlColors(Control& control);

		void setForeColor(Color color);
		void setBackColor(Color color);
		void setControlForeColor(Color color);
		void setControlBackColor(Color color);
		void setControlForeColorHover(Color color);
		void setControlBackColorHover(Color color);
		void setControlForeColorActivated(Color color);
		void setControlBackColorActivated(Color color);
		void setControlBorderColor(Color color);  
	};
}

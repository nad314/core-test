#pragma once
namespace core {
	class Form : public Window {
	protected:
		Image img;
		buffer<Control*> items;
		int flags;
		vec4b backColor;
		vec4b foreColor;

	public:
		inline Image& image() { return img; }
		inline operator Image&() { return img; }

		void onOpened() override;
		void onClosing() override;
		int onResize (const eventInfo& e) override;
		int onPaint (const eventInfo& e) override;

		//validation for redrawing
		inline void invalidate() { flags &= ~1; }
		inline void validate() { flags |= 1; }
		inline bool valid() { return flags & 1;  }

		inline void push(Control& c) {
			items.push_back(&c);
		}

		virtual void onStartPaint(const eventInfo& e);
		virtual void onControlPaint(const eventInfo& e);
		virtual void onEndPaint(const eventInfo& e);
	};
}

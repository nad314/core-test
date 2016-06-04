#pragma once
namespace core {
	class Form : public Window {
	protected:
		Image img;
	private:
		int flags;
		vec4b backColor;
		vec4b foreColor;

	public:
		inline Image& Image() { return img; }

		void onOpened() override;
		void onClosing() override;
		int onResize (const eventInfo& e) override;
		int onPaint (const eventInfo& e) override;

		//validation for redrawing
		inline void invalidate() { flags &= ~1; }
		inline void validate() { flags |= 1; }
		inline bool valid() { return flags & 1;  }

		virtual void onStartPaint();
		virtual void onControlPaint();
		virtual void onEndPaint();
	};
}

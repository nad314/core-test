#pragma once

namespace core {
	class ImageButton : public ButtonObject {
	private:
		Image* ref;
		Image def, hov, act; // default, hover and activated prerendered images
		bool prerendered;
	public:
		ImageButton(): ButtonObject(), prerendered(0), ref(NULL) {};
		ImageButton(const vec4i& r) :ButtonObject(r), prerendered(0), ref(NULL) {}
		ImageButton(const vec4i& r, Image* img, Form& f) : ButtonObject(r, f), prerendered(0), ref(img) {}

		void make(const vec4i& r, Image* img, Form& f);
		void make(const vec4i& r, Image* img, Form& f, buttonFunc func);
		inline void setImage(Image* img) { prerendered = 0; ref = img; }

		void prerender();

		int onPaint(const eventInfo& e) override;

	};
}

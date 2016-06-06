#include <main>
namespace core {
	void ImageButton::make(const vec4i& r, Image* img, Form& f) {
		rect = r;
		form = &f;
		flags = 0;
		setImage(img);
	}

	void ImageButton::make(const vec4i& r, Image* img, Form& f, buttonFunc func) {
		make(r, img, f);
		onClick = func;
	}

	void ImageButton::prerender() {
		prerendered = true;
		def.make(rect.z-rect.x, rect.w-rect.y, 32);
		hov.make(rect.z - rect.x, rect.w - rect.y, 32);
		act.make(rect.z - rect.x, rect.w - rect.y, 32);

		Renderer::clearImage(def, backColor);
		Renderer::blendImage(Rect(0, 0, (int)def.width, (int)def.height), *ref, def);
		Renderer::clearImage(hov, backColorHover);
		Renderer::blendImage(Rect(0, 0, (int)hov.width, (int)hov.height), *ref, hov);
		Renderer::clearImage(act, backColorActivated);
		Renderer::blendImage(Rect(0, 0, (int)act.width, (int)act.height), *ref, act);
	}

	int ImageButton::onPaint(const eventInfo& e) {
		if (ButtonObject::onPaint(e) || ref == NULL)
			return 1;
		if (!prerendered)
			prerender();
		Renderer::drawImage(rect, (flags & 4) ? act : ((flags & 2) ? hov : def), *form);
		return 0;
	}

}
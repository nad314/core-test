#include <main>

void simdView::make (const int width, const int height) {
	img.free();
	img.make (width, height, 32);
	vec4 viewVec = vec4(width, height, 0.5f, 0.0f);
	simdViewVec = viewVec;
	vvs = vec4s(vec4(width-1, height-1, 1.0f, 0.0f));
	v09 = vec4s(vec4(width, height, 1.0f, 1.0f));
}

void simdView::clear() {
	core::Renderer::clearImage(img, core::vec4b(51, 51, 51, 255));
}

void simdView::home() {
	fov = 41.5;
	translation.init();
	rotation.init();
	if(img.height)projection = projection.projection (fov, (float)img.width/img.height, 1.0f, 100.0f);
	translation.translate( 0.0f, 0.0f, -2.0f );
}

void simdView::updateMatrix() {
	if (mode == 0)
		if (img.height)projection = projection.projection(fov, (float)img.width / img.height, 0.001f, 100.0f);
	else if (img.height) {
		float ys = fov / 41.5f;
		float xs = ys * img.width / img.height;
		projection.ortho(-xs, xs, -ys, ys);
	}
	mat = rotation*translation*projection;
}


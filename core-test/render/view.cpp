#include <main>

void simdView::make (const int width, const int height) {
	img.free();
	img.make (width, height, 32);
	viewVec = vec4(width, height, 0.5f, 0.0f);
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

vec4 simdView::project (const vec4 &v) {
	vec4 r(0,0,0,0);
	if (v.w==0.0f)return r;
	r.w=1.0f/v.w;
	r.x=(v.x*r.w*0.5+0.5)*img.width+0;
	r.y=(v.y*r.w*0.5+0.5)*img.height+0;
	r.z=(1.0+v.z*r.w)*0.5;
	return r;
}

vec4 simdView::unproject(const vec4& v) {
	if (v.w == 0.0f)
		return vec4(0.0f);
	vec4 r = v / vec4((float)img.width, (float)img.height, 1.0f, 1.0f) - vec4(0.5f, 0.5f, 0.0f, 0.0f);
	r /= vec4(0.5f, 0.5f, 0.5f, 1.0f);
	r.x /= r.w;
	r.y /= r.w;	
	r.z = (r.z - 1.0f) / r.w;
	r.w = 1.0f;
	return r;
}

__m128 simdView::projectSSE (const __m128 &v, const __m128 &viewVec, const __m128 &v05) const{
	return _mm_mul_ps (_mm_add_ps(
											_mm_mul_ps(
												_mm_mul_ps(v,
													_mm_rcp_ps(_mm_shuffle_ps (v, v, _MM_SHUFFLE(3,3,3,3)))),
												v05), 
											v05), 
										viewVec);
}

void simdView::updateMatrix() {
	if (img.height)projection = projection.projection(fov, (float)img.width / img.height, 0.001f, 100.0f);
	//parallel projection
	/*
	if (img.height) {
		float ys = fov / 41.5f;
		float xs = ys * img.width / img.height;
		projection.ortho(-xs, xs, -ys, ys);
	}
	*/
	mat = rotation*translation*projection;
}


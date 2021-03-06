#include <main>

void Controller::render() {
	wg->executeLocal();
}

void Controller::getPoint(const float x, const float y) {
	core::simdView& view = (static_cast<RenderWindow*>(parent))->view;
	core::PBVH& bvh = storage->pbvh;
	matrixf inv = view.mat;
	inv.invert();
	matrixs sinv = inv;

	core::Ray ray;
	core::PBVH::Ray oray;
	const float h = (float)view.img.height;

	ray.sr0 = sinv*view.unproject(vec4s(vec4(x, (float)h - y, 0.0f, 1.0f)));
	ray.sr0 /= _mm_permute_ps(ray.sr0, 0b11111111);
	ray.sr1 = sinv*view.unproject(vec4s(vec4(x, (float)h - y, 1.0f, 1.0f)));
	ray.sr1 /= _mm_permute_ps(ray.sr1, 0b11111111);
	ray.sr1 = (ray.sr1 - ray.sr0);
	ray.sr1 /= _mm_sqrt_ps(_mm_dp_ps(ray.sr1, ray.sr1, 0x7F));
	ray.sinvr1 = _mm_rcp_ps(ray.sr1);

	oray.r0.x = _mm256_broadcast_ss(&ray.sr0.m.m128_f32[0]);
	oray.r0.y = _mm256_broadcast_ss(&ray.sr0.m.m128_f32[1]);
	oray.r0.z = _mm256_broadcast_ss(&ray.sr0.m.m128_f32[2]);

	oray.r1.x = _mm256_broadcast_ss(&ray.sr1.m.m128_f32[0]);
	oray.r1.y = _mm256_broadcast_ss(&ray.sr1.m.m128_f32[1]);
	oray.r1.z = _mm256_broadcast_ss(&ray.sr1.m.m128_f32[2]);

	oray.inv.x = _mm256_broadcast_ss(&ray.sinvr1.m.m128_f32[0]);
	oray.inv.y = _mm256_broadcast_ss(&ray.sinvr1.m.m128_f32[1]);
	oray.inv.z = _mm256_broadcast_ss(&ray.sinvr1.m.m128_f32[2]);
	oray.d = 100.0f;

	static std::pair<int, float> stack[256];
	int* priority = new int[bvh.inner.size()];
	memset(priority, 0, bvh.inner.size() * sizeof(int));

	if (bvh.findFirst(oray, stack, priority, false) > 0.0f) {
		const vec4s point = ray.sr0 + ray.sr1*vec4s(oray.d);
		point.store(clickPoint);
	}
	else clickPoint = vec4(0.0f);
	delete[] priority;
}

core::Ray Controller::getRay(const float x, const float y) const {
	core::simdView& view = *this->view;
	matrixf inv = view.mat;
	inv.invert();
	matrixs sinv = inv;

	core::Ray ray;
	const float h = (float)view.img.height;

	ray.sr0 = sinv*view.unproject(vec4s(vec4(x, (float)h - y, 0.0f, 1.0f)));
	ray.sr0 /= _mm_permute_ps(ray.sr0, 0b11111111);
	ray.sr1 = sinv*view.unproject(vec4s(vec4(x, (float)h - y, 1.0f, 1.0f)));
	ray.sr1 /= _mm_permute_ps(ray.sr1, 0b11111111);
	ray.sr1 = (ray.sr1 - ray.sr0);
	ray.sr1 /= _mm_sqrt_ps(_mm_dp_ps(ray.sr1, ray.sr1, 0x7F));
	ray.sinvr1 = _mm_rcp_ps(ray.sr1);

	return ray;
}

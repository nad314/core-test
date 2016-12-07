#include <main> 

namespace core {
	void msRenderTask::execute(Renderer::Worker* pWorker) {
		if (pWorker == NULL)
			return;
		Renderer::Worker& worker = *pWorker;
		PBVH& bvh = *pbvh;
		View &view = *pview;
		Image &img = view.img;
		int* mp = reinterpret_cast<int*>(img.data);
		vec4 bp, bq; // bounding box projected coordinates
		Renderer::projectedBox(bvh, pview, bp, bq);

		const int w = img.width;
		const int h = img.height;

		matrixf inv = view.mat;
		inv.invert();
		matrixs sinv = inv;

		const int square = 8;
		vec4s lightPos = inv*vec4(0.0f, 0.0f, -5.0f, 1.0f);
		const float step = 1.0f / (float)samples;

		Ray ray;
		OBVH::Ray oray;
		std::pair<int, float> stack[256];
		int* priority = new int[bvh.inner.size()];
		memset(priority, 0, bvh.inner.size() * sizeof(int));

		__m128 svmin;
		for (int gy = 0; gy < img.height; gy += square) {
			if (gy > bq.y || (gy + square) < bp.y)
				continue;
			for (int gx = square*worker.threadNumber; gx < w; gx += square*worker.threadCount) {
				if (gx > bq.x || (gx + square) < bp.x)
					continue;
				const int mx = std::min(gx + square, w);
				const int my = std::min(gy + square, h);
				for (int i = gy; i < my; ++i) {
					for (int j = gx; j < mx; ++j) {
						int b = 0;
						int c = 0;
						for( int k = 0; k < samples; ++k)
							for (int l = 0; l < samples; ++l) {
								ray.sr0 = sinv*view.unproject(vec4s(vec4((float)j + step*k, (float)h - i + step*l, 0.0f, 1.0f)));
								ray.sr0 /= _mm_permute_ps(ray.sr0, 0b11111111);
								ray.sr1 = sinv*view.unproject(vec4s(vec4((float)j + step*k, (float)h - i + step*l, 1.0f, 1.0f)));
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

								//if (bvh.inner[0].rayIntersectionT(oray, bvh) > 0.0f) {
								if (bvh.rayIntersectionTIt(oray, stack, priority) > 0.0f) {
									const vec4s pminusl = (lightPos - (ray.sr0 + ray.sr1*vec4s(oray.d)));
									const vec4s ndotl = oray.plane.dot3(pminusl / _mm_sqrt_ps(pminusl.dot3(pminusl)));
									b += (byte)_mm_cvtss_si32(_mm_mul_ps(_mm_max_ps(_mm_sub_ps(_mm_setzero_ps(), ndotl), ndotl), _mm_set1_ps(255.0f)));
									++c;
								}
							}
						b /= (samples*samples);
						if (c) {
							int fragOut = 0xff000000 | (b << 16) | (b << 8) | b;
							_mm_stream_si32(mp + j + i*w, fragOut);
						}
					}
				}
			}
		}
		delete[] priority;

	}
}
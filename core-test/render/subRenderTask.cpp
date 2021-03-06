#include <main> 

namespace core {
	void subRenderTask::execute(Renderer::Worker* pWorker) {
		if (pWorker == NULL)
			return;
		Renderer::Worker& worker = *pWorker;
		PBVH& bvh = *pbvh;
		simdView &view = *pview;
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

		Ray ray;
		PBVH::Ray oray;
		std::pair<int, float> stack[256];
		int* priority = new int[bvh.inner.size()];
		memset(priority, 0, bvh.inner.size() * sizeof(int));

		//__m128 svmin;
		for (int gy = 0; gy < img.height; gy += square) {
			if (gy > bq.y || (gy + square) < bp.y)
				continue;
			for (int gx = square*worker.threadNumber; gx < w; gx += square*worker.threadCount) {
				if (gx > bq.x || (gx + square) < bp.x)
					continue;
				const int mx = std::min(gx + square, w);
				const int my = std::min(gy + square, h);
				for (int i = gy; i < my-my%2; i+=2) {
					for (int j = gx; j < mx-mx%2; j+=2) {
						ray.sr0 = sinv*view.unproject(vec4s(vec4((float)j, (float)h - i, 0.0f, 1.0f)));
						ray.sr0 /= _mm_permute_ps(ray.sr0, 0b11111111);
						ray.sr1 = sinv*view.unproject(vec4s(vec4((float)j, (float)h - i, 1.0f, 1.0f)));
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

						if (bvh.findFirst(oray, stack, priority, true) > 0.0f) {
							const vec4s pminusl = (lightPos - (ray.sr0 + ray.sr1*vec4s(oray.d)));
							const vec4s ndotl = oray.plane.dot3(pminusl / _mm_sqrt_ps(pminusl.dot3(pminusl)));
							const byte b = (byte)_mm_cvtss_si32(_mm_mul_ps(_mm_max_ps(_mm_set1_ps(0.3f), _mm_max_ps(_mm_sub_ps(_mm_setzero_ps(), ndotl), ndotl)), _mm_set1_ps(255.0f)));
							int fragOut = 0xff000000 | (b << 16) | (b << 8) | b;
							_mm_stream_si32(mp + j + i*w, fragOut);
							_mm_stream_si32(mp + j + 1 + i*w, fragOut);
							_mm_stream_si32(mp + j + 1 + (i + 1)*w, fragOut);
							_mm_stream_si32(mp + j + (i + 1)*w, fragOut);
						}
					}
				}
				
				/*
				for (int i = gy; i < my - 1; i++)
					for (int j = gx; j < mx - 1; j++) {
						const int frag00 = *(mp + j + i*w);
						const int frag01 = *(mp + j + 1 + i*w);
						const int frag11 = *(mp + j + 1 + (i + 1)*w);
						const int frag10 = *(mp + j + (i + 1)*w);
						vec4i c00 = vec4i(frag00 & 0xff, (frag00 >> 8) & 0xff, (frag00 >> 16) & 0xff, (frag00 >> 24) & 0xff);
						vec4i c01 = vec4i(frag01 & 0xff, (frag01 >> 8) & 0xff, (frag01 >> 16) & 0xff, (frag01 >> 24) & 0xff);
						vec4i c11 = vec4i(frag11 & 0xff, (frag11 >> 8) & 0xff, (frag11 >> 16) & 0xff, (frag11 >> 24) & 0xff);
						vec4i c10 = vec4i(frag10 & 0xff, (frag10 >> 8) & 0xff, (frag10 >> 16) & 0xff, (frag10 >> 24) & 0xff);
						c00 += c01 + c10 + c11;
						c00 /= 4;
						int fragOut = 0xff000000 | (c00.z << 16) | (c00.y << 8) | c00.z;
						_mm_stream_si32(mp + j + i*w, fragOut);
					}*/
			}
		}
		delete[] priority;

	}
}

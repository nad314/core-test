#include <main>
namespace core {
	void Renderer::raytrace(PolyOctree& octree, OBVH& bvh, View* pview) {
		View &view = *pview;
		Image &img = view.img;
		uint* mp = reinterpret_cast<uint*>(img.data);
		//vec4 pp, qq; //because SSE to non SSE conversion
		vec4 bp, bq; // bounding box projected coordinates
		projectedBox(octree, pview, bp, bq);
		/*
		p.store(pp);
		q.store(qq);
		*/
		/*
		pp = octree.root->p;
		qq = octree.root->q;
		*/
		vector4<byte> color(255, 255, 255, 255);
		uint clr = *reinterpret_cast<const uint*>(&color);
		const int w = img.width;

		matrixf inv = view.mat;
		inv.invert();

		const int square = 32;
		vec4s lightPos = view.mat*vec4(0.0f, 0.0f, -2.0f, 1.0f);

		Ray ray;
		matrixs sinv = inv;
		const vec4s vone = vec4(1.0f, 1.0f, 1.0f, 1.0f);

		PolyOctree::Node& node = *octree.root;
		vec4 r0, r1, rinv;

		OBVH::Ray oray;

		__m128 svmin;
		for (int gy = 0; gy < img.height; gy += square) {
			if (gy > bq.y || (gy + square) < bp.y)
				continue;
			for (int gx = 0; gx < w; gx += square) {
				if (gx > bq.x || (gx + square) < bp.x)
					continue;
				const int mx = std::min(gx + square, w);
				const int my = std::min(gy + square, (int)img.height);
				for (int i = gy; i < my; ++i) {
					for (int j = gx; j < mx; ++j) {
						ray.sr0 = sinv*view.unproject(vec4s(vec4((float)j, (float)img.height - i, 0.0f, 1.0f)));
						ray.sr0 /= _mm_shuffle_ps(ray.sr0, ray.sr0, _MM_SHUFFLE(3, 3, 3, 3));
						ray.sr1 = sinv*view.unproject(vec4s(vec4((float)j, (float)img.height - i, 1.0f, 1.0f)));
						ray.sr1 /= _mm_shuffle_ps(ray.sr1, ray.sr1, _MM_SHUFFLE(3, 3, 3, 3));
						ray.sr1 = (ray.sr1 - ray.sr0);
						ray.sr1 /= _mm_sqrt_ps(_mm_dp_ps(ray.sr1, ray.sr1, 0x7F));
						ray.sinvr1 = vone / ray.sr1;
						ray.d = 100.0f;

						ray.sr0.store(r0);
						ray.sr1.store(r1);
						ray.sinvr1.store(rinv);

						oray.r0.x = _mm256_broadcast_ss(&r0.x);
						oray.r0.y = _mm256_broadcast_ss(&r0.y);
						oray.r0.z = _mm256_broadcast_ss(&r0.z);

						oray.r1.x = _mm256_broadcast_ss(&r1.x);
						oray.r1.y = _mm256_broadcast_ss(&r1.y);
						oray.r1.z = _mm256_broadcast_ss(&r1.z);

						oray.inv.x = _mm256_broadcast_ss(&rinv.x);
						oray.inv.y = _mm256_broadcast_ss(&rinv.y);
						oray.inv.z = _mm256_broadcast_ss(&rinv.z);
						oray.d = 100.0f;

						if (bvh.inner[0].rayIntersectionT(oray, bvh) > 0.0f) {
							const vec4s pminusl = (lightPos - (ray.sr0 + ray.sr1*vec4s(oray.d)));
							const vec4s ndotl = oray.plane.dot3(pminusl / _mm_sqrt_ps(pminusl.dot3(pminusl)));
							const byte b = (byte)_mm_cvtss_si32(_mm_mul_ps(_mm_max_ps(_mm_setzero_ps(), ndotl), _mm_set1_ps(255.0f)));
							color = vec4b(b, b, b, 255);
							clr = *reinterpret_cast<const uint*>(&color);
							memcpy(mp + j + i * w, &clr, 4);
						}
					}
				}
			}
		}
	}
}

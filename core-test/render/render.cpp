#include <main>

namespace core {

	bool Renderer::valid = false;

	void Renderer::drawPoints(simdMesh &mesh, View* pview) {
		if (!pview)return;
		View &view = *pview;
		Image &img = view.img;
		view.clear();
		_mm_prefetch(reinterpret_cast<const char*>(static_cast<vec4s*>(mesh.vecs)), _MM_HINT_T0);

		static const vector4<byte> color(255, 255, 255, 255);
		const uint clr = *reinterpret_cast<const uint*>(&color);
		const matrixs& mat = static_cast<const matrixs&>(view.mat);
		uint* mp = reinterpret_cast<uint*>(img.data);
		const int w = img.width;

		__m128i mi;
		for (const auto &i : mesh.vecs) {
			mi = view.clamp(view.project(mat*i)).trunc();
			memcpy(mp + mi.m128i_i32[0] + mi.m128i_i32[1] *w, &clr, 4);
		}
	}

	void Renderer::drawPointRange(simdMesh &mesh, View* pview, const int& start, const int& stop) {
		if (!pview)return;
		View &view = *pview;
		Image &img = view.img;
		//view.clear();
		//_mm_prefetch(reinterpret_cast<const char*>(static_cast<vec4s*>(mesh.vecs)), _MM_HINT_T0);
		vec4s* vi = &mesh.vecs[0];

		const vector4<byte> color(255, 255, 255, 255);
		const uint clr = *reinterpret_cast<const uint*>(&color);
		const matrixs& mat = static_cast<const matrixs&>(view.mat);
		uint* mp = reinterpret_cast<uint*>(img.data);
		const int w = img.width;

		__m128i mi;
		for (int i = start; i < stop; ++i) {
			mi = view.clamp(view.project(mat* *(vi+i))).trunc();
			memcpy(mp + mi.m128i_i32[0] + mi.m128i_i32[1] * w, &clr, 4);
		}
	}

	void Renderer::drawPointThread(simdMesh &mesh, View* pview, const int& start, const int& stop, bool* done) {
		while (! *done) {
			while (valid)Sleep(1);
			drawPointRange(mesh, pview, start, stop);
		}
	}

	void Renderer::raytrace(PolyOctree& octree, View* pview) {
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

		for (int gy = 0; gy<img.height; gy+= square) {
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
						
						Renderer::rayBoxIntersectionTestSIMD2(ray, node.spp, node.sqq);
						if (_mm_comilt_ss(ray.svmin, _mm_setzero_ps()) || _mm_comilt_ss(_mm_set1_ps(ray.d), ray.svmin))
							continue;
						//if (_mm_comigt_ss(node.rayIntersectionT(ray), _mm_setzero_ps())) {
						if (node.rayIntersectionT(ray) > 0.0f) {
							const vec4s pminusl = (lightPos - (ray.sr0 + ray.sr1*vec4s(ray.d)));
							const vec4s ndotl = ray.plane.dot3(pminusl/_mm_sqrt_ps(pminusl.dot3(pminusl)));
							const byte b = (byte)_mm_cvtss_si32(_mm_mul_ps(_mm_max_ps(_mm_setzero_ps(), ndotl),_mm_set1_ps(255.0f)));
							color = vec4b(b, b, b, 255);
							clr = *reinterpret_cast<const uint*>(&color);
							memcpy(mp + j + i * w, &clr, 4);
						}
					}
				}
			}
		}
	}


	void Renderer::projectedBox(const PolyOctree& octree, const View* pview, vec4& pOut, vec4& qOut) {
		vec4s pO, qO;
		vec4 p, q;
		octree.root->spp.store(p);
		octree.root->sqq.store(q);
		const View& view = *pview;

		vec4s ps = view.project(vec4s(view.mat*p));
		pO = qO = ps;

		ps = view.project(vec4s(view.mat*vec4(q.x, q.y, p.z, 1.0f)));
		pO = _mm_min_ps(pO, ps);
		qO = _mm_max_ps(qO, ps);

		ps = view.project(vec4s(view.mat*vec4(p.x, q.y, p.z, 1.0f)));
		pO = _mm_min_ps(pO, ps);
		qO = _mm_max_ps(qO, ps);

		ps = view.project(vec4s(view.mat*vec4(q.x, p.y, p.z, 1.0f)));
		pO = _mm_min_ps(pO, ps);
		qO = _mm_max_ps(qO, ps);



		ps = view.project(vec4s(view.mat*vec4(p.x, p.y, q.z, 1.0f)));
		pO = _mm_min_ps(pO, ps);
		qO = _mm_max_ps(qO, ps);

		ps = view.project(vec4s(view.mat*vec4(q.x, q.y, q.z, 1.0f)));
		pO = _mm_min_ps(pO, ps);
		qO = _mm_max_ps(qO, ps);

		ps = view.project(vec4s(view.mat*vec4(p.x, q.y, q.z, 1.0f)));
		pO = _mm_min_ps(pO, ps);
		qO = _mm_max_ps(qO, ps);

		ps = view.project(vec4s(view.mat*vec4(q.x, p.y, q.z, 1.0f)));
		pO = _mm_min_ps(pO, ps);
		qO = _mm_max_ps(qO, ps);

		pO.store(pOut);
		qO.store(qOut);
	}
}

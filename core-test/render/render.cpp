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

	void Renderer::raytrace(const vec4s& p, const vec4s& q, View* pview) {
		View &view = *pview;
		Image &img = view.img;
		uint* mp = reinterpret_cast<uint*>(img.data);
		vec4 pp, qq;
		p.store(pp);
		q.store(qq);
		const vector4<byte> color(255, 255, 255, 255);
		const uint clr = *reinterpret_cast<const uint*>(&color);
		const int w = img.width;

		matrixf inv = view.mat;
		inv.invert();
		for (int i = 0; i < img.height; ++i) {
			for (int j = 0; j < w; ++j) {
				vec4 rr0 = inv*view.unproject(vec4((float)j, (float)i, 0.0f, 1.0f));
				vec4 rr1 = inv*view.unproject(vec4((float)j, (float)i, 1.0f, 1.0f));
				rr0 /= rr0.w;
				rr1 /= rr1.w;
				rr1 = (rr1 - rr0).normalize3d();
				if (rayBoxIntersectionTest(rr0, rr1, pp, qq))
					memcpy(mp + j + i * w, &clr, 4);
			}
		}
	}

	bool Renderer::rayBoxIntersectionTest(const vec4& r0, const vec4& r1, const vec4& p, const vec4& q) {
		float invD = 1.0f / r1.x;
		vec4 rd = vec4(1.0f) / r1;
		float t1 = (p.x - r0.x)*rd.x;
		float t2 = (q.x - r0.x)*rd.x;
		float t3 = (p.y - r0.y)*rd.y;
		float t4 = (q.y - r0.y)*rd.y;
		float t5 = (p.z - r0.z)*rd.z;
		float t6 = (q.z - r0.z)*rd.z;

#define min std::min
#define max std::max

		float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
		float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

		// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
		if (tmax < 0)
		{
			//t = tmax;
			return false;
		}

		// if tmin > tmax, ray doesn't intersect AABB
		if (tmin > tmax)
		{
			//t = tmax;
			return false;
		}

		//t = tmin;
		return true;
#undef min
#undef max
	}
}

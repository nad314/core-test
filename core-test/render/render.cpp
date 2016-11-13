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
		vec4 pp, qq;
		/*
		p.store(pp);
		q.store(qq);
		*/
		pp = octree.root.p;
		qq = octree.root.q;
		const vector4<byte> color(255, 255, 255, 255);
		const uint clr = *reinterpret_cast<const uint*>(&color);
		const int w = img.width;

		matrixf inv = view.mat;
		inv.invert();

		for (int i = 0; i < img.height; ++i) {
			for (int j = 0; j < w; ++j) {
				vec4 rr0 = inv*view.unproject(vec4((float)j, (float)img.height-i, 0.0f, 1.0f));
				vec4 rr1 = inv*view.unproject(vec4((float)j, (float)img.height - i, 1.0f, 1.0f));
				rr0 /= rr0.w;
				rr1 /= rr1.w;
				rr1 = (rr1 - rr0).normalize3d();
				if (octree.root.rayIntersectionT(rr0, rr1)>0.0f)
					memcpy(mp + j + i * w, &clr, 4);
			}
		}
	}
}

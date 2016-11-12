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

}

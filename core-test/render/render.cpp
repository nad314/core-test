#include <main>

namespace core {
	void Renderer::drawModel(basicMesh &mesh, pView* pview) {
		if (!pview)return;
		pView &view = *pview;
		Image &img = view.img;
		static const vector3<byte> color = vector3<byte>(255, 255, 255);
		vec4 v;
		view.clear();

		for (int i = 0; i < mesh.vecs.size(); ++i) {
			v = view.project(view.mat*mesh.vecs[i]);
			if (v.x >= 0.0f && v.x < img.width && v.y >= 0.0f && v.y<img.height && v.z>0.0f)
				putPixel((int)v.x, (int)v.y, color, img);
		}
	}

	void Renderer::drawPoints(simdMesh &mesh, pView* pview) {
		if (!pview)return;
		pView &view = *pview;
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
}

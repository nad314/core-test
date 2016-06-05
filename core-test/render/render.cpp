#include <main>

namespace core {
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

	bool Renderer::clearImage(Image& img, vec4b color) {
		if (img.bits != 32 || img.width == 0 || img.height == 0)
			return false;
		float* data = reinterpret_cast<float*>(img.data);
		int mod = img.width*img.height % 4;
		int to = img.width*img.height - mod;
		float clr = *(float*)&color;
		__m128 xmm0 = _mm_set1_ps(clr);
		for (int i = 0; i < to; i+=4)
			_mm_storeu_ps(data + i, xmm0);
		for (int i = to; i < to + mod; ++i)
			*(data + i) = clr;
		return true;
	}

	bool Renderer::drawRect(vec4i rect, vec4b color, Image &img) {
		if (img.bits != 32 || img.width == 0 || img.height == 0)
			return false;
		if (rect.z < rect.x || rect.w < rect.y)
			return false;
		--rect.w;
		--rect.z;
		float clr = *(float*)&color;
		__m128 xmm0 = _mm_set1_ps(clr);

		float* data = reinterpret_cast<float*>(img.data);
		data += img.width * (int)rect.y;
		float* data2 = data + img.width * (int)(rect.w-rect.y);
		int mod = img.width % 4;
		int to = (int)rect.z - mod;
		for (int i = rect.x; i < to; i += 4) {
			_mm_storeu_ps(data + i, xmm0);
			_mm_storeu_ps(data2 + i, xmm0);
		}
		for (int i = to; i < to + mod; ++i) {
			*(data + i) = clr;
			*(data2 + i) = clr;
		}
		
		data += rect.x;
		data2 = data + rect.z - rect.x;
		to = (rect.w - rect.y)*img.width;
		for (int i = 0; i < to; i += img.width) {
			*(data + i) = clr;
			*(data2 + i) = clr;
		}

		return true;
	}


	bool Renderer::fillRect(vec4i rect, vec4b color, Image &img) {
		if (img.bits != 32 || img.width == 0 || img.height == 0)
			return false;
		if (rect.z < rect.x || rect.w < rect.y)
			return false;
		--rect.w;
		--rect.z;
		float clr = *(float*)&color;
		__m128 xmm0 = _mm_set1_ps(clr);

		for (int i = rect.y; i < rect.w; ++i) {
			float* data = reinterpret_cast<float*>(img.data) + img.width * i;
			int mod = img.width % 4;
			int to = (int)rect.z - mod;
			for (int i = rect.x; i < to; i += 4)
				_mm_storeu_ps(data + i, xmm0);
			for (int i = to; i < to + mod; ++i)
				*(data + i) = clr;
		}

		return true;
	}

}

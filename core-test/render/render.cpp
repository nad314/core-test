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
		if (rect.x >= img.width || rect.z < 0 || rect.y >= img.height || rect.w < 0)
			return false;		
		rect.w = std::min(--rect.w, (int)img.height-1);
		rect.z = std::min(--rect.z, (int)img.width-1);
		rect.x = std::max(rect.x, 0);
		rect.y = std::max(rect.y, 0);
		if (rect.z < rect.x || rect.w < rect.y)
			return false;

		float clr = *(float*)&color;
		__m128 xmm0 = _mm_set1_ps(clr);

		float* data = reinterpret_cast<float*>(img.data);
		data += img.width * (int)rect.y;
		float* data2 = data + img.width * (int)(rect.w-rect.y);
		int mod = (rect.z-rect.x) % 4;
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
		if (rect.x >= img.width || rect.z < 0 || rect.y >= img.height || rect.w < 0)
			return false;
		rect.w = std::min(--rect.w, (int)img.height-1);
		rect.z = std::min(--rect.z, (int)img.width-1);
		rect.x = std::max(rect.x, 0);
		rect.y = std::max(rect.y, 0);
		if (rect.z < rect.x || rect.w < rect.y)
			return false;

		float clr = *(float*)&color;
		__m128 xmm0 = _mm_set1_ps(clr);

		for (int i = rect.y; i < rect.w; ++i) {
			float* data = reinterpret_cast<float*>(img.data) + img.width * i;
			int mod = (rect.z-rect.x) % 4;
			int to = (int)rect.z - mod;
			for (int i = rect.x; i < to; i += 4)
				_mm_storeu_ps(data + i, xmm0);
			for (int i = to; i < to + mod; ++i)
				*(data + i) = clr;
		}
		return true;
	}

	bool Renderer::drawImage(Rect rect, Image& imgFrom, Image& imgTo) {
		int i, j;
		Rect pos(0, 0, imgFrom.width, imgFrom.height);
		if (rect.x < 0) { pos.x -= rect.x; rect.x = 0; }
		if (rect.y < 0) { pos.y -= rect.y; rect.y = 0; }
		rect.z = std::min(std::min(rect.z, (int)imgTo.width), rect.x+(int)imgFrom.width);
		rect.w = std::min(std::min(rect.w, (int)imgTo.height), rect.y+(int)imgFrom.height);
		pos.z = std::min(pos.x + rect.z - rect.x, pos.z);
		pos.w = std::min(pos.y + rect.w - rect.y, pos.w);

		int* dataFrom = ((int*)imgFrom.data) + imgFrom.width*pos.y + pos.x;
		int* dataTo = ((int*)imgTo.data) + imgTo.width*rect.y + rect.x;

		pos.z -= pos.x;

		__m128i xmm0, xmm1, xmm2, xmm3;
		int alpha;

		int div = pos.z / 4;
		int mod = pos.z % 4;
		int to = pos.z - mod;


		for (i = pos.y; i < pos.w; ++i) {
			for (j = 0; j < to; j+=4)
				_mm_storeu_ps((float*)(dataTo + j), _mm_loadu_ps((float*)(dataFrom + j)));
			for (j = to; j < rect.z; ++j)
				*(dataTo + j) = *(dataFrom + j);
			dataFrom += imgFrom.width;
			dataTo += imgTo.width;
		}

		return true;
	}


	bool Renderer::blendImage(Rect rect, Image& imgFrom, Image& imgTo) {
		int i, j;
		Rect pos(0, 0, imgFrom.width, imgFrom.height);
		if (rect.x < 0) { pos.x -= rect.x; rect.x = 0; }
		if (rect.y < 0) { pos.y -= rect.y; rect.y = 0; }
		rect.z = std::min(std::min(rect.z, (int)imgTo.width), rect.x + (int)imgFrom.width);
		rect.w = std::min(std::min(rect.w, (int)imgTo.height), rect.y + (int)imgFrom.height);
		pos.z = std::min(pos.x + rect.z - rect.x, pos.z);
		pos.w = std::min(pos.y + rect.w - rect.y, pos.w);

		int* dataFrom = ((int*)imgFrom.data) + imgFrom.width*pos.y + pos.x;
		int* dataTo = ((int*)imgTo.data) + imgTo.width*rect.y + rect.x;

		pos.z -= pos.x;

		__m128i xmm0, xmm1, xmm2, xmm3;
		int alpha;

		for (i = pos.y; i < pos.w; ++i) {
			for (j = 0; j < pos.z; ++j) {
				alpha = (*(dataFrom + j)) & 0xff000000;
				xmm1 = _mm_cvtsi32_si128(alpha | alpha >> 8 | alpha >> 16 | alpha >> 24);
				xmm1 = _mm_unpacklo_epi8(xmm1, _mm_setzero_si128());
				xmm1 = _mm_unpacklo_epi16(xmm1, _mm_setzero_si128());
				xmm3 = _mm_sub_epi32(_mm_set1_epi32(255), xmm1);

				xmm0 = _mm_cvtsi32_si128(*(dataFrom + j));
				xmm0 = _mm_unpacklo_epi8(xmm0, _mm_setzero_si128());
				xmm0 = _mm_unpacklo_epi16(xmm0, _mm_setzero_si128());
				xmm0 = _mm_mullo_epi32(xmm0, xmm1);
				xmm0 = _mm_srli_epi32(xmm0, 8);

				xmm2 = _mm_cvtsi32_si128(*(dataTo + j));
				xmm2 = _mm_unpacklo_epi8(xmm2, _mm_setzero_si128());
				xmm2 = _mm_unpacklo_epi16(xmm2, _mm_setzero_si128());
				xmm2 = _mm_mullo_epi32(xmm2, xmm3);
				xmm2 = _mm_srli_epi32(xmm2, 8);

				xmm0 = _mm_add_epi32(xmm0, xmm2);

				*(dataTo + j) = xmm0.m128i_i32[0] | xmm0.m128i_i32[1] << 8 | xmm0.m128i_i32[2] << 16 | xmm0.m128i_i32[0] << 24;

			}
			dataFrom += imgFrom.width;
			dataTo += imgTo.width;
		}

		return true;
	}

}

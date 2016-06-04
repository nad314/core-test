#pragma once
namespace core {
	class Renderer {
	private:
	public:
		static void drawPoints(simdMesh &mesh, View* view);
		static inline void putPixel(const int &x, const int &y, const vector3<byte> &v, Image &img) {
			memcpy(img.data + (y*img.width + x) * 4, v, 3);
		}

		static inline void putPixel(const int &x, const int &y, const vector4<byte> &v, Image &img) {
			memcpy(img.data + (y*img.width + x) * 4, v, 4);
		}

		static inline void print(const char* text, Image& img, int x, int y) {
			Font::get().print(text, img, x, y);
		}

		static bool clearImage(Image& img, vec4b color);
		static bool drawRect(vec4i rect, vec4b color, Image &img);

	};
}


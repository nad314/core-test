#pragma once
namespace core {
	class Renderer {
	private:
	public:
		static void drawModel(basicMesh &mesh, pView* view);
		static void drawPoints(simdMesh &mesh, pView* view);
		static inline void putPixel(const int &x, const int &y, const vector3<byte> &v, Image &img) {
			memcpy(img.data + (y*img.width + x) * 4, v, 3);
		}

		static inline void putPixel(const int &x, const int &y, const vector4<byte> &v, Image &img) {
			memcpy(img.data + (y*img.width + x) * 4, v, 4);
		}
	};
}


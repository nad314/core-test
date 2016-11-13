#pragma once
namespace core {
	class Renderer : public Core2D {
	private:
		static bool valid;
	public:

		inline static void invalidate() { valid = 0; }
		static void drawPoints(simdMesh &mesh, View* view);
		static void drawPointRange(simdMesh &mesh, View* view, const int& start, const int& stop);
		static void drawPointThread(simdMesh &mesh, View* view, const int& start, const int& stop, bool* done);

		static void raytrace(const vec4s& p, const vec4s& q, View* view);
		static bool rayBoxIntersectionTest(const vec4& r0, const vec4& r1, const vec4& p, const vec4& q);
	};
}


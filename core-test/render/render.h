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

		static void raytrace(PolyOctree& octree, View* view);

		static inline bool rayBoxIntersectionTest(const Ray& ray, const vec4& p, const vec4& q) {
			vec4 v0 = (p - ray.r0)*ray.invr1;
			vec4 v1 = (q - ray.r0)*ray.invr1;

			float tmin = std::max(std::max(std::min(v0.x, v1.x), std::min(v0.y, v1.y)), std::min(v0.z, v1.z));
			float tmax = std::min(std::min(std::max(v0.x, v1.x), std::max(v0.y, v1.y)), std::max(v0.z, v1.z));
			return (tmax > 0 && tmin < tmax);
		}

		static inline float rayBoxIntersectionTestF(const Ray& ray, const vec4& p, const vec4& q) {
			const vec4 v0 = (p - ray.r0)*ray.invr1;
			const vec4 v1 = (q - ray.r0)*ray.invr1;
			const vec4 vmin = v0.min(v1);
			const vec4 vmax = v0.max(v1);

			const float tmin = std::max(std::max(vmin.x, vmin.y), vmin.z);
			const float tmax = std::min(std::min(vmax.x, vmax.y), vmax.z);
			if (tmax<0 || tmin>tmax) return -1.0f;
			return tmin;
		}

	};
}


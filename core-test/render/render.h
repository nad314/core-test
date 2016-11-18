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
		/*
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
			if (tmin>tmax || tmax<0.0f) return -1.0f;
			return tmin;
		}*/

		//stores minimum distance into ray, returns maximum distance
		static inline const vec4s rayBoxIntersectionTestSIMD(Ray& ray, const vec4s& p, const vec4s& q) {
			const vec4s v0 = (p - ray.sr0)*ray.sinvr1;
			const vec4s v1 = (q - ray.sr0)*ray.sinvr1;
			const vec4s min0 = _mm_min_ps(v0, v1);
			const vec4s max0 = _mm_max_ps(v0, v1);
			const vec4s min1 = _mm_max_ps(min0, _mm_permute_ps(min0, 0b10110001));
			const vec4s max1 = _mm_min_ps(max0, _mm_permute_ps(max0, 0b10110001));
			ray.svmin = _mm_max_ps(min1, _mm_permute_ps(min1, 0b01001011));
			return _mm_min_ps(max1, _mm_permute_ps(max1, 0b01001011));
		}

		static inline bool raySpehereIntersectionTestSIMD(const Ray& ray, const vec4s& c, const vec4s& r) {
			const vec4s l = c - ray.sr0;
			const vec4s t = l.dot3(ray.sr1);
			if (_mm_comilt_ss(t, _mm_setzero_ps()))
				return 0;
			const vec4s d2 = l.dot3(l) - t*t;
			if (_mm_comilt_ss(r, d2))
				return 0;
			return 1;
		}

		static inline void raySpehereIntersectionSIMD(Ray& ray, const vec4s& c, const vec4s& r) {
			ray.svmin = _mm_set1_ps(-1.0f);
			const vec4s l = c - ray.sr0;
			const vec4s t = l.dot3(ray.sr1);
			if (_mm_comilt_ss(t, _mm_setzero_ps()))
				return;
			const vec4s d2 = l.dot3(l) - t*t;
			if (_mm_comilt_ss(r, d2))
				return;
			ray.svmin = t - _mm_sqrt_ps(r - d2);
		}

		static void projectedBox(const PolyOctree& octree, const View* view, vec4& pOut, vec4& qOut);

	};
}


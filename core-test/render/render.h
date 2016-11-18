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
			if (tmin>tmax || tmax<0.0f) return -1.0f;
			return tmin;
		}

		static inline void rayBoxIntersectionTestSIMD(Ray& ray, const vec4s& p, const vec4s& q) {
			const vec4s v0 = (p - ray.sr0)*ray.sinvr1;
			const vec4s v1 = (q - ray.sr0)*ray.sinvr1;
			vec4s vmin = _mm_min_ps(v0, v1);
			vec4s vmax = _mm_max_ps(v0, v1);
			vmin = _mm_max_ps(vmin, _mm_shuffle_ps(vmin, vmin, _MM_SHUFFLE(1, 0, 3, 2)));
			vmin = _mm_max_ps(vmin, _mm_shuffle_ps(vmin, vmin, _MM_SHUFFLE(1, 0, 2, 3)));

			vmax = _mm_min_ps(vmax, _mm_shuffle_ps(vmax, vmax, _MM_SHUFFLE(1, 0, 3, 2)));
			vmax = _mm_min_ps(vmax, _mm_shuffle_ps(vmax, vmax, _MM_SHUFFLE(1, 0, 2, 3)));

			//vmax = _mm_cmpgt_ps(_mm_max_ps(_mm_max_ps(vmin, vnull), vnull), vmax);
			//vmax = _mm_or_ps(_mm_cmpgt_ps(vmin, vmax), _mm_cmpgt_ps(vnull, vmax));

			//_mm_store_ss(ray.vmin, vmin);
			vmin.store(ray.vmin);
			//vmax.store(ray.vmax);
			/*
			const float tmin = std::max(std::max(ray.vmin.x, ray.vmin.y), ray.vmin.z);
			const float tmax = std::min(std::min(ray.vmax.x, ray.vmax.y), ray.vmax.z);*/

			if (_mm_comige_ss(vmin, vmax) || _mm_comige_ss(_mm_setzero_ps(), vmax))
				ray.vmin.x = -1.0f;

			/*
			if (ray.vmax.x < ray.vmin.x || ray.vmax.x < 0.0f)
				ray.vmin.x = -1.0f;
				*/
		}


		static inline void rayBoxIntersectionTestSIMD2(Ray& ray, const vec4s& p, const vec4s& q) {
			const vec4s v0 = (p - ray.sr0)*ray.sinvr1;
			const vec4s v1 = (q - ray.sr0)*ray.sinvr1;
			ray.svmin = _mm_min_ps(v0, v1);
			ray.svmax = _mm_max_ps(v0, v1);
			ray.svmin = _mm_max_ps(ray.svmin, _mm_shuffle_ps(ray.svmin, ray.svmin, _MM_SHUFFLE(1, 0, 3, 2)));
			ray.svmin = _mm_max_ps(ray.svmin, _mm_shuffle_ps(ray.svmin, ray.svmin, _MM_SHUFFLE(1, 0, 2, 3)));
			ray.svmax = _mm_min_ps(ray.svmax, _mm_shuffle_ps(ray.svmax, ray.svmax, _MM_SHUFFLE(1, 0, 3, 2)));
			ray.svmax = _mm_min_ps(ray.svmax, _mm_shuffle_ps(ray.svmax, ray.svmax, _MM_SHUFFLE(1, 0, 2, 3)));
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


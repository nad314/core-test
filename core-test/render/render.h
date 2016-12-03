#pragma once
namespace core {
	class Renderer: public SIMD, public Core2D {
	private:
		static bool valid;
	public:

		struct Worker: public SIMD {
			std::pair<int, float> stack[256];
			int* priority;
			std::thread thread;
			int threadNumber;
			int threadCount;
			bool done;
			bool pass;
			static int go;
			static int stop;
			static std::condition_variable cv;
			static std::mutex mutex;

			Worker() { priority = NULL; };
			~Worker() { if (priority)delete[] priority; priority = NULL; }

			void create(OBVH& bvh, View* view, const int& tn, const int& tc);
			void render(OBVH& bvh, View* view);
			void join();

			void start();
			void wait();

			void threadFunc(OBVH& bvh, View* view);

		};

		inline static void invalidate() { valid = 0; }
		static void drawPoints(simdMesh &mesh, View* view);
		static void drawPointRange(simdMesh &mesh, View* view, const int& start, const int& stop);
		static void drawPointRange(PointCloud &mesh, View* view, const int& start, const int& stop);
		static void drawPointThread(simdMesh &mesh, View* view, const int& start, const int& stop, bool* done);

		static void raytrace(PolyOctree& octree, View* view);
		static void raytrace(OBVH& bvh, View* view);
		static void raytrace(PBVH& bvh, View* view);
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

		//commented because removing unnecessary varibales from Ray class due to cache problems
		//stores minimum distance into ray, returns maximum distance
		
		static inline const vec4s rayBoxIntersectionTestSIMD(Ray& ray, const vec4s& p, const vec4s& q, __m128& svmin) {
			const vec4s v0 = (p - ray.sr0)*ray.sinvr1;
			const vec4s v1 = (q - ray.sr0)*ray.sinvr1;
			const vec4s min0 = _mm_min_ps(v0, v1);
			const vec4s max0 = _mm_max_ps(v0, v1);
			const vec4s min1 = _mm_max_ps(min0, _mm_permute_ps(min0, 0b01001010));
			const vec4s max1 = _mm_min_ps(max0, _mm_permute_ps(max0, 0b01001010));
			svmin = _mm_max_ss(min1, _mm_permute_ps(min1, 0b11100001));
			return _mm_min_ss(max1, _mm_permute_ps(max1, 0b11100001));
		}
		

		//commented because removing unnecessary varibales from Ray class due to cache problems
		/*
		static inline const __m256 rayBoxIntersectionTestSIMD(Ray& ray, const vec4s& p0, const vec4s& q0, const vec4s& p1, const vec4s& q1) {
			const __m256 p = _mm256_insertf128_ps(_mm256_castps128_ps256(p0), p1, 1);
			const __m256 q = _mm256_insertf128_ps(_mm256_castps128_ps256(q0), q1, 1);
			const __m256 v0 = _mm256_mul_ps(_mm256_sub_ps(p, ray.ar0), ray.ainvr1);
			const __m256 v1 = _mm256_mul_ps(_mm256_sub_ps(q, ray.ar0), ray.ainvr1);
			const __m256 min0 = _mm256_min_ps(v0, v1);
			const __m256 max0 = _mm256_max_ps(v0, v1);
			const __m256 min1 = _mm256_max_ps(min0, _mm256_permute_ps(min0, 0b01001010));
			const __m256 max1 = _mm256_min_ps(max0, _mm256_permute_ps(max0, 0b01001010));
			ray.avmin = _mm256_max_ps(min1, _mm256_permute_ps(min1, 0b11100001));
			return _mm256_min_ps(max1, _mm256_permute_ps(max1, 0b11100001));
		}*/


		
		//up to 22 instructions form 14, but doing 8 at a time
		static inline const __m256 rayBoxIntersectionTestAVX(OBVH::Ray& ray, const vec3avx& p, const vec3avx& q, __m256&  min) {
			const __m256 v0x = _mm256_mul_ps(_mm256_sub_ps(p.x, ray.r0.x), ray.inv.x);
			const __m256 v0y = _mm256_mul_ps(_mm256_sub_ps(p.y, ray.r0.y), ray.inv.y);
			const __m256 v0z = _mm256_mul_ps(_mm256_sub_ps(p.z, ray.r0.z), ray.inv.z);
			const __m256 v1x = _mm256_mul_ps(_mm256_sub_ps(q.x, ray.r0.x), ray.inv.x);
			const __m256 v1y = _mm256_mul_ps(_mm256_sub_ps(q.y, ray.r0.y), ray.inv.y);
			const __m256 v1z = _mm256_mul_ps(_mm256_sub_ps(q.z, ray.r0.z), ray.inv.z);

			const __m256 min0x = _mm256_min_ps(v0x, v1x);
			const __m256 min0y = _mm256_min_ps(v0y, v1y);
			const __m256 min0z = _mm256_min_ps(v0z, v1z);
			const __m256 max0x = _mm256_max_ps(v0x, v1x);
			const __m256 max0y = _mm256_max_ps(v0y, v1y);
			const __m256 max0z = _mm256_max_ps(v0z, v1z);

			min = _mm256_max_ps(min0x, _mm256_max_ps(min0y, min0z));
			return _mm256_min_ps(max0x, _mm256_min_ps(max0y, max0z));
		}
		/*
		//up to 22 instructions form 14, but doing 4 at a time
		static inline const __m128 rayBoxIntersectionTestSSE(Ray& ray, const vec3sse& p, const vec3sse& q) {
			const __m128 v0x = _mm_mul_ps(_mm_sub_ps(p.x, ray.r0s.x), ray.rinvs.x);
			const __m128 v0y = _mm_mul_ps(_mm_sub_ps(p.y, ray.r0s.y), ray.rinvs.y);
			const __m128 v0z = _mm_mul_ps(_mm_sub_ps(p.z, ray.r0s.z), ray.rinvs.z);
			const __m128 v1x = _mm_mul_ps(_mm_sub_ps(q.x, ray.r0s.x), ray.rinvs.x);
			const __m128 v1y = _mm_mul_ps(_mm_sub_ps(q.y, ray.r0s.y), ray.rinvs.y);
			const __m128 v1z = _mm_mul_ps(_mm_sub_ps(q.z, ray.r0s.z), ray.rinvs.z);

			const __m128 min0x = _mm_min_ps(v0x, v1x);
			const __m128 min0y = _mm_min_ps(v0y, v1y);
			const __m128 min0z = _mm_min_ps(v0z, v1z);
			const __m128 max0x = _mm_max_ps(v0x, v1x);
			const __m128 max0y = _mm_max_ps(v0y, v1y);
			const __m128 max0z = _mm_max_ps(v0z, v1z);

			ray.vmins = _mm_max_ps(min0x, _mm_max_ps(min0y, min0z));
			return _mm_min_ps(max0x, _mm_min_ps(max0y, max0z));
		}
		*/

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
		/*
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
		}*/

		static void projectedBox(const PolyOctree& octree, const View* view, vec4& pOut, vec4& qOut);
		static void projectedBox(const OBVH& bvh, const View* view, vec4& pOut, vec4& qOut);
		static void projectedBox(const PBVH& bvh, const View* view, vec4& pOut, vec4& qOut);

	};
}


#pragma once
namespace core {
	struct PBVH : public SIMD {
		struct Ray {
			vec3avx r0;
			vec3avx r1;
			vec3avx inv;
			vec4s plane;
			float rs;
			float d;
			int node;
		};

		struct innerNode : public SIMD {
			vec3avx p, q;
			int node[8];
			int nn;
			int threadPriority; //indices as result of priority changes
			int priority;
			int parent;
			int pos;

			innerNode() { memset(node, 0, sizeof(node)); nn = 0; priority = 0; parent = 0; pos = 0; }
			const float rayIntersectionT(PBVH::Ray& ray, OBVH& bvh);
		};

		struct leafNode : public SIMD {
			vec3avx p[4];
			vec4avx n[4];
			int np;
			int parent;
			int pos;

			void __vectorcall rayIntersection(PBVH::Ray& ray, const int& node) const;
		};

		buffer<innerNode> inner;
		buffer<leafNode> leaf;
		vec4s p, q;
		float radiusSquared;
		int pointCount = 0;

		inline void dispose() { inner.clear(); leaf.clear(); p = q = _mm_setzero_ps(); radiusSquared = 0.0f; pointCount = 0; }
		void build(const PointOctree& tree);
		const float rayIntersectionTIt(PBVH::Ray& ray, std::pair<int, float>* stack, int* priority);
		const float estimateRadius();
		void setRadius(const float& rad);
		void pointsPerBox();
	};
}

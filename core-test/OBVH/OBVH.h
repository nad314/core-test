#pragma once
namespace core {
	struct OBVH: public SIMD {

		struct Ray {
			vec3avx r0;
			vec3avx r1;
			vec3avx inv;
			vec4s sr0, sr1;
			vec4s plane;
			float d;
		};

		struct innerNode: public SIMD {
			vec3avx p, q;
			int node[8];
			int nn;
			int threadPriority; //indices as result of priority changes
			int reserved[2];

			innerNode() { memset(node, 0, sizeof(node)); nn = 0; memset(reserved, 0, sizeof(reserved)); }
			const float rayIntersectionT(OBVH::Ray& ray, OBVH& bvh);
		};

		struct leafNode: public SIMD {
			//we'll order it in 8point->8plane->8point->8plane fashion
			//buffer<vec3avx> points;
			
			vec3avx p0, p1, p2;
			vec4avx plane;
			int np;
			/*
			buffer<vec4s> points;
			buffer<vec4s> planes;*/
			const float rayIntersectionT(OBVH::Ray& ray, const OBVH& bvh) const;
		};

		buffer<innerNode> inner;
		buffer<leafNode> leaf;

		void build(const PolyOctree& tree);
	};
}

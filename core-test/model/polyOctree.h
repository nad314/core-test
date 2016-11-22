#pragma once
#include <queue>
namespace core {
	
	struct vec3avx : public SIMD {
		__m256 x;
		__m256 y;
		__m256 z;

		vec3avx() {}
		vec3avx(const float* xx, const float* yy, const float* zz) {
			x = _mm256_loadu_ps(xx);
			y = _mm256_loadu_ps(yy);
			z = _mm256_loadu_ps(zz);
		}
		vec3avx(const __m256& xx, const __m256& yy, const __m256& zz) :x(xx), y(yy), z(zz) {}
	};

	struct vec4avx : public SIMD {
		__m256 x;
		__m256 y;
		__m256 z;
		__m256 w;

		vec4avx() {}
		vec4avx(const float* xx, const float* yy, const float* zz, const float* ww) {
			x = _mm256_loadu_ps(xx);
			y = _mm256_loadu_ps(yy);
			z = _mm256_loadu_ps(zz);
			w = _mm256_loadu_ps(ww);
		}
		vec4avx(const __m256& xx, const __m256& yy, const __m256& zz, const __m256& ww) :x(xx), y(yy), z(zz), w(ww) {}
	};


	struct vec3sse : public SIMD {
		__m128 x;
		__m128 y;
		__m128 z;

		vec3sse() {}
		vec3sse(const float* xx, const float* yy, const float* zz) {
			x = _mm_load_ps(xx);
			y = _mm_load_ps(yy);
			z = _mm_load_ps(zz);
		}
		vec3sse(const __m128 xx, const __m128 yy, const __m128 zz) :x(xx), y(yy), z(zz) {}
	};
	

	struct Ray : public SIMD {
		vec4s sr0;
		vec4s sr1;
		vec4s sinvr1;
		float d;
		vec4s plane;
		__m256 ar0, ainvr1;

		Ray() {}
		Ray(vec4 v0, vec4 v1) { 
			const vec4 r0 = v0;
			vec4 r1 = v1;
			#define eps 1e-6
			if (fabs(r1.x) < eps)r1.x = eps;
			if (fabs(r1.y) < eps)r1.y = eps;
			if (fabs(r1.z) < eps)r1.z = eps;
			#undef eps
			const vec4 invr1 = vec4(1.0f) / r1; 
			d = 100.0f; 
			sr0 = r0;
			sr1 = r1;
			sinvr1 = invr1;
		}
	};

	class PolyOctree : public SIMD {
	public:
		struct Node : public SIMD {
			static const int maxPolys = 8;
			static const short maxDepth = 24;
			static const byte subtreeDepth = 4;
			static Node* lastNode;
			static int hitsPerBox;
			static int hitPerBoxCount;

			vec4s spp, sqq;
			Node* node[8];
			buffer<vec4s> points;
			buffer<vec4s> planes;
			byte depth;
			byte nnodes;
			bool hasNodes;

			Node() { for (byte i = 0; i < 8; ++i)node[i] = NULL; hasNodes = 0; }
			Node(vec4 pp, vec4 qq) :spp(pp), sqq(qq) { for (byte i = 0; i < 8; ++i)node[i] = NULL; hasNodes = 0;}
			~Node() { if(hasNodes)for (byte i = 0; i < 8; ++i) { delete node[i]; node[i] = NULL; } }

			void build(buffer<vec4>& buff);
			void sub();
			const float rayIntersectionT(Ray& ray);
			int count();
			void unlink();

			void cacheSort(Node* mem, int& pos, int depth);
			void multVecs();
			int countNodes();
			void expand();
			void shrinkNodes();
			void trunc();

			inline bool isEmpty() { return (hasNodes && nnodes == 0) || (!hasNodes && points.size()<1); }
			void reduceDepth() {
				--depth;
				if (hasNodes)
					for (int i = 0; i < nnodes; ++i)
						node[i]->reduceDepth();
			}

			inline const float px() const { return spp.m.m128_f32[0]; }
			inline const float py() const { return spp.m.m128_f32[1]; }
			inline const float pz() const { return spp.m.m128_f32[2]; }
			inline const float qx() const { return sqq.m.m128_f32[0]; }
			inline const float qy() const { return sqq.m.m128_f32[1]; }
			inline const float qz() const { return sqq.m.m128_f32[2]; }
		};

	public:
		int size;
		Node* root;
		void build(simdMesh& mesh);
		void cacheSort();
		vec4s rayIntersectionT(Ray& ray) const;
		//float rayIntersectionIterative(Ray& ray) const;

		PolyOctree() : root(NULL) {}
		~PolyOctree() { dispose(); }
		inline void dispose() { if(root)root->unlink(); delete root; root = NULL; }
	};
}

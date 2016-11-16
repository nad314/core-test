#pragma once
#include <queue>
namespace core {

	struct Ray : public SIMD {
		vec4 r0;
		vec4 r1;
		vec4 invr1;
		vec4s sr0;
		vec4s sr1;
		vec4s sinvr1;
		float d;
		vec4 plane;
		vec4 vmin, vmax;

		Ray() {}
		Ray(vec4 v0, vec4 v1) : r0(v0), r1(v1) { 
			#define eps 1e-6
			if (fabs(r1.x) < eps)r1.x = eps;
			if (fabs(r1.y) < eps)r1.y = eps;
			if (fabs(r1.z) < eps)r1.z = eps;
			#undef eps
			invr1 = vec4(1.0f) / r1; 
			d = 100.0f; 
			sr0 = r0;
			sr1 = r1;
			sinvr1 = invr1;
		}
	};

	class PolyOctree {
	public:
		struct Node : public SIMD {
			static const int maxPolys = 8;
			static const short maxDepth = 10;
			static const byte subtreeDepth = 4;
			static Node* lastNode;

			vec4 p, q;
			vec4 pp, qq;
			vec4s spp, sqq;
			Node* node[8];
			buffer<vec4> points;
			buffer<vec4> planes;
			byte depth;
			byte nnodes;
			bool hasNodes;

			Node() { for (byte i = 0; i < 8; ++i)node[i] = NULL; hasNodes = 0; }
			Node(vec4 pp, vec4 qq) :p(pp), q(qq) { for (byte i = 0; i < 8; ++i)node[i] = NULL; hasNodes = 0;}
			~Node() { if(hasNodes)for (byte i = 0; i < 8; ++i) { delete node[i]; node[i] = NULL; } }

			void build(buffer<vec4>& buff);
			void sub();
			float rayIntersectionT(Ray& ray);
			void bbox();
			int count();
			void unlink();

			void cacheSort(Node* mem, int& pos, int depth);
			void multVecs();
			int countNodes();
			void shrinkNodes();

			inline bool isEmpty() { return (hasNodes && nnodes == 0) || (!hasNodes && points.size()<1); }

		};

	public:
		Node* root;
		void build(simdMesh& mesh);
		void cacheSort();
		float rayIntersectionT(Ray& ray) const;

		PolyOctree() : root(NULL) {}
		~PolyOctree() { dispose(); }
		inline void dispose() { if(root)root->unlink(); delete root; root = NULL; }
	};
}

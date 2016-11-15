#pragma once
#include <queue>
namespace core {

	struct Ray {
		vec4 r0;
		vec4 r1;
		vec4 invr1;
		float d;
		vec4 plane;

		Ray(vec4 v0, vec4 v1) : r0(v0), r1(v1) { 
			vec4 rr1 = r1;
			#define eps 1e-6
			if (fabs(r1.x) < eps)r1.x = eps;
			if (fabs(r1.y) < eps)r1.y = eps;
			if (fabs(r1.z) < eps)r1.z = eps;
			#undef eps
			invr1 = vec4(1.0f) / r1; d = 100.0f; 
		}
	};

	class PolyOctree {
	public:
		struct Node {
			static const int maxPolys = 8;
			static const short maxDepth = 10;
			static const byte subtreeDepth = 5;
			static Node* lastNode;

			vec4 p, q;
			vec4 pp, qq;
			Node* node[8];
			byte depth;
			bool hasNodes;
			buffer<vec4> points;
			buffer<vec4> planes;

			Node() { for (byte i = 0; i < 8; ++i)node[i] = NULL; hasNodes = 0; }
			Node(vec4 pp, vec4 qq) :p(pp), q(qq) { for (byte i = 0; i < 8; ++i)node[i] = NULL; hasNodes = 0; }
			~Node() { for (byte i = 0; i < 8; ++i) { delete node[i]; node[i] = NULL; } }

			Node& operator = (const Node& n) {
				p = n.p;
				q = n.q;
				pp = n.pp;
				qq = n.qq;
				memcpy(node, n.node, sizeof(node));
				depth = n.depth;
				hasNodes = n.hasNodes;
				points = n.points;
				planes = n.planes;
				return *this;
			}

			void build(buffer<vec4>& buff);
			void sub();
			float rayIntersectionT(Ray& ray);
			void bbox();
			int count();
			void unlink();

			void cacheSort(Node* mem, int& pos, int depth);
			void multVecs();

		};

	public:
		Node* root;
		void build(simdMesh& mesh);
		void cacheSort();
		float rayIntersectionT(Ray& ray) const;

		PolyOctree() : root(NULL) {}
		~PolyOctree() { root->unlink(); delete[] root; root = NULL; }
	};
}

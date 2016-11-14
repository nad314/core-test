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
			invr1 = vec4(1.0f) / r1; d = 100.0f; 
		}
	};

	class PolyOctree {
	public:
		struct Node {
			static const int maxPolys = 16;
			static const short maxDepth = 8;
			static Node* lastNode;

			vec4 p, q;
			vec4 pp, qq;
			Node* node[8];
			Node* nodeBuff[8];
			short depth;
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
				memcpy(nodeBuff, n.nodeBuff, sizeof(node));
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

			inline void swapNodes(const Ray& ray) {
				if (ray.r0.x > q.x) { 
					std::swap(node[0], node[1]); 
					std::swap(node[2], node[3]); 
					std::swap(node[4], node[5]);
					std::swap(node[6], node[7]);
				}
				if (ray.r0.y > q.y) {
					std::swap(node[0], node[2]);
					std::swap(node[1], node[3]);
					std::swap(node[4], node[6]);
					std::swap(node[5], node[7]);
				}
				if (ray.r0.z > q.z) {
					std::swap(node[0], node[4]);
					std::swap(node[1], node[5]);
					std::swap(node[2], node[6]);
					std::swap(node[3], node[7]);
				}
			}

			int count();

			void unlink() {
				if(hasNodes)
					for (int i = 0; i < 8; ++i) {
						node[i]->unlink();
						node[i] = NULL;
					}
			}

			void cacheSort(Node* mem, int& pos, int depth) const;

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

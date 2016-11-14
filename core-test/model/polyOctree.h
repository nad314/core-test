#pragma once

namespace core {

	struct Ray {
		vec4 r0;
		vec4 r1;
		vec4 invr1;
		float d;
		vec4 plane;

		Ray(vec4 v0, vec4 v1) : r0(v0), r1(v1) { invr1 = vec4(1.0f) / r1; d = 100.0f; }
	};

	class PolyOctree {
	private:
		struct Node {
			static const int maxPolys = 16;
			static const short maxDepth = 8;

			vec4 p, q;
			vec4 pp, qq;
			Node* node[8];
			short depth;
			bool hasNodes;
			buffer<vec4> points;
			buffer<vec4> planes;

			Node() { for (byte i = 0; i < 8; ++i)node[i] = NULL; hasNodes = 0; }
			Node(vec4 pp, vec4 qq) :p(pp), q(qq) { for (byte i = 0; i < 8; ++i)node[i] = NULL; hasNodes = 0; }
			~Node() { for (byte i = 0; i < 8; ++i) { delete node[i]; node[i] = NULL; } }

			void build(buffer<vec4>& buff);
			void sub();
			float rayIntersectionT(Ray& ray) const;
			void bbox();
		};

	public:
		Node root;
		void build(simdMesh& mesh);
	};
}

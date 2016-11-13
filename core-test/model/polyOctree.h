#pragma once

namespace core {
	class PolyOctree {
	private:
		struct Node {
			static const int maxPolys = 32;
			static const short maxDepth = 8;

			vec4 p, q;
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
			float rayIntersectionT(const vec4& r0, const vec4& r1) const;
		};

	public:
		Node root;
		void build(simdMesh& mesh);
	};
}

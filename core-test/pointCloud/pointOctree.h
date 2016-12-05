#pragma once

namespace core {
	struct PointOctree {
		struct Node : public SIMD {
			static const int maxPoints = 32;
			static const short maxDepth = 32;
			static const byte subtreeDepth = 4;

			vec4s spp, sqq;
			Node* node[8];
			buffer<vec4s> points;
			buffer<vec4s> planes;
			byte depth;
			byte nnodes;
			bool hasNodes;

			Node() { for (byte i = 0; i < 8; ++i)node[i] = NULL; hasNodes = 0; }
			Node(vec4 pp, vec4 qq) :spp(pp), sqq(qq) { for (byte i = 0; i < 8; ++i)node[i] = NULL; hasNodes = 0; }
			~Node() { if (hasNodes)for (byte i = 0; i < 8; ++i) { delete node[i]; node[i] = NULL; } points.clear(); planes.clear(); }

			void build(buffer<vec4>& buff, buffer<vec4>& nbuff);
			void sub();
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
		void build(PointCloud& cloud);
		void cacheSort();

		PointOctree() : root(NULL) {}
		~PointOctree() { dispose(); }
		inline void dispose() { if (root)root->unlink(); delete[] root; root = NULL; size = 0; }
	};
}

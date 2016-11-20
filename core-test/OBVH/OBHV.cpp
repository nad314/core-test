#include <main>
namespace core {
	void OBVH::build(const PolyOctree& tree) {
		int n = 0;
		buffer<int> leafCount;
		int lc = 0;
		for (int i = 0; i < tree.size; ++i) {
			if (tree.root[i].hasNodes)
				++n;
			else ++lc;
			leafCount.push_back(lc);
		}
		inner.reserve(n);
		leaf.reserve(tree.size - n);

		float px[8], py[8], pz[8], qx[8], qy[8], qz[8];
		float pp[13][8];

		for (int i = 0; i < tree.size; ++i)
			if (tree.root[i].hasNodes) {
				int& c = inner.count();
				const int llc = leafCount[(int)(&tree.root[i]-tree.root)];
				memset(px, 0, sizeof(px));
				memset(py, 0, sizeof(py));
				memset(pz, 0, sizeof(pz));
				memset(qx, 0, sizeof(qx));
				memset(qy, 0, sizeof(qy));
				memset(qz, 0, sizeof(qz));
				for (int j = 0; j < tree.root[i].nnodes; ++j) {
					if (tree.root[i].node[j]->hasNodes)
						inner[c].node[j] = c + (int)(tree.root[i].node[j] - &tree.root[i]) - (leafCount[(int)(tree.root[i].node[j] - tree.root)] - llc);
					else
						inner[c].node[j] = -(leafCount[(int)(tree.root[i].node[j] - tree.root)] - 1);
					px[j] = tree.root[i].node[j]->px();
					py[j] = tree.root[i].node[j]->py();
					pz[j] = tree.root[i].node[j]->pz();
					qx[j] = tree.root[i].node[j]->qx();
					qy[j] = tree.root[i].node[j]->qy();
					qz[j] = tree.root[i].node[j]->qz();
				}
				inner[c].nn = tree.root[i].nnodes;
				inner[c].p = vec3avx(px, py, pz);
				inner[c].q = vec3avx(qx, qy, qz);
				inner[c].threadPriority = 0x76543210;
				//inner[c].threadPriority = 0x00779350;
				++c;
			}
			else {
				
				for (int j = 0; j < 13; ++j)
					for (int k = 0; k < 8; ++k)
						pp[j][k] = 1.0f;
				for (int j = 0; j < std::min(8, tree.root[i].planes.count()); j++) {
					pp[0][j] = tree.root[i].points[j*3].m.m128_f32[0];
					pp[1][j] = tree.root[i].points[j*3].m.m128_f32[1];
					pp[2][j] = tree.root[i].points[j*3].m.m128_f32[2];
					pp[3][j] = tree.root[i].points[j*3 + 1].m.m128_f32[0];
					pp[4][j] = tree.root[i].points[j*3 + 1].m.m128_f32[1];
					pp[5][j] = tree.root[i].points[j*3 + 1].m.m128_f32[2];
					pp[6][j] = tree.root[i].points[j*3 + 2].m.m128_f32[0];
					pp[7][j] = tree.root[i].points[j*3 + 2].m.m128_f32[1];
					pp[8][j] = tree.root[i].points[j*3 + 2].m.m128_f32[2];

					pp[9][j] = tree.root[i].planes[j].m.m128_f32[0];
					pp[10][j] = tree.root[i].planes[j].m.m128_f32[1];
					pp[11][j] = tree.root[i].planes[j].m.m128_f32[2];
					pp[12][j] = tree.root[i].planes[j].m.m128_f32[3];
				}

				
				int& c = leaf.count();

				leaf[c].p0 = vec3avx(pp[0], pp[1], pp[2]);
				leaf[c].p1 = vec3avx(pp[3], pp[4], pp[5]);
				leaf[c].p2 = vec3avx(pp[6], pp[7], pp[8]);
				leaf[c].plane = vec4avx(pp[9], pp[10], pp[11], pp[12]);
				leaf[c].np = tree.root[i].planes.count();

				/*
				memset(px, 0, sizeof(px));
				memset(py, 0, sizeof(py));
				memset(pz, 0, sizeof(pz));
				memset(px, 0, sizeof(px));
				memset(py, 0, sizeof(py));
				memset(pz, 0, sizeof(pz));
				memset(qx, 0, sizeof(qx));
				memset(qy, 0, sizeof(qy));
				memset(qz, 0, sizeof(qz));
				px[0] = tree.root[i].px();
				py[0] = tree.root[i].py();
				pz[0] = tree.root[i].pz();
				qx[0] = tree.root[i].qx();
				qy[0] = tree.root[i].qy();
				qz[0] = tree.root[i].qz();
				leaf[c].p0 = vec3avx(px, py, pz);
				leaf[c].p1 = vec3avx(qx, qy, qz);*/

				++c;
			}
	}
}

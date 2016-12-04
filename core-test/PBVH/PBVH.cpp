#include <main>
namespace core {
	void PBVH::build(const PointOctree& tree) {
		if (!tree.root)
			return;
		radiusSquared = 0.0f;
		p = tree.root[0].spp;
		q = tree.root[0].sqq;
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
		float pp[8][32];
		inner[0].parent = 0;
		for (int i = 0; i < tree.size; ++i)
			if (tree.root[i].hasNodes) {
				int& c = inner.count();
				const int llc = leafCount[(int)(&tree.root[i] - tree.root)];
				memset(px, 0, sizeof(px));
				memset(py, 0, sizeof(py));
				memset(pz, 0, sizeof(pz));
				memset(qx, 0, sizeof(qx));
				memset(qy, 0, sizeof(qy));
				memset(qz, 0, sizeof(qz));
				for (int j = 0; j < tree.root[i].nnodes; ++j) {
					if (tree.root[i].node[j]->hasNodes) {
						inner[c].node[j] = c + (int)(tree.root[i].node[j] - &tree.root[i]) - (leafCount[(int)(tree.root[i].node[j] - tree.root)] - llc);
						inner[inner[c].node[j]].parent = c;
						inner[inner[c].node[j]].pos = j;
					}
					else {
						inner[c].node[j] = -(leafCount[(int)(tree.root[i].node[j] - tree.root)] - 1);
						leaf[-inner[c].node[j]].parent = c;
						leaf[-inner[c].node[j]].pos = j;
					}
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

				for (int j = 0; j < 8; ++j)
					for (int k = 0; k < 32; ++k)
						pp[j][k] = 1.0f;
				for (int j = 0; j < std::min(PointOctree::Node::maxPoints, tree.root[i].points.count()); j++) {
					pp[0][j] = tree.root[i].points[j].m.m128_f32[0];
					pp[1][j] = tree.root[i].points[j].m.m128_f32[1];
					pp[2][j] = tree.root[i].points[j].m.m128_f32[2];
					pp[3][j] = tree.root[i].planes[j].m.m128_f32[0];
					pp[4][j] = tree.root[i].planes[j].m.m128_f32[1];
					pp[5][j] = tree.root[i].planes[j].m.m128_f32[2];
					pp[6][j] = tree.root[i].planes[j].m.m128_f32[3];

				}


				int& c = leaf.count();
				leaf[c].np = std::min(PointOctree::Node::maxPoints, tree.root[i].points.count());
				for (int j = 0; j < PointOctree::Node::maxPoints/8; ++j) {
					leaf[c].p[j] = vec3avx(pp[0] + j * 8, pp[1] + j * 8, pp[2] + j * 8);
					leaf[c].n[j] = vec4avx(pp[3] + j * 8, pp[4] + j * 8, pp[5] + j * 8, pp[6] + j * 8);
				}
				++c;
			}
	}

	const float PBVH::estimateRadius() {
		float rad = 0.0f;
		float count = 0.0f;
		for (auto& node : leaf) {
			for (int i = 0; i < node.np; ++i) {
				const int a = i / 8;
				const int b = i % 8;
				vec3 point = vec3(node.p[a].x.m256_f32[b], node.p[a].y.m256_f32[b], node.p[a].z.m256_f32[b]);
				float dmin = -1.0f;
				for (int j = 0; j < node.np; ++j) {
					if (j == i)
						continue;
					const int a = j / 8;
					const int b = j % 8;
					vec3 point2 = vec3(node.p[a].x.m256_f32[b], node.p[a].y.m256_f32[b], node.p[a].z.m256_f32[b]);
					const float dist = (point2 - point).length();
					if (dist > 0.0f && (dmin < 0.0f || dist < dmin)) {
						dmin = dist;
					}
				}
				if (dmin > 0.0f) {
					count += 1.0f;
					rad += dmin;
				}
			}
		}
		rad = (float)((double)rad / count);
		core::Debug::print("Estimated %f radius\n", rad);
		return rad;
	}

	void PBVH::setRadius(const float& rad) {
		const float oldRad = sqrt(radiusSquared);
		const __m256 omr = _mm256_set1_ps(oldRad);
		for (auto&i : inner) {
			i.p.x = _mm256_add_ps(i.p.x, omr);
			i.p.y = _mm256_add_ps(i.p.y, omr);
			i.p.z = _mm256_add_ps(i.p.z, omr);
			i.q.x = _mm256_sub_ps(i.q.x, omr);
			i.q.y = _mm256_sub_ps(i.q.y, omr);
			i.q.z = _mm256_sub_ps(i.q.z, omr);
		}
		radiusSquared = rad*rad;
		const __m256 mr = _mm256_set1_ps(rad);
		for (auto&i : inner) {
			i.p.x = _mm256_sub_ps(i.p.x, mr);
			i.p.y = _mm256_sub_ps(i.p.y, mr);
			i.p.z = _mm256_sub_ps(i.p.z, mr);
			i.q.x = _mm256_add_ps(i.q.x, mr);
			i.q.y = _mm256_add_ps(i.q.y, mr);
			i.q.z = _mm256_add_ps(i.q.z, mr);
		}
	}

	void PBVH::pointsPerBox() {
		float points = 0.0f;
		float count = 0.0f;
		for (auto& i : leaf) {
			points += i.np;
			++count;
		}
		core::Debug::print("Average Points Per Box: %.2f\n", points / count);
	}
}

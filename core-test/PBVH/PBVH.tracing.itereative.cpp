#include <main>

namespace core {
	void __vectorcall PBVH::leafNode::rayIntersection(PBVH::Ray& ray, const int& node) const {
		for (int j = 0; j < (np-1)/8+1; ++j) {
			//ray sphere intersection
			const __m256 lx = _mm256_sub_ps(p[j].x, ray.r0.x);
			const __m256 ly = _mm256_sub_ps(p[j].y, ray.r0.y);
			const __m256 lz = _mm256_sub_ps(p[j].z, ray.r0.z);
			const __m256 tca = _mm256_add_ps(_mm256_add_ps(
				_mm256_mul_ps(lx, ray.r1.x), 
				_mm256_mul_ps(ly, ray.r1.y)), 
				_mm256_mul_ps(lz, ray.r1.z));
			const __m256 ldotl = _mm256_add_ps(_mm256_add_ps(
				_mm256_mul_ps(lx, lx),
				_mm256_mul_ps(ly, ly)),
				_mm256_mul_ps(lz, lz));
			const __m256 d2 = _mm256_sub_ps(ldotl, _mm256_mul_ps(tca, tca));
			const __m256 thc = _mm256_sqrt_ps(_mm256_sub_ps(_mm256_set1_ps(ray.rs), d2));
			const __m256 dist = _mm256_sub_ps(tca, thc);
			
			/*
			//ray plane distance
			const __m256 dx = _mm256_mul_ps(ray.r1.x, n[j].x);
			const __m256 dy = _mm256_mul_ps(ray.r1.y, n[j].y);
			const __m256 dz = _mm256_mul_ps(ray.r1.z, n[j].z);
			const __m256 dt = _mm256_add_ps(_mm256_add_ps(dx, dy), dz);

			const __m256 d1x = _mm256_mul_ps(ray.r0.x, n[j].x);
			const __m256 d1y = _mm256_mul_ps(ray.r0.y, n[j].y);
			const __m256 d1z = _mm256_mul_ps(ray.r0.z, n[j].z);
			const __m256 d1t = _mm256_add_ps(_mm256_add_ps(d1x, d1y), d1z);

			const __m256 dist = _mm256_div_ps(_mm256_sub_ps(_mm256_setzero_ps(), _mm256_add_ps(n[j].w, d1t)), dt);

			//point in circle test
			const __m256 px = _mm256_add_ps(ray.r0.x, _mm256_mul_ps(ray.r1.x, dist));
			const __m256 py = _mm256_add_ps(ray.r0.y, _mm256_mul_ps(ray.r1.y, dist));
			const __m256 pz = _mm256_add_ps(ray.r0.z, _mm256_mul_ps(ray.r1.z, dist));
			const __m256 pdx = _mm256_sub_ps(p[j].x, px);
			const __m256 pdy = _mm256_sub_ps(p[j].y, py);
			const __m256 pdz = _mm256_sub_ps(p[j].z, pz);
			const __m256 pddot = _mm256_add_ps(_mm256_mul_ps(pdx, pdx), _mm256_add_ps(_mm256_mul_ps(pdy, pdy), _mm256_mul_ps(pdz, pdz)));
			*/
			const int to = std::min(np - j * 8, 8);
			for (int i = 0; i < to; ++i) {
				
				if (tca.m256_f32[i] < 0 || d2.m256_f32[i] > ray.rs || ray.d <= dist.m256_f32[i])
					continue;
				
				/*
				if (dist.m256_f32[i] < 0  || ray.d <= dist.m256_f32[i] || pddot.m256_f32[i] > ray.rs)
					continue;
				*/	
				ray.d = dist.m256_f32[i];
				ray.node = node;
				ray.plane = vec4s(vec4(n[j].x.m256_f32[i], n[j].y.m256_f32[i], n[j].z.m256_f32[i], n[j].w.m256_f32[i]));
			}
		}
	}

	const float __vectorcall PBVH::rayIntersectionTIt(PBVH::Ray& ray, std::pair<int, float>* stack, int* priority) {
		typedef std::pair<int, float> SE;
		SE* st = stack;
		*st++ = SE(0, -1.0f);
		SE* current = stack;
		ray.d = 100.0f;
		ray.node = 0;
		ray.rs = radiusSquared;

		while (st != stack) {
			//test if distance to box is greater than previously found distance
			if (ray.d <= current->second) {
				--st;
				current = st - 1;
				continue;
			}

			innerNode& n = inner[current->first];
			const int pri = priority[current->first];

			/*
			if (n.node[pri] > 0)
				_mm_prefetch(reinterpret_cast<const char*>(&inner[n.node[pri]]), _MM_HINT_T2);
			else 				
				_mm_prefetch(reinterpret_cast<const char*>(&leaf[n.node[-pri]]), _MM_HINT_T2);
				*/

			//ray box intersection test
			const __m256 v0x = _mm256_mul_ps(_mm256_sub_ps(n.p.x, ray.r0.x), ray.inv.x);
			const __m256 v1x = _mm256_mul_ps(_mm256_sub_ps(n.q.x, ray.r0.x), ray.inv.x);
			const __m256 v0y = _mm256_mul_ps(_mm256_sub_ps(n.p.y, ray.r0.y), ray.inv.y);
			const __m256 v1y = _mm256_mul_ps(_mm256_sub_ps(n.q.y, ray.r0.y), ray.inv.y);
			const __m256 v0z = _mm256_mul_ps(_mm256_sub_ps(n.p.z, ray.r0.z), ray.inv.z);
			const __m256 v1z = _mm256_mul_ps(_mm256_sub_ps(n.q.z, ray.r0.z), ray.inv.z);

			const __m256 min0x = _mm256_min_ps(v0x, v1x);
			const __m256 max0x = _mm256_max_ps(v0x, v1x);
			const __m256 min0y = _mm256_min_ps(v0y, v1y);
			const __m256 max0y = _mm256_max_ps(v0y, v1y);
			const __m256 min0z = _mm256_min_ps(v0z, v1z);
			const __m256 max0z = _mm256_max_ps(v0z, v1z);

			const __m256 min = _mm256_max_ps(min0x, _mm256_max_ps(min0y, min0z));
			const __m256 max = _mm256_min_ps(max0x, _mm256_min_ps(max0y, max0z));

			--st; //pop
				  //do until 0th element so that there's less branching
			for (int m = 7; m > 0; --m) {
				const int i = (m == pri) ? 0 : m;
				if (max.m256_f32[i] <= min.m256_f32[i] || max.m256_f32[i] < 0.0f)
					continue;

				if (n.node[i] > 0)
					*st++ = SE(n.node[i], min.m256_f32[i]);
				else
					leaf[-n.node[i]].rayIntersection(ray, n.node[i]);
				//if (ray.d > min.m256_f32[i]) { ray.d = min.m256_f32[i]; ray.node = n.node[i]; }
			}
			//do the 0th element
			const int i = pri;
			if (max.m256_f32[i] > min.m256_f32[i] && max.m256_f32[i] >= 0.0f) {
				if (n.node[i] > 0)
					*st++ = SE(n.node[i], min.m256_f32[i]);
				else
					leaf[-n.node[i]].rayIntersection(ray, n.node[i]);
				//if (ray.d > min.m256_f32[i]) { ray.d = min.m256_f32[i]; ray.node = n.node[i]; }
			}
			current = st - 1;
		}

		if (ray.node < 0) {
			int node = leaf[-ray.node].parent;
			int pos = leaf[-ray.node].pos;
			do {
				priority[node] = pos;
				innerNode& n = inner[node];
				node = n.parent;
				pos = n.pos;
			} while (node > 0);
			priority[0] = pos;
			return ray.d;
		}
		return -1.0f;
	}
}

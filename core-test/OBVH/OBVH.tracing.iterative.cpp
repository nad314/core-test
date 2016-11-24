#include <main>

namespace core {
	void OBVH::leafNode::rayIntersection(OBVH::Ray& ray, const int& node) const {
		//ray plane intersection distance
		const __m256 dx = _mm256_mul_ps(ray.r1.x, plane.x);
		const __m256 dy = _mm256_mul_ps(ray.r1.y, plane.y);
		const __m256 dz = _mm256_mul_ps(ray.r1.z, plane.z);
		const __m256 dt = _mm256_add_ps(_mm256_add_ps(dx, dy), dz);

		const __m256 d1x = _mm256_mul_ps(ray.r0.x, plane.x);
		const __m256 d1y = _mm256_mul_ps(ray.r0.y, plane.y);
		const __m256 d1z = _mm256_mul_ps(ray.r0.z, plane.z);
		const __m256 d1t = _mm256_add_ps(_mm256_add_ps(d1x, d1y), d1z);

		const __m256 dist = _mm256_div_ps(_mm256_sub_ps(_mm256_setzero_ps(), _mm256_add_ps(plane.w, d1t)), dt);

		//point in triangle test
		const __m256 pointx = _mm256_add_ps(ray.r0.x, _mm256_mul_ps(ray.r1.x, dist));
		const __m256 pointy = _mm256_add_ps(ray.r0.y, _mm256_mul_ps(ray.r1.y, dist));
		const __m256 pointz = _mm256_add_ps(ray.r0.z, _mm256_mul_ps(ray.r1.z, dist));

		const __m256 vpx = _mm256_sub_ps(pointx, p0.x);
		const __m256 vpy = _mm256_sub_ps(pointy, p0.y);
		const __m256 vpz = _mm256_sub_ps(pointz, p0.z);
		const __m256 D = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(vpx, p1.x), _mm256_mul_ps(vpy, p1.y)), _mm256_mul_ps(vpz, p1.z));
		const __m256 e = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(vpx, p2.x), _mm256_mul_ps(vpy, p2.y)), _mm256_mul_ps(vpz, p2.z));

		const __m256 ac_bb = _mm256_sub_ps(_mm256_mul_ps(a, c), _mm256_mul_ps(b, b));
		const __m256 x = _mm256_sub_ps(_mm256_mul_ps(D, c), _mm256_mul_ps(e, b));
		const __m256 y = _mm256_sub_ps(_mm256_mul_ps(e, a), _mm256_mul_ps(D, b));
		const __m256 z = _mm256_sub_ps(_mm256_add_ps(x, y), ac_bb);

		const __m256 mm_min = _mm256_min_ps(dist, _mm256_min_ps(x, y));
		const __m256 mm_min2 = _mm256_min_ps(mm_min, _mm256_sub_ps(_mm256_setzero_ps(), z));

		for (int i = 0; i < np; ++i) {
			if (mm_min2.m256_f32[i]<0 || ray.d <= dist.m256_f32[i])
				continue;
			ray.d = dist.m256_f32[i];
			ray.node = node;
			ray.plane = vec4(plane.x.m256_f32[i], plane.y.m256_f32[i], plane.z.m256_f32[i], plane.w.m256_f32[i]);
		}
	}

	const float OBVH::rayIntersectionTIt(OBVH::Ray& ray, std::pair<int, float>* stack, int* priority) {
		typedef std::pair<int, float> SE;
		SE* st = stack;
		*st++ = SE(0, -1.0f);
		SE* current = stack;
		ray.d = 100.0f;
		ray.node = 0;

		while (st != stack) {
			//test if distance to box is greater than previously found distance
			if (ray.d <= current->second) {
				--st;
				current = st - 1;
				continue;
			}

			innerNode& n = inner[current->first];
			const int pri = priority[current->first];
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
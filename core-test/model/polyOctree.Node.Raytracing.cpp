#include <main>
namespace core {

	vec4s PolyOctree::rayIntersectionT(Ray& ray) const {/*
		Renderer::rayBoxIntersectionTestSIMD(ray, root->spp, root->sqq);
		if (_mm_comilt_ss(ray.svmin, _mm_setzero_ps()) || _mm_comilt_ss(_mm_set1_ps(ray.d), ray.svmin))
			return -1.0f;
		return root->rayIntersectionT(ray);
		*/
		return -1.0f;
	}

	const float PolyOctree::Node::rayIntersectionT(Ray& ray) {
		if (hasNodes) {
			float d(-1.0f);
			for (int i = 0; i < nnodes - (nnodes&1); i+=2) {
				const __m256 p = _mm256_insertf128_ps(_mm256_castps128_ps256(node[i]->spp), node[i+1]->spp, 1);
				const __m256 q = _mm256_insertf128_ps(_mm256_castps128_ps256(node[i]->sqq), node[i+1]->sqq, 1);
				const __m256 v0 = _mm256_mul_ps(_mm256_sub_ps(p, ray.ar0), ray.ainvr1);
				const __m256 v1 = _mm256_mul_ps(_mm256_sub_ps(q, ray.ar0), ray.ainvr1);
				const __m256 min0 = _mm256_min_ps(v0, v1);
				const __m256 max0 = _mm256_max_ps(v0, v1);
				const __m256 min1 = _mm256_max_ps(min0, _mm256_permute_ps(min0, 0b01001010));
				const __m256 max1 = _mm256_min_ps(max0, _mm256_permute_ps(max0, 0b01001010));
				const __m256 avmin = _mm256_max_ps(min1, _mm256_permute_ps(min1, 0b11100001));
				const __m256 svmax = _mm256_min_ps(max1, _mm256_permute_ps(max1, 0b11100001));

				if (avmin.m256_f32[0] < svmax.m256_f32[0] &&
					0.0f < svmax.m256_f32[0] &&
					avmin.m256_f32[0] < ray.d) {

					const float dist = node[i]->rayIntersectionT(ray);
					if (dist > 0 && (dist < d || d < 0.0f)) {
						ray.d = d = dist;
						if (i > 0) {
							const Node const* tmp = node[0];
							node[0] = node[i];
							node[i] = node[1];
							node[1] = const_cast<Node*>(tmp);
						}
					}
				}
				if (avmin.m256_f32[4] < svmax.m256_f32[4] &&
					0.0f < svmax.m256_f32[4] &&
					avmin.m256_f32[4] < ray.d) {

					const float dist = node[i+1]->rayIntersectionT(ray);
					if (dist > 0 && (dist < d || d < 0.0f)) {
						ray.d = d = dist;
						const Node const* tmp = node[0];
						node[0] = node[i+1];
						node[i+1] = node[1];
						node[1] = const_cast<Node*>(tmp);
					}
				}

			}
			if (nnodes & 1) {
				const int i = nnodes - 1;
				//const vec4s svmax = Renderer::rayBoxIntersectionTestSIMD(ray, node[i]->spp, node[i]->sqq);

				const vec4s v0 = (node[i]->spp - ray.sr0)*ray.sinvr1;
				const vec4s v1 = (node[i]->sqq - ray.sr0)*ray.sinvr1;
				const vec4s min0 = _mm_min_ps(v0, v1);
				const vec4s max0 = _mm_max_ps(v0, v1);
				const vec4s min1 = _mm_max_ps(min0, _mm_permute_ps(min0, 0b01001010));
				const vec4s max1 = _mm_min_ps(max0, _mm_permute_ps(max0, 0b01001010));
				const vec4s svmin = _mm_max_ss(min1, _mm_permute_ps(min1, 0b11100001));
				const vec4s svmax = _mm_min_ss(max1, _mm_permute_ps(max1, 0b11100001));


				if (_mm_comige_ss(svmin, svmax) ||
					_mm_comige_ss(_mm_setzero_ps(), svmax) ||
					_mm_comige_ss(svmin, _mm_set1_ps(ray.d)))
					return d;

				const float dist = node[i]->rayIntersectionT(ray);
				if (dist < 0 || (dist > d && d > 0.0f))
					return d;

				ray.d = d = dist;
				const Node const* tmp = node[0];
				node[0] = node[i];
				node[i] = node[1];
				node[1] = const_cast<Node*>(tmp);
			}
			return d;

		}
		else {
			/*
			if (points.size() > 0)
				return ray.d = _mm_cvtss_f32(ray.svmin);
			return -1.0f;
			*/
			
			vec4s d(-1.0f);
			for (int i = 0, j = 0; i < points.size(); i += 3, ++j) {
				const vec4s dist = SSE::rayPlaneT(ray.sr0, ray.sr1, planes[j]);
				if ((_mm_comige_ss(dist, d) && _mm_comige_ss(d, _mm_setzero_ps())) ||
					_mm_comilt_ss(dist, _mm_setzero_ps()) ||
					SSE::pointInTriangle(ray.sr0 + ray.sr1*dist, planes.at(j), points.at(i), points.at(i + 1), points.at(i + 2)))
					continue;
				d = dist;
				ray.d = _mm_cvtss_f32(d);
				ray.plane = planes[j];
			}
			return _mm_cvtss_f32(d);

		}
	}
}
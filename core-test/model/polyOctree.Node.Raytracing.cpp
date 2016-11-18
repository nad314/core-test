#include <main>
namespace core {

	vec4s PolyOctree::rayIntersectionT(Ray& ray) const {
		Renderer::rayBoxIntersectionTestSIMD(ray, root->spp, root->sqq);
		if (ray.vmin.x < 0.0f || ray.vmin.x>ray.d)
			return -1.0f;
		return root->rayIntersectionT(ray);
	}

#define pop() if(stack.count()<=1) return (ray.d = e.d); else {stackE& se = stack[stack.size()-2]; se.r = e.r; se.jb = 1; stack.pop(); continue;}
#define cont() {++e.i; continue;}

	//failed experiment
	/*
	float PolyOctree::rayIntersectionIterative(Ray& ray)const {
		Renderer::rayBoxIntersectionTestSIMD(ray, root->spp, root->sqq);
		if (ray.vmin.x < 0.0f || ray.vmin.x>ray.d)
			return -1.0f;

		struct stackE {
			Node* n;
			byte i;
			float r;
			float d;
			bool jb;
			stackE() {}
			stackE(Node* nn) : n(nn), i(0), r(-1.0f), d(-1.0f), jb(0) {}
		};

		buffer<stackE> stack;
		stack.reserve(Node::maxDepth);
		stack.push_back(stackE(root));

		char s[256];

		ray.d = 100.0f;

		while (stack.size()) {
			stackE& e = stack[stack.size() - 1];
			if (e.n->hasNodes) {
				if (e.jb) {
					if (e.r > 0.0f && (e.r < e.d || e.d < 0.0f)) {
						ray.d = e.d = e.r;
						if (e.i > 0) std::swap(e.n->node[e.i - 1], e.n->node[e.i]);
					}
					e.jb = 0;
					++e.i;
				}
				if (e.i >= e.n->nnodes) {
					e.r = e.d;
					pop();
				}
				Node& n = *e.n->node[e.i];
				Renderer::rayBoxIntersectionTestSIMD2(ray, n.spp, n.sqq);
				if (_mm_comige_ss(ray.svmin, ray.svmax) ||
					_mm_comige_ss(_mm_setzero_ps(), ray.svmax) ||
					_mm_comige_ss(ray.svmin, _mm_set1_ps(ray.d)))
					cont();
				stack.push_back(stackE(&n));
				continue;
			}
			else {
				if (e.n->points.size() > 0) {
					Renderer::rayBoxIntersectionTestSIMD(ray, e.n->spp, e.n->sqq);
					e.r = e.d =  ray.d = ray.vmin.x;
					pop();
				}
				e.r = -1.0f;
				pop();
			}
		}
		return -1.0f;
	}
	*/
	const float PolyOctree::Node::rayIntersectionT(Ray& ray) {
		float d = -1.0f;
		if (hasNodes) {
			int ind = 0;
			for (byte i = 0; i < nnodes; ++i) {
				/*
				Renderer::raySpehereIntersectionSIMD(ray, node[i]->sc, node[i]->sr);
				if (_mm_comige_ss(_mm_setzero_ps(), ray.svmin) ||
					_mm_comige_ss(ray.svmin, _mm_set1_ps(ray.d)))
					continue;*/
				Renderer::rayBoxIntersectionTestSIMD2(ray, node[i]->spp, node[i]->sqq);
				if (_mm_comige_ss(ray.svmin, ray.svmax) ||
					_mm_comige_ss(_mm_setzero_ps(), ray.svmax) ||
					_mm_comige_ss(ray.svmin, _mm_set1_ps(ray.d)))
					continue;
				const float dist = node[i]->rayIntersectionT(ray);
				//if (_mm_comigt_ss(dist, _mm_setzero_ps()) && (_mm_comilt_ss(dist, d) || _mm_comilt_ss(d, _mm_setzero_ps()))) {
				if (dist > 0 && (dist < d || d < 0.0f)) {
					ray.d = d = dist;
					if (i > 0) std::swap(node[i - 1], node[i]);
				}
			}
			return d;
		}
		else {
			/*
			if (points.size() > 0) {
				//Renderer::raySpehereIntersectionSIMD(ray, sc, sr);
				//Renderer::rayBoxIntersectionTestSIMD(ray, spp, sqq);
				ray.svmin.store(ray.vmin);
				return ray.d = ray.vmin.x;
			}
			return -1.0f;
			*/
			d = std::min(d, ray.d);
			const int ls = points.size();
			for (int i = 0, j = 0; i < ls; i += 3, ++j) {
				const vec4s dist = SSE::rayPlaneT(ray.sr0, ray.sr1, planes[j]);
				if (_mm_comigt_ss(dist, _mm_setzero_ps()) && (_mm_comilt_ss(dist, _mm_set1_ps(d)) || _mm_comilt_ss(_mm_set1_ps(d), _mm_setzero_ps()))) {
					if (SSE::pointInTriangle(planes[j], ray.sr0 + ray.sr1*dist, points[i], points[i + 1], points[i + 2])) {
						_mm_store1_ps(&d, dist);
						ray.d = d;
						ray.plane = planes[j];
					}
				}
			}
			return d;
			
		}
	}
}
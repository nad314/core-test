#include <main>
namespace core {

	vec4s PolyOctree::rayIntersectionT(Ray& ray) const {
		Renderer::rayBoxIntersectionTestSIMD(ray, root->spp, root->sqq);
		if (_mm_comilt_ss(ray.svmin, _mm_setzero_ps()) || _mm_comilt_ss(_mm_set1_ps(ray.d), ray.svmin))
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
		if (hasNodes) {
			float d(-1.0f);
			for (int i = 0; i < nnodes; ++i) {
				const vec4s svmax = Renderer::rayBoxIntersectionTestSIMD(ray, node[i]->spp, node[i]->sqq);
				if (_mm_comige_ss(ray.svmin, svmax) ||
					_mm_comige_ss(_mm_setzero_ps(), svmax) ||
					_mm_comige_ss(ray.svmin, _mm_set1_ps(ray.d)))
					continue;

				const float dist = node[i]->rayIntersectionT(ray);
				if (dist > 0 && (dist < d || d < 0.0f)) {
					ray.d = d = dist;
					if (i > 0) {
						std::swap(node[1], node[i]);
						std::swap(node[0], node[1]);
					}
				}
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
				if ((_mm_comilt_ss(dist, d) || _mm_comilt_ss(d, _mm_setzero_ps())) &&
					_mm_comige_ss(dist, _mm_setzero_ps()) &&
					SSE::pointInTriangle(ray.sr0 + ray.sr1*dist, planes.at(j), points.at(i), points.at(i + 1), points.at(i + 2))) {
					d = dist;
					ray.d = _mm_cvtss_f32(d);
					ray.plane = planes[j];
				}
			}
			return _mm_cvtss_f32(d);
			
		}
	}
}
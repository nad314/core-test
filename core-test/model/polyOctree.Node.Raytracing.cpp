#include <main>
namespace core {

	float PolyOctree::rayIntersectionT(Ray& ray) const {
		Renderer::rayBoxIntersectionTestSIMD(ray, root->spp, root->sqq);
		if (ray.vmin.x < 0.0f || ray.vmin.x>ray.d)
			return -1.0f;
		return root->rayIntersectionT(ray);
	}


	float PolyOctree::Node::rayIntersectionT(Ray& ray) {
		float d = -1.0f;
		if (hasNodes) {
			for (byte i = 0; i < nnodes; ++i) {
				Renderer::rayBoxIntersectionTestSIMD(ray, node[i]->spp, node[i]->sqq);
				if (ray.vmin.x<0.0f || ray.vmin.x>ray.d)
					continue;
				const float dist = node[i]->rayIntersectionT(ray);
				if (dist > 0 && (dist < d || d < 0.0f))
					ray.d = d = dist;
				if (i>0) std::swap(node[i-1], node[i]);
			}
			return d;
		}
		else {
			/*
			if (points.size() > 0) {
				Renderer::rayBoxIntersectionTestSIMD(ray, spp, sqq);
				ray.d = ray.vmin.x;
				return ray.d;
			}
			return -1.0f;*/
			
			d = std::min(ray.d, d);
			
			const int ls = points.size();
			for (int i = 0, j = 0; i < ls; i += 3, ++j) {
				const float dist = Math::rayPlaneT(ray.r0, ray.r1, planes[j]);
				if (dist > 0 && (dist < d || d < 0.0f))
					if (Math::pointInTriangle((ray.r0 + ray.r1*dist) * 100.0f, points[i], points[i + 1], points[i + 2])) {
						ray.d = d = dist;
						ray.plane = planes[j];
						//lastNode = this;
					}
			}
			return d;
		}
	}
}
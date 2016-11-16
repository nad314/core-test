#include <main>

namespace core {
	PolyOctree::Node* PolyOctree::Node::lastNode = NULL;

	int PolyOctree::Node::count() {
		if (!hasNodes)
			return 1;
		int ret = 1;
		for (int i = 0; i < 8; ++i)
			ret += node[i]->count();
		return ret;
	}

	void PolyOctree::Node::build(buffer<vec4>& buff) {
		if (buff.count() % 3 != 0 || buff.count() == 0)return;
		for (int i = 0; i < buff.count(); i += 3)
			if (Math::triangleAABBIntersection(buff[i], buff[i + 1], buff[i + 2], p, q)) {
				points.push_back(buff[i]);
				points.push_back(buff[i + 1]);
				points.push_back(buff[i + 2]);
				planes.push_back(Math::plane(buff[i].xyz(), buff[i + 1].xyz(), buff[i + 2].xyz()));
			}
	}

	void PolyOctree::Node::bbox() {
		spp = p;
		sqq = q;
		if (points.count() < 1)
			return;
		pp = qq = points[0];
		for (auto& i : points) {
			pp = pp.min(i);
			qq = qq.max(i);
		}
		spp = pp;
		sqq = qq;
	}

	void PolyOctree::Node::sub() {
		if (depth >= maxDepth || points.count() < maxPolys * 3)
			return;
		hasNodes = 1;
		for (int i = 0; i < 8; ++i) {
			node[i] = new Node();
			node[i]->depth = depth + 1;
		}
		vec4 c = (p + q)*0.5f;
		node[0]->p = vec4(p.x, p.y, p.z, 1.0f);
		node[0]->q = vec4(c.x, c.y, c.z, 1.0f);

		node[1]->p = vec4(c.x, p.y, p.z, 1.0f);
		node[1]->q = vec4(q.x, c.y, c.z, 1.0f);

		node[2]->p = vec4(p.x, c.y, p.z, 1.0f);
		node[2]->q = vec4(c.x, q.y, c.z, 1.0f);

		node[3]->p = vec4(c.x, c.y, p.z, 1.0f);
		node[3]->q = vec4(q.x, q.y, c.z, 1.0f);

		node[4]->p = vec4(p.x, p.y, c.z, 1.0f);
		node[4]->q = vec4(c.x, c.y, q.z, 1.0f);

		node[5]->p = vec4(c.x, p.y, c.z, 1.0f);
		node[5]->q = vec4(q.x, c.y, q.z, 1.0f);

		node[6]->p = vec4(p.x, c.y, c.z, 1.0f);
		node[6]->q = vec4(c.x, q.y, q.z, 1.0f);

		node[7]->p = vec4(c.x, c.y, c.z, 1.0f);
		node[7]->q = vec4(q.x, q.y, q.z, 1.0f);

		for (int i = 0; i < points.count(); i += 3) {
			for (int j = 0; j < 8; ++j) {
				if (Math::triangleAABBIntersection(points[i], points[i + 1], points[i + 2], node[j]->p, node[j]->q)) {
					node[j]->points.push_back(points[i]);
					node[j]->points.push_back(points[i + 1]);
					node[j]->points.push_back(points[i + 2]);
					node[j]->planes.push_back(Math::plane(points[i].xyz(), points[i + 1].xyz(), points[i + 2].xyz()));
					break;
				}
			}
		}
		points.clear();
		planes.clear();
		for (int i = 0; i < 8; ++i) {
			node[i]->bbox();
			node[i]->sub();
		}
	}

	float PolyOctree::Node::rayIntersectionT(Ray& ray) {
		float d = -1.0f;
		if (hasNodes) {
			for (byte i = 0; i < nnodes; ++i) {
				const float dtb = Renderer::rayBoxIntersectionTestSIMD(ray, node[i]->spp, node[i]->sqq);
				if (dtb<0.0f || dtb>ray.d)
					continue;
				const float dist = node[i]->rayIntersectionT(ray);
				if (dist > 0 && (dist < d || d < 0.0f))
					ray.d = d = dist;
				std::swap(node[0], node[i]);
			}
			return d;
		}
		else {
			/*
			if (points.size() > 0) {
				ray.d = Renderer::rayBoxIntersectionTestSIMD(ray, spp, sqq);
				return ray.d;
			}
			return -1.0f;
			*/
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

	void PolyOctree::Node::unlink() {
		if (hasNodes)
			for (int i = 0; i < 8; ++i) {
				node[i]->unlink();
				node[i] = NULL;
			}
	}

	void PolyOctree::Node::cacheSort(Node* mem, int& pos, int ddepth) {
		std::queue<Node*> q;
		q.push(this);
		int spos = pos;
		while (!q.empty()) {
			Node* n = q.front();
			q.pop();
			const int cpos = pos;
			mem[pos++] = *n;
			if (n->hasNodes && n->depth < ddepth)
				for (int i = 0; i < 8; ++i) {
					q.push(n->node[i]);
					mem[cpos].node[i] = &mem[cpos + q.size()];
				}
		}
		int cpos = pos;
		for (int i = spos; i < cpos; ++i) {
			if (mem[i].depth >= ddepth && mem[i].hasNodes) {
				for (int j = 0; j < 8; ++j) {
					const int tmp = pos;
					mem[i].node[j]->cacheSort(mem, pos, ddepth + Node::subtreeDepth);
					mem[i].node[j] = &mem[tmp];
				}
			}
		}
	}

	void PolyOctree::Node::multVecs() {
		for (auto& i : points)
			i *= 100.0f;
		if (hasNodes) {
			for (int i = 0; i < 8; ++i)
				node[i]->multVecs();
		}
	}

}

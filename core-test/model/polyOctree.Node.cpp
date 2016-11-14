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
		if (points.count() < 1)
			return;
		pp = qq = points[0];
		for (auto& i : points) {
			pp = pp.min(i);
			qq = qq.max(i);
		}
	}

	void PolyOctree::Node::sub() {
		if (depth >= maxDepth || points.count()<maxPolys * 3)
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

		memcpy(nodeBuff, node, sizeof(node));

		/*
		for (int i = 0; i < 8; ++i)
		node[i]->build(points);
		*/

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
			for (int i = 0; i < 8; ++i) {
				const float dtb = Renderer::rayBoxIntersectionTestF(ray, node[i]->pp, node[i]->qq);
				if (dtb<0.0f || dtb>ray.d)
					continue;
				const float dist = node[i]->rayIntersectionT(ray);
				if (dist > 0 && (dist < d || d < 0.0f)) {
					ray.d = d = dist;
					std::swap(node[0], node[i]);
				}
			}
			return d;
		}
		else {

			if (points.size() > 0) {
				ray.d = Renderer::rayBoxIntersectionTestF(ray, pp, qq);
				return ray.d;
			}
			return -1.0f;

			d = std::min(ray.d, d);

			for (int i = 0; i < points.size(); i += 3) {
				float dist = Math::rayPlaneT(ray.r0.xyz(), ray.r1.xyz(), planes[i / 3]);
				if (dist > 0 && (dist < d || d < 0.0f))
					if (Math::pointInTriangle((ray.r0 + ray.r1*dist) * 100, points[i] * 100, points[i + 1] * 100, points[i + 2] * 100)) {
						ray.d = d = dist;
						ray.plane = planes[i / 3];
						//lastNode = this;
					}
			}
			return d;
		}
	}

	void PolyOctree::Node::cacheSort(Node* mem, int& pos, int depth) const {
		std::queue<Node const*> q;
		q.push(this);
		while (!q.empty()) {
			Node const* n = q.front();
			q.pop();
			mem[pos++] = *n;
			if (n->hasNodes) {
				if (this->depth < depth) {
					for (int i = 0; i < 8; ++i) {
						q.push(n->node[i]);
						mem[pos - 1].node[i] = &mem[pos + q.size() - 1];
					}
				}
				else {
					for (int i = 0; i < 8; ++i) {
						int cpos = pos - 1;
						n->node[i]->cacheSort(mem, pos, depth + 4);
						mem[cpos].node[i] = &mem[pos];
					}
				}
			}
		}
	}

}

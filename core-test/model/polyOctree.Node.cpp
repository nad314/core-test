#include <main>

namespace core {
	PolyOctree::Node* PolyOctree::Node::lastNode = NULL;

	int PolyOctree::Node::count() {
		if (!hasNodes)
			return 1;
		int ret = 1;
		for (int i = 0; i < nnodes; ++i)
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
		pp = q;
		qq = p;
		spp = pp;
		sqq = qq;
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

	void PolyOctree::Node::unlink() {
		if (hasNodes)
			for (int i = 0; i < nnodes; ++i) {
				node[i]->unlink();
				node[i] = NULL;
			}
		hasNodes = 0;
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
				for (int i = 0; i < n->nnodes; ++i) {
					q.push(n->node[i]);
					mem[cpos].node[i] = &mem[cpos + q.size()];
				}
		}
		int cpos = pos;
		for (int i = spos; i < cpos; ++i) {
			if (mem[i].depth >= ddepth && mem[i].hasNodes) {
				for (int j = 0; j < mem[i].nnodes; ++j) {
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


	int PolyOctree::Node::countNodes() {
		nnodes = 0;
		if (!hasNodes)
			return nnodes = (points.size() > 0) ? 1 : 0;
		int cp = 8;
		for (byte i = 0; i < cp; ++i) {
			if (node[i]->countNodes() > 0) {
				++nnodes;
			}
			else {
				std::swap(node[i], node[--cp]);
				--i;
			}
		}
		return (nnodes > 0) ? 1 : 0;
	}

	void PolyOctree::Node::shrinkNodes() {
		if (!hasNodes) return;
		for (byte i = 0; i < 8; ++i)
			node[i]->shrinkNodes();

		if (node[0]->isEmpty() && node[1]->isEmpty() && node[2]->isEmpty() && node[3]->isEmpty())
			pp.z = std::min(std::min(node[4]->pp.z, node[5]->pp.z), std::min(node[6]->pp.z, node[7]->pp.z));
		if (node[4]->isEmpty() && node[5]->isEmpty() && node[6]->isEmpty() && node[7]->isEmpty())
			qq.z = std::max(std::max(node[0]->qq.z, node[1]->qq.z), std::max(node[2]->qq.z, node[3]->qq.z));
		
		if (node[0]->isEmpty() && node[1]->isEmpty() && node[4]->isEmpty() && node[5]->isEmpty())
			pp.y = std::min(std::min(node[4]->pp.y, node[5]->pp.y), std::min(node[6]->pp.y, node[7]->pp.y));
		if (node[2]->isEmpty() && node[3]->isEmpty() && node[6]->isEmpty() && node[7]->isEmpty())
			qq.y = std::max(std::max(node[0]->qq.y, node[1]->qq.y), std::max(node[4]->qq.y, node[5]->qq.y));
		
		if (node[0]->isEmpty() && node[2]->isEmpty() && node[4]->isEmpty() && node[6]->isEmpty())
			pp.x = std::min(std::min(node[1]->pp.x, node[3]->pp.x), std::min(node[5]->pp.x, node[7]->pp.x));
		if (node[1]->isEmpty() && node[3]->isEmpty() && node[5]->isEmpty() && node[7]->isEmpty())
			qq.x = std::max(std::max(node[0]->qq.x, node[2]->qq.x), std::max(node[4]->qq.x, node[6]->qq.x));
		spp = pp;
		sqq = qq;
	}


}

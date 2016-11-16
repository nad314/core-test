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
		vec4 p, q;
		spp.store(p);
		sqq.store(q);
		for (int i = 0; i < buff.count(); i += 3)
			if (Math::triangleAABBIntersection(buff[i], buff[i + 1], buff[i + 2], p, q)) {
				points.push_back(buff[i]);
				points.push_back(buff[i + 1]);
				points.push_back(buff[i + 2]);
				planes.push_back(Math::plane(buff[i].xyz(), buff[i + 1].xyz(), buff[i + 2].xyz()));
			}
	}

	void PolyOctree::Node::sub() {
		if (depth >= maxDepth || points.size() < maxPolys * 3)
			return;
		hasNodes = 1;
		for (int i = 0; i < 8; ++i) {
			node[i] = new Node();
			node[i]->depth = depth + 1;
		}

		vec4 p, q;
		spp.store(p);
		sqq.store(q);
		vec4 c = (p + q)*0.5f;

		node[0]->spp = vec4(p.x, p.y, p.z, 1.0f);
		node[0]->sqq = vec4(c.x, c.y, c.z, 1.0f);

		node[1]->spp = vec4(c.x, p.y, p.z, 1.0f);
		node[1]->sqq = vec4(q.x, c.y, c.z, 1.0f);

		node[2]->spp = vec4(p.x, c.y, p.z, 1.0f);
		node[2]->sqq = vec4(c.x, q.y, c.z, 1.0f);

		node[3]->spp = vec4(c.x, c.y, p.z, 1.0f);
		node[3]->sqq = vec4(q.x, q.y, c.z, 1.0f);

		node[4]->spp = vec4(p.x, p.y, c.z, 1.0f);
		node[4]->sqq = vec4(c.x, c.y, q.z, 1.0f);

		node[5]->spp = vec4(c.x, p.y, c.z, 1.0f);
		node[5]->sqq = vec4(q.x, c.y, q.z, 1.0f);

		node[6]->spp = vec4(p.x, c.y, c.z, 1.0f);
		node[6]->sqq = vec4(c.x, q.y, q.z, 1.0f);

		node[7]->spp = vec4(c.x, c.y, c.z, 1.0f);
		node[7]->sqq = vec4(q.x, q.y, q.z, 1.0f);

		vec4 np, nq;
		for (int i = 0; i < points.count(); i += 3) {
			for (int j = 0; j < 8; ++j) {
				node[j]->spp.store(np);
				node[j]->sqq.store(nq);
				if (Math::triangleAABBIntersection(points[i], points[i + 1], points[i + 2], np, nq)) {
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

	void PolyOctree::Node::expand() {
		if (hasNodes) {
			for (int i = 0; i < nnodes; ++i)
				node[i]->expand();
			spp = node[0]->spp;
			sqq = node[0]->sqq;
			for (int i = 0; i < nnodes; ++i) {
				spp = _mm_min_ps(spp, node[i]->spp);
				sqq = _mm_max_ps(sqq, node[i]->sqq);
			}
		}
		else {
			std::swap(spp, sqq);
			if (points.size() < 1)
				return;
			spp = vec4s(points[0]);
			sqq = vec4s(points[0]);
			for (int i = 1; i < points.size(); ++i) {
				const vec4s p = vec4s(points[i]);
				spp = _mm_min_ps(spp, p);
				sqq = _mm_max_ps(sqq, p);
			}
		}
		
	}

	void PolyOctree::Node::shrinkNodes() {
		if (!hasNodes) return;
		vec4 np[8];
		vec4 nq[8];
		vec4 pp, qq;
		spp.store(pp);
		sqq.store(qq);
		for (byte i = 0; i < 8; ++i) {
			node[i]->shrinkNodes();
			node[i]->spp.store(np[i]);
			node[i]->sqq.store(nq[i]);
		}
		
		if (node[0]->isEmpty() && node[1]->isEmpty() && node[2]->isEmpty() && node[3]->isEmpty())
			pp.z = std::min(std::min(np[4].z, np[5].z), std::min(np[6].z, np[7].z));
		if (node[4]->isEmpty() && node[5]->isEmpty() && node[6]->isEmpty() && node[7]->isEmpty())
			qq.z = std::max(std::max(nq[0].z, nq[1].z), std::max(nq[2].z, nq[3].z));
		
		if (node[0]->isEmpty() && node[1]->isEmpty() && node[4]->isEmpty() && node[5]->isEmpty())
			pp.y = std::min(std::min(np[4].y, np[5].y), std::min(np[6].y, np[7].y));
		if (node[2]->isEmpty() && node[3]->isEmpty() && node[6]->isEmpty() && node[7]->isEmpty())
			qq.y = std::max(std::max(nq[0].y, nq[1].y), std::max(nq[4].y, nq[5].y));
		
		if (node[0]->isEmpty() && node[2]->isEmpty() && node[4]->isEmpty() && node[6]->isEmpty())
			pp.x = std::min(std::min(np[1].x, np[3].x), std::min(np[5].x, np[7].x));
		if (node[1]->isEmpty() && node[3]->isEmpty() && node[5]->isEmpty() && node[7]->isEmpty())
			qq.x = std::max(std::max(nq[0].x, nq[2].x), std::max(nq[4].x, nq[6].x));
		spp = pp;
		sqq = qq;
		
	}


}


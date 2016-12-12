#include <main>

namespace core {
	int PointOctree::Node::count() {
		if (!hasNodes)
			return 1;
		int ret = 1;
		for (int i = 0; i < nnodes; ++i)
			ret += node[i]->count();
		return ret;
	}

	void PointOctree::Node::build(buffer<vec4>& buff, buffer<vec4>& nbuff) {
		if (buff.count() == 0) return;
		vec4 p, q;
		spp.store(p);
		sqq.store(q);
		for (int i = 0; i < buff.count(); ++i)
			if (Math::pointInBox(buff[i], p, q)) {
				points.push_back(vec4s(buff[i]));
				planes.push_back(vec4s(nbuff[i]));
			}
	}

	void PointOctree::Node::sub() {
		if (depth >= maxDepth || points.size() <= maxPoints)
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
		vec4 a;
		for (int i = 0; i < points.count(); ++i) {
			for (int j = 0; j < 8; ++j) {
				node[j]->spp.store(np);
				node[j]->sqq.store(nq);
				points[i].store(a);
				if (Math::pointInBox(a, np, nq)) {
					node[j]->points.push_back(points[i]);
					node[j]->planes.push_back(planes[i]);
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

	void PointOctree::Node::unlink() {
		if (hasNodes)
			for (int i = 0; i < nnodes; ++i) {
				node[i]->unlink();
				node[i] = NULL;
			}
		hasNodes = 0;
	}

	void PointOctree::Node::cacheSort(Node* mem, int& pos, int ddepth) {
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

	void PointOctree::Node::multVecs() {
		for (auto& i : points)
			i *= 1.0f;
		//for the old pointInTriangle method
		/*for (int i = 0; i < points.size(); i += 3) {
		points[i + 1] -= points[i];
		points[i + 2] -= points[i];
		}*/
		if (hasNodes) {
			for (int i = 0; i < 8; ++i)
				node[i]->multVecs();
		}
	}

	int PointOctree::Node::countNodes() {
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

	void PointOctree::Node::expand() {
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

	void PointOctree::Node::expand(const vec4s& rad) {
		if (hasNodes) {
			for (int i = 0; i < nnodes; ++i)
				node[i]->expand(rad);
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
			spp = vec4s(points[0]-rad);
			sqq = vec4s(points[0]+rad);
			for (int i = 1; i < points.size(); ++i) {
				const vec4s p = vec4s(points[i]);
				spp = _mm_min_ps(spp, p-rad);
				sqq = _mm_max_ps(sqq, p+rad);
			}
		}

	}


	void PointOctree::Node::shrinkNodes() {
		if (!hasNodes)
			return;
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

	void PointOctree::Node::trunc() {
		if (hasNodes) {
			bool found;
			while (1) {
				found = 0;
				//trunc childs with only 1 child
				for (int i = 0; i < nnodes; ++i) {
					if (node[i]->hasNodes && node[i]->nnodes == 1) {
						Node* tmp = node[i];
						node[i] = new Node;
						*node[i] = *tmp->node[0];

						for (int j=0;j<8;++j)
							tmp->node[0]->node[j] = NULL;
						delete tmp;

						node[i]->reduceDepth();
						found = 1;
					}
				}
				//merge nodes with few points
				for (int i = 0; i < nnodes; ++i) {
					for (int j = 0; j < nnodes; ++j) {
						if (i == j) continue;
						if ((node[i]->points.count() > 1) && (node[j]->points.count() > 1) && (node[i]->points.count() + node[j]->points.count()) <= Node::maxPoints) {
							for (int k = 0; k < node[j]->points.count(); ++k) {
								node[i]->points.push_back(node[j]->points[k]);
								node[i]->planes.push_back(node[j]->planes[k]);
							}
							node[i]->spp = _mm_min_ps(node[i]->spp, node[j]->spp);
							node[i]->sqq = _mm_max_ps(node[i]->sqq, node[j]->sqq);
							std::swap(node[j], node[--nnodes]);
							delete node[nnodes];
							node[nnodes] = new Node;
							found = 1;
						}
					}
				}
				//fill up to 8 slots
				if (nnodes < 8) {
					int c = -1;
					for (int i = 0; i < nnodes; ++i)
						if (node[i]->hasNodes && node[i]->nnodes > 1)
							c = i;
					if (c >= 0) {
						for (int i = 0; i < nnodes; ++i)
							if (node[i]->hasNodes && node[i]->nnodes < node[c]->nnodes && node[i]->nnodes > 1)
								c = i;
						delete node[nnodes++];
						node[nnodes - 1] = node[c]->node[--node[c]->nnodes];
						node[c]->node[node[c]->nnodes] = new Node;
						node[c]->node[node[c]->nnodes]->depth = node[c]->depth + 1;
						node[nnodes - 1]->reduceDepth();
						continue;
					}
				}
				if (!found) break;
			}
			for (int i = 0; i < nnodes; ++i)
				node[i]->trunc();
		}
	}


}

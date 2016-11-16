#include <main>

namespace core {
	/*
	inline float max3(float a, float b, float c) {
		return std::max(a, std::max(b, c));
	}*/

	void PolyOctree::build(simdMesh& mesh) {
		vec4s pp, qq;
		mesh.bbox(pp, qq);
		root = new Node;
		root->depth = 0;
		pp.store(root->p);
		qq.store(root->q);
		/*
		vec4 q = root->q;
		vec4 p = root->p;
		float maxd = max3(q.x - p.x, q.y - p.y, q.z - p.z);
		q = p + vec4(maxd, maxd, maxd, 1.0f);
		root->q = q;
		root->p = p;
		*/
		buffer<vec4> points;
		points.reserve(mesh.vecs.count() * 3);
		vec4 p;
		for (auto& i : mesh.polys) {
			if (i.n >= 3) {
				mesh.vecs[i[0]].store(p);
				points.push_back(p);
				mesh.vecs[i[1]].store(p);
				points.push_back(p);
				mesh.vecs[i[2]].store(p);
				points.push_back(p);
			}
			if (i.n == 4) {
				mesh.vecs[i[0]].store(p);
				points.push_back(p);
				mesh.vecs[i[2]].store(p);
				points.push_back(p);
				mesh.vecs[i[3]].store(p);
				points.push_back(p);
			}
		}
		//mesh.~simdMesh();
		root->depth = 0;
		root->build(points);
		root->bbox();
		if (root->points.count() > Node::maxPolys * 3)
			root->sub();
		root->countNodes();
		root->multVecs();
		root->shrinkNodes();

		cacheSort();
	}

	void PolyOctree::cacheSort() {
		if (!root)
			return;
		Node* mem;
		int nn = root->count();
		mem = new Node[nn];
		int pos = 0;
		root->cacheSort(mem, pos, Node::subtreeDepth-1);
		delete root;
		root = &mem[0];
	}
}

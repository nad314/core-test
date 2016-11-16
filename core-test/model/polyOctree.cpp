#include <main>

namespace core {
	void PolyOctree::build(simdMesh& mesh) {
		root = new Node;
		mesh.bbox(root->spp, root->sqq);
		root->depth = 0;

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
		if (root->points.count() > Node::maxPolys * 3)
			root->sub();
		root->countNodes();
		root->expand();
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

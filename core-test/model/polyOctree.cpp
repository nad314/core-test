#include <main>

namespace core {

	float PolyOctree::rayIntersectionT(Ray& ray) const {
		const float dtb = Renderer::rayBoxIntersectionTestF(ray, root->pp, root->qq);
		if (dtb < 0.0f || dtb>ray.d)
			return -1.0f;
		return root->rayIntersectionT(ray);
	}

	void PolyOctree::build(simdMesh& mesh) {
		vec4s pp, qq;
		mesh.bbox(pp, qq);
		root = new Node;
		root->depth = 0;
		pp.store(root->p);
		qq.store(root->q);
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
		mesh.~simdMesh();
		root->depth = 0;
		root->build(points);
		root->bbox();
		if (root->points.count() > Node::maxPolys * 3)
			root->sub();
		cacheSort();
		root->multVecs();
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

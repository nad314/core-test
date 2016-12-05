#include <main>

namespace core {
	void PointOctree::build(PointCloud& cloud) {
		root = new Node;
		cloud.bbox(root->spp, root->sqq);
		root->depth = 0;

		buffer<vec4> points;
		buffer<vec4> planes;
		points.reserve(cloud.points.count());
		planes.reserve(cloud.points.count());
		vec4 p, n;
		for (int i = 0; i < cloud.points.count(); ++i) {
			cloud.points[i].store(p);
			points.push_back(p);
			cloud.normals[i].store(n);
			n.w = -Math::dot3(n, p);
			planes.push_back(n);
		}
		root->depth = 0;
		root->build(points, planes);
		if (root->points.count() > Node::maxPoints)
			root->sub();
		root->countNodes();
		root->trunc();
		root->expand();
		root->multVecs();
		root->shrinkNodes();
		cacheSort();
	}

	void PointOctree::cacheSort() {
		if (!root)
			return;
		Node* mem;
		int nn = root->count();
		size = nn;
		printf("Mem size: %d bytes, %d nodes\n", nn * sizeof(Node), nn);
		mem = new Node[nn];
		int pos = 0;
		root->cacheSort(mem, pos, Node::subtreeDepth);
		delete root;
		root = &mem[0];
	}
}
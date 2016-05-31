#pragma once

struct simdMesh {
	core::buffer<core::vec4s> vecs;
	core::buffer<polygon> polys;

	simdMesh() : vecs(), polys() {}
	~simdMesh() { free(); }
	inline void free() { vecs.free(); polys.free(); }

	inline simdMesh &operator = (const basicMesh &m) {
		polys = m.polys;
		vecs.reserve(m.vecs.size());
		for (auto& i : m.vecs)
			vecs[vecs.count()++] = i;
		return *this;
	}

	void importgdev(const char *path);
	void normalize();
	inline void pushp(polygon p) { polys.push_back(p); }
	inline void pushv(const core::vec4s& v) { vecs.push_back(v); }

};

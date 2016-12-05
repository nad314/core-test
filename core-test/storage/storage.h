#pragma once
struct Storage: public core::SIMD, public core::Getter<Storage> {
	core::PointCloud cloud;
	core::PointOctree cloudTree;
	core::PBVH pbvh;

	Storage() { set(*this); }
	int load(const char* path);
	void dispose();
};

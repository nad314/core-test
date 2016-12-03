#pragma once

class CoreTest final : public core::Module, public core::SIMD {
private:
	MainWindow wnd;
	core::simdMesh mesh;
	core::PolyOctree octree;
	core::OBVH bvh;

	core::PointCloud cloud;
	core::PointOctree cloudTree;
	core::PBVH pbvh;
	core::Timer<float> timer;
	core::Timer<float> globalTimer;

	vec4s p, q;

public:
	int onLoad() override;
	int onDispose() override;

	int onStart() override;
	int onStop() override;
	int main() override;
};

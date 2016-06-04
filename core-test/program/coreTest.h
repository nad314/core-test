#pragma once

class CoreTest final : public core::Module, public core::SIMD {
private:
	MainWindow wnd;
	core::simdMesh mesh;
	core::Timer<float> timer;
	core::Timer<float> globalTimer;

public:
	int onLoad() override;
	int onDispose() override;

	int onStart() override;
	int onStop() override;
	int main() override;
};

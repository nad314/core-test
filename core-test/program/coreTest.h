#pragma once

class CoreTest final : public core::Module, public core::SIMD {
private:
	MainWindow wnd;
	simdMesh mesh;
	pView view;
	bool done = 0;
	core::Timer<float> timer;
	core::Timer<float> globalTimer;

public:
	int onLoad() override;
	int onDispose() override;

	int onStart() override;
	int onStop() override;
	int main() override;

	inline MainWindow& getWnd() { return wnd; }
	void onResize();

};

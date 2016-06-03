#pragma once

class CoreTest final : public core::Module {
private:
	MainWindow wnd;
	simdMesh mesh;
	pView view;
	bool done = 0;
	core::Font font;
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

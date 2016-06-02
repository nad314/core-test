#pragma once

class coreTest final : public core::module {
private:
	mainWindow wnd;
	simdMesh mesh;
	pView view;
	pRenderer rend;
	bool done = 0;
	core::font font;
public:
	core::glDevice gl;
public:
	int onLoad() override;
	int onDispose() override;

	int onStart() override;
	int onStop() override;
	int main() override;

	inline mainWindow& getWnd() { return wnd; }
	void onResize();
};

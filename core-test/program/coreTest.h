#pragma once

LRESULT CALLBACK inputProc(HWND hwnd, UINT msg, WPARAM wP, LPARAM lP);
LRESULT CALLBACK inputProcChild(HWND hwnd, UINT msg, WPARAM wP, LPARAM lP);

class coreTest final : public core::module {
private:
	mainWindow wnd;
	core::glDevice gl;
	simdMesh mesh;
	core::image img;
	pView view;
	pRenderer rend;
	bool done = 0;

public:
	int onLoad() override;
	int onDispose() override;

	int onStart() override;
	int onStop() override;
	int main() override;

	inline mainWindow& getWnd() { return wnd; }
	void onResize();
};

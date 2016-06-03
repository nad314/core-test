#pragma once

class MainWindow final : public core::Window {
public:
	core::Module* mp; //module pointer
	RenderWindow rwnd;
	HWND hStatusBar;
public:
	inline core::Window& getRenderWindow() { return rwnd; }
	inline void setModule(core::Module* m) { mp = m; }
	void onOpening() override;
	void onOpened() override;
	void onClosing() override;
	
	int onResize(const core::eventInfo &e) override;
	void setStatusbarText(const char* text);

	inline HWND& getStatusBar() { return hStatusBar; }
};


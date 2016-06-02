#pragma once

class mainWindow final : public core::window {
public:
	core::module* mp; //module pointer
	renderWindow rwnd;
	HWND hStatusBar;
public:
	inline core::window& getRenderWindow() { return rwnd; }
	inline void setModule(core::module* m) { mp = m; }
	void onOpening() override;
	void onOpened() override;
	void onClosing() override;
	
	int onResize(const core::eventInfo &e) override;
	void setStatusbarText(const char* text);

	inline HWND& getStatusBar() { return hStatusBar; }
};


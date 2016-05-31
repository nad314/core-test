#pragma once

class mainWindow final : public core::window {
private:
	renderWindow rwnd;
	HWND hStatusBar;
public:
	inline core::window& getRenderWindow() { return rwnd; }
	void onOpening() override;
	void onOpened() override;
	void onClosing() override;
	void onClosed() override;

	void onResize();
	void setStatusbarText(const char* text);
};


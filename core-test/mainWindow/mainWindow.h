#pragma once

class MainWindow final : public core::Form {
public:
	RenderWindow rwnd;
	core::Button closeButton;
public:
	MainWindow() {}
	inline core::Window& getRenderWindow() { return rwnd; }
	void onOpening() override;
	void onOpened() override;
	void onClosing() override;
	
	int onResize(const core::eventInfo& e) override;
	int onDefault(const core::eventInfo& e) override;
	void onEndPaint(const core::eventInfo& e) override;
	int onLButtonDown(const core::eventInfo& e) override;
};


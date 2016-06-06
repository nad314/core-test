#pragma once

class MainWindow final : public core::WindowForm {
public:
	RenderWindow rwnd;
public:
	MainWindow() {}
	inline core::Window& getRenderWindow() { return rwnd; }
	void onOpening() override;
	void onOpened() override;
	void onClosing() override;
	
	int onResize(const core::eventInfo& e) override;
	int onDefault(const core::eventInfo& e) override;
	void onEndPaint(const core::eventInfo& e) override;
	int onPaint(const core::eventInfo& e) override;
	int onLButtonDown(const core::eventInfo& e) override;
};


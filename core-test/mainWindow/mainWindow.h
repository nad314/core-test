#pragma once

class MainWindow final : public core::WindowForm, public core::Getter<MainWindow> {
private:
	RenderWindow rwnd;
public:
	inline core::Window& getRenderWindow() { return rwnd; }
	void onOpening() override;
	void onOpened() override;
	void onClosing() override;
	
	int onResize(const core::eventInfo& e) override;
	int onActivate(const core::eventInfo& e) override {
		WindowForm::onActivate(e);
		SetFocus(rwnd);
		return 0;
	}
	int onDropFiles(const core::eventInfo& e) override;
	void onEndPaint(const core::eventInfo& e) override;
};


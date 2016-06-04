#pragma once
class RenderWindow final : public core::Window, public core::SIMD{
private:
public:
	View view;

	inline operator View&() { return view; }
	inline operator core::Image&() { return view.img; }

	void onOpening() override;
	void onOpened() override;
	int onResize(const core::eventInfo& e) override;

	void move(int xw, int yw);
};

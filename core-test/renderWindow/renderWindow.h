#pragma once
class RenderWindow final : public core::Window{
private:
public:
	core::vec2i mouse;
	void onOpening() override;
	void move(int xw, int yw);
	void moveMouse(const int& x, const int& y);
};

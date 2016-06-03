#pragma once
class renderWindow final : public core::window{
private:
public:
	core::vec2i mouse;
	void onOpening() override;
	void move(int xw, int yw);
	void moveMouse(const int& x, const int& y);
};

#pragma once
class renderWindow final : public core::window{
private:
public:
	void onOpening() override;
	void move(int xw, int yw);
};

#pragma once

class buttonWindow : public core::windowObject {
private:
	int x, y;
	char text[MAX_PATH];
public:
	buttonWindow() :windowObject() {}
	~buttonWindow() {}

	virtual void setText(const char* caption);
	virtual std::string getText();
	virtual void open(const int& posx, const int& posy, const int& sizex, const int& sizey, windowObject& wParent);
	virtual void open(const int& posx, const int& posy, const int& sizex, const int& sizey, windowObject& wParent, char* caption);
	virtual void onBeginDraw(DRAWITEMSTRUCT* ds);
	virtual void onDraw(DRAWITEMSTRUCT* ds);
};

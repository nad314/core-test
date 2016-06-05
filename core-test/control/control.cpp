#include <main>

namespace core {
	int Control::onMouseMove(const eventInfo& e) { return 0; }
	int Control::onLeftButtonDown(const eventInfo& e) { return 0; }
	int Control::onLeftButtonUp(const eventInfo& e) { return 0; }
	int Control::onRightButtonDown(const eventInfo& e) { return 0; }
	int Control::onRightButtonUp(const eventInfo& e) { return 0; }
	int Control::onPaint(const eventInfo& e) { return 0; }

	void Control::setForeColor(Color color) { foreColor = color; }
	void Control::setBackColor(Color color) { backColor = color; }
	void Control::setForeColorHover(Color color) { foreColorHover = color; }
	void Control::setBackColorHover(Color color) { backColorHover = color; }
	void Control::setForeColorActivated(Color color) { foreColorActivated = color; }
	void Control::setBackColorActivated(Color color) { backColorActivated = color; }
	void Control::setBorderColor(Color color) { borderColor = color; }
}


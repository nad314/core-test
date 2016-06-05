#include <main>

namespace core {
	void Form::setForeColor(Color color) { foreColor = color; __invalidate(); invalidate(); }
	void Form::setBackColor(Color color) { backColor = color; __invalidate(); invalidate(); }

	void Form::setControlForeColor(Color color) {
		controlForeColor = color;
		for (auto& i : items)
			i->setForeColor(color);
		__invalidate(); //redraw everything on next draw
		invalidate(); //request draw
	}

	void Form::setControlBackColor(Color color) {
		controlBackColor = color;
		for (auto& i : items)
			i->setBackColor(color);
		__invalidate();
		invalidate();
	}

	void Form::setControlForeColorHover(Color color) {
		controlForeColorHover = color;
		for (auto& i : items)
			i->setForeColorHover(color);
		__invalidate();
		invalidate();
	}

	void Form::setControlBackColorHover(Color color) {
		controlBackColorHover = color;
		for (auto& i : items)
			i->setBackColorHover(color);
		__invalidate();
		invalidate();
	}

	void Form::setControlForeColorActivated(Color color) {
		controlForeColorActivated = color;
		for (auto& i : items)
			i->setForeColorActivated(color);
		__invalidate();
		invalidate();
	}

	void Form::setControlBackColorActivated(Color color) {
		controlBackColorActivated = color;
		for (auto& i : items)
			i->setBackColorActivated(color);
		__invalidate();
		invalidate();
	}

	void Form::setControlBorderColor(Color color) {
		controlBorderColor = color;
		for (auto& i : items)
			i->setBorderColor(color);
		__invalidate();
		invalidate();
	}

	void Form::setControlColors(Control& control) {
		for (auto& i : items) {
			i->setForeColor(controlForeColor);
			i->setBackColor(controlBackColor);
			i->setForeColorHover(controlForeColorHover);
			i->setBackColorHover(controlBackColorHover);
			i->setForeColorActivated(controlForeColorActivated);
			i->setBackColor(controlBackColorActivated);
			i->setBorderColor(controlBorderColor);
		}
		__invalidate();
		invalidate();
	}

}

#include <main>

Color Theme::backColor = Color(45, 45, 48, 255);
Color Theme::foreColor = Color(255, 255, 255, 255);
Color Theme::controlBackColor = Color(31, 31, 33, 255);
Color Theme::controlForeColor = Color(255, 255, 255, 255);
Color Theme::controlBackColorHover = Color(46, 46, 51, 255);
Color Theme::controlForeColorHover = Color(255, 255, 255, 255);
Color Theme::controlBackColorActivated = Color(42, 42, 46, 255);
Color Theme::controlForeColorActivated = Color(255, 255, 255, 255);
Color Theme::controlBorderColor = Color(16, 16, 16, 255);

void Theme::setFormColor(core::Form& form) {
	form.setForeColor(foreColor);
	form.setBackColor(backColor);
	form.setControlForeColor(controlForeColor);
	form.setControlBackColor(controlBackColor);
	form.setControlForeColorHover(controlForeColorHover);
	form.setControlBackColorHover(controlBackColorHover);
	form.setControlForeColorActivated(controlForeColorActivated);
	form.setControlBackColorActivated(controlBackColorActivated);
	form.setControlBorderColor(controlBorderColor);
}


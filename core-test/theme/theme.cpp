#include <main>

Color Theme::backColor;
Color Theme::foreColor;
Color Theme::controlBackColor;
Color Theme::controlForeColor;
Color Theme::controlBackColorHover;
Color Theme::controlForeColorHover;
Color Theme::controlForeColorActivated;
Color Theme::controlBackColorActivated;
Color Theme::controlBorderColor;

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


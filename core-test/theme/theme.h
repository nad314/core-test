#pragma once
class Theme {
public:
	static Color backColor;
	static Color foreColor;
	static Color controlBackColor;
	static Color controlForeColor;
	static Color controlBackColorHover;
	static Color controlForeColorHover;
	static Color controlForeColorActivated;
	static Color controlBackColorActivated;
	static Color controlBorderColor;

	static void setFormColor(core::Form& form);
};

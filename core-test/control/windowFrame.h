#pragma once

namespace core {
	class WindowFrame final : public Control {
	public:
		Form* form;
		std::string title;
		ImageButton closeButton;
		ImageButton minButton;
	public:

		WindowFrame() : Control(), form(NULL), title("Core") {}

		void make(Form* f, std::string t);
		void setTitle(std::string t) { title = t; form->invalidate(); flags = 0; }
		void colorize();
		
		int onMouseMove(const eventInfo& e) override;
		int onPaint(const eventInfo& e) override;
		int onLButtonDown(const eventInfo& e) override;
		int onLButtonUp(const eventInfo& e) override;
		int onResize(const eventInfo& e) override;


		void setForeColor(Color color) override;
		void setBackColor(Color color) override;
		void setForeColorHover(Color color) override;
		void setBackColorHover(Color color) override;
		void setForeColorActivated(Color color) override;
		void setBackColorActivated(Color color) override;
		void setBorderColor(Color color) override;
	};
}

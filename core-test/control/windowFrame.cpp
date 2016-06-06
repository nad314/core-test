#include <main>

namespace core {

	void WindowFrame::colorize() {
		closeButton.setBackColor(form->backColor);
		closeButton.setBackColorHover(form->controlBackColor);
		closeButton.setBackColorActivated(form->controlBackColor);
		minButton.setBackColor(form->backColor);
		minButton.setBackColorHover(form->controlBackColor);
		minButton.setBackColorActivated(form->controlBackColor);
	}

	void WindowFrame::make(Form* f, std::string t) { 
		if (f == NULL)
			return;
		form = f; 
		setTitle(t); 
		rect = Rect(0, 0, f->width, 32);
		closeButton.make(Rect(form->width - 35, 0, form->width - 7, 28), &Resource::closeImage, *form, [](Form& f)->void {  PostQuitMessage(0); });
		minButton.make(Rect(form->width - 35 - 29, 0, form->width - 7 - 29, 28), &Resource::minimizeImage, *form, [](Form& f)->void { ShowWindow(f, SW_MINIMIZE); });
	}


	int WindowFrame::onMouseMove(const eventInfo& e) {
		if (closeButton.onMouseMove(e) || minButton.onMouseMove(e))
			flags &= ~1;		
		return 0;
	}

	int WindowFrame::onPaint(const eventInfo& e) {
		Renderer::fillRect(vec4i(1, 1, form->width - 65, 32), form->backColor, *form);
		Renderer::print(title.c_str(), *form, 12, 12);

		closeButton.onPaint(e);
		minButton.onPaint(e);

		flags |= 1;
		return 0;
	}

	int WindowFrame::onLButtonDown(const eventInfo& e) {
		if (LOWORD(e.lP)<form->width-65)
			SendMessage(*form, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		if (closeButton.onLButtonDown(e) || minButton.onLButtonDown(e))
			flags &= ~1;
		return 0;
	}

	int WindowFrame::onLButtonUp(const eventInfo& e) {
		if (closeButton.onLButtonUp(e) || minButton.onLButtonUp(e))
			flags &= ~1;
		return 0;
	}

	int WindowFrame::onResize(const eventInfo& e) {
		make(form, title);
		return 0;
	}

	void WindowFrame::setForeColor(Color color) { foreColor = color; colorize();  }
	void WindowFrame::setBackColor(Color color) { backColor = color; colorize(); }
	void WindowFrame::setForeColorHover(Color color) { foreColorHover = color;  colorize();}
	void WindowFrame::setBackColorHover(Color color) { backColorHover = color;  colorize();}
	void WindowFrame::setForeColorActivated(Color color) { foreColorActivated = color;  colorize();}
	void WindowFrame::setBackColorActivated(Color color) { backColorActivated = color;  colorize();}
	void WindowFrame::setBorderColor(Color color) { borderColor = color;  colorize();}

}

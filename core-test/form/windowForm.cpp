#include <main>
namespace core {
	void WindowForm::onOpened() {
		Form::onOpened();
		frame.make(this, getTitle());
		push(frame);
	}

	void WindowForm::setFormTitle(const char* t) {
		Form::setFormTitle(t);
		frame.setTitle(t);
	}
}

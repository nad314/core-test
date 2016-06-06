#pragma once

namespace core {
	class WindowForm : public Form {
	public:
		WindowFrame frame;
	public:
		void onOpened() override;
		void setFormTitle(const char* t) override;
	};
}

#pragma once
class Controller: public core::EventListener, public::core::Getter<Controller> {
public:
	core::Window* parent;
	core::PBVH* pbvh;
	View* view;
	bool valid = 0;
	core::vec2i mouse;
	bool dragging = 0;
	int threads;
	core::Renderer::Worker* thread;
	
	Controller(core::Window* p, core::PBVH* lpbvh);
	~Controller();

	inline void invalidate() { valid = 0; }
	inline void validate() { valid = 1; }
	int onLButtonDown(const core::eventInfo& e) override;
	int onLButtonUp(const core::eventInfo& e) override;
	int onMouseMove(const core::eventInfo& e) override;
	void render();
};

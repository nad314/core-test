#pragma once
namespace core {
	struct subRenderTask : public Renderer::Worker::Task {
		PBVH* pbvh;
		View* pview;
		~subRenderTask() {}
		subRenderTask(PBVH* pB, View* pW) : pbvh(pB), pview(pW) {}
		virtual void execute(Renderer::Worker* pWorker) override;
	};
}

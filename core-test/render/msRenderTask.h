#pragma once
namespace core {
	struct msRenderTask : public Renderer::Worker::Task {
		PBVH* pbvh;
		View* pview;
		~msRenderTask() {}
		msRenderTask(PBVH* pB, View* pW) : pbvh(pB), pview(pW) {}
		virtual void execute(Renderer::Worker* pWorker) override;
	};
}

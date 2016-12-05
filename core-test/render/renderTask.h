#pragma once
namespace core {
	struct RenderTask : public Renderer::Worker::Task {
		PBVH* pbvh;
		View* pview;
		RenderTask(PBVH* pB, View* pW): pbvh(pB), pview(pW) {}
		virtual void execute(Renderer::Worker* pWorker) override;
	};
}

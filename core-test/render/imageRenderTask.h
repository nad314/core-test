#pragma once
namespace core {

	struct RenderShader final : public Renderer::PixelShader, public SIMD {
		vec4s lightPos;
		RenderShader(const simdView& view) { 
			matrixf inv = view.mat;
			inv.invert();
			matrixs sinv = inv;
			lightPos = sinv*vec4s(2.0f, 2.0f, -5.0f, 1.0f);
		}
		const vec4s getColor(const Ray& ray, const float& d, const vec4s& normal, const vec4s& color, const PBVH& bvh, std::pair<int, float>* stack, int* priority) const {
			Ray lightRay;
			PBVH::Ray lray;
			const vec4s point = (ray.sr0 + ray.sr1*vec4s(d));
			const vec4s lightDir = (lightPos - point).normalized3d();
			lightRay.sr0 = point + lightDir*vec4s(0.0001f);
			lightRay.sr0.w1();
			lightRay.sr1 = lightDir;
			lightRay.sr1.w1();
			lightRay.sinvr1 = _mm_rcp_ps(lightDir);
			lray = lightRay;
			lray.d = -100.0f;
			if (bvh.findFirst(lray, stack, priority, false) < 0.0f) {
				const vec4s ndotl = normal.dot3(lightDir);
				return _mm_max_ps(_mm_sub_ps(_mm_setzero_ps(), ndotl), ndotl);
			}
			return vec4s(0.0f);
		};
	};

	struct VolumetricShader final : public Renderer::PixelShader, public SIMD {
		vec4s lightPos;
		vec4s lightPlane;
		int samples;
		VolumetricShader(const simdView& view, int lightSamples);
		const vec4s getColor(const Ray& ray, const float& d, const vec4s& normal, const vec4s& color, const PBVH& bvh, std::pair<int, float>* stack, int* priority) const;
	};


	struct renderShowTask final: public Renderer::imageShowTask, public Getter<renderShowTask> {
		Image img;
		Image simg;
		vec2i *threadSquare;
		int nt;
		renderShowTask(simdView* view, int nthreads);
		renderShowTask() { delete[] threadSquare; }
		void onStartNode(simdView* view, vec2i cStep, vec2i nStep, int square) override;
		void onEndNode(simdView* view, vec2i cStep, vec2i nStep, int square) override;
	};

	struct imageRenderTask : public Renderer::Worker::Task {
		PBVH* pbvh;
		simdView* pview;
		Renderer::PixelShader* shader;
		int samples;
		~imageRenderTask() {}
		imageRenderTask(PBVH* pB, simdView* pW, const int samp, Renderer::PixelShader* sh) : pbvh(pB), pview(pW), samples(samp) { shader = sh; }
		virtual void execute(Renderer::Worker* pWorker) override;
	};
}

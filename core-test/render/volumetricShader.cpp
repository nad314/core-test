#include <main>

namespace core {
	VolumetricShader::VolumetricShader(const simdView& view, int lightSamples) {
		matrixf inv = view.rotation*view.translation;
		inv.invert();
		matrixs sinv = inv;
		lightPos = sinv*vec4s(-0.5f, -0.5f, -5.0f, 1.0f);
		vec4 lp;
		(vec4s(_mm_setzero_ps()) - lightPos).store(lp);
		lp.normalize3d();
		lp.w = -m128(vec4s(lp).dot3(lightPos).m)[0];
		lightPlane = lp;
		srand(rand());
		samples = lightSamples;
	}
	const vec4s VolumetricShader::getColor(const Ray& ray, const float& d, const vec4s& normal, const vec4s& color, const PBVH& bvh, std::pair<int, float>* stack, int* priority) const {
		Ray lightRay;
		PBVH::Ray lray;
		const vec4s point = (ray.sr0 + ray.sr1*vec4s(d));
		const vec4s lightDir = (lightPos - point);
		const vec4s nor = normal;
		/*
		vec4s nor = normal;
		if (_mm_comilt_ss(lightDir.dot3(nor), _mm_setzero_ps()) == 0)
		nor = _mm_sub_ps(_mm_setzero_ps(), normal);
		*/
		vec4s ret(0.0f);
		const float rad = 8.0f;
		for (int i = 0; i < samples; ++i) {
			vec3 rv = vec3(rand() % 100 - 50, rand() % 100 - 50, rand() % 100 - 50) / 50.0f * 60.0f;
			matrixf mat;
			mat.rotate(rv.z, 0.0f, 0.0f, 1.0f);
			mat.rotate(rv.y, 0.0f, 1.0f, 0.0f);
			mat.rotate(rv.x, 1.0f, 0.0f, 0.0f);
			matrixs ms = mat;
			const vec4s newDir = (ms*nor).normalized3d();
			lightRay.sr0 = point + newDir*vec4s(0.0001f);
			//lightRay.sr0.w1();
			lightRay.sr1 = newDir;
			//lightRay.sr1.w1();
			lightRay.sinvr1 = _mm_rcp_ps(newDir);
			lray = lightRay;
			lray.d = -100.0f;

			if (bvh.findFirst(lray, stack, priority, true) < 0.0f) {
				const vec4s dist = SSE::rayPlaneT(lightRay.sr0, lightRay.sr1, lightPlane);
				const vec4s ip = lightRay.sr0 + lightRay.sr1*dist;
				const vec4s dd = lightPos - ip;
				if (m128(dd.dot3(dd))[0] < rad*rad) {
					const vec4s ndotl = nor.dot3(newDir.normalized3d())*1.6f;
					ret += _mm_max_ps(_mm_setzero_ps(), ndotl);
				}
				//else return vec4s(0.0f);
			}
		}
		return _mm_min_ps(vec4s(1.0f), ret / (float)samples);
	}

}
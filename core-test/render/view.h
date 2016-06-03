struct pView {
	core::Image img;
	matrixf projection;
	matrixf	translation;
	matrixf rotation;
	matrixf mat;
	float fov;
	vec4 viewVec;
	vec4s simdViewVec;
	vec4s v05 = vec4s(vec4(0.5f, 0.5f, 1.0f, 1.0f));
	vec4s v06 = vec4s(vec4(0.5f, -0.5f, 1.0f, 1.0f));
	vec4s nvs = vec4s(vec4(0,0,0,0));
	vec4s vvs;
	
	void make (const int width, const int height);
	void clear();
	void home();
	vec4 project (const vec4 &v);
	__m128 projectSSE (const __m128 &v, const __m128 &viewVec, const __m128 &v05) const;
	void updateMatrix();
	
	inline vec4s project (const vec4s &v) const {
		return vec4s(_mm_mul_ps (_mm_add_ps(
												_mm_mul_ps(
													_mm_mul_ps(v,
														_mm_rcp_ps(_mm_shuffle_ps (v, v, _MM_SHUFFLE(3,3,3,3)))),
													v06), 
												v05), 
											simdViewVec));
	}
	
	inline vec4s clamp (const vec4s &v) const {
		return vec4s(_mm_min_ps(_mm_max_ps(v, _mm_setzero_ps()), vvs));
	}
	
};

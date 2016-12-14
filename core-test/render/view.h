struct simdView : public core::SIMD {
	vec4s simdViewVec;
	vec4s v05 = vec4s(vec4(0.5f, 0.5f, 1.0f, 1.0f));
	vec4s v06 = vec4s(vec4(0.5f, -0.5f, 1.0f, 1.0f));
	vec4s nvs = vec4s(vec4(0,0,0,0));
	vec4s vvs;

	vec4s v07 = vec4s(vec4(0.5f, 0.5f, 0.0f, 0.0f));
	vec4s v08 = vec4s(vec4(0.5f, 0.5f, 0.5f, 1.0f));
	vec4s v09;
	vec4s v10 = vec4s(vec4(0.0f, 0.0f, 1.0f, 0.0f));

	matrixf projection;
	matrixf	translation;
	matrixf rotation;
	matrixf mat;
	float fov;
	int mode = 0;

	core::Image img;
	
	void make (const int width, const int height);
	void clear();
	void home();
	void updateMatrix();
	
	inline vec4s project (const vec4s &v) const {
		return (v*_mm_rcp_ps(_mm_shuffle_ps (v, v, _MM_SHUFFLE(3,3,3,3)))*v06 + v05)*simdViewVec;
	}
	
	inline vec4s unproject(const vec4s &v) const {
		const vec4s t = (v / v09 - v07) / v08;
		return (t-v10)/_mm_permute_ps(t, 0xff);
	}

	inline vec4s clamp (const vec4s &v) const {
		return vec4s(_mm_min_ps(_mm_max_ps(v, _mm_setzero_ps()), vvs));
	}
	
};

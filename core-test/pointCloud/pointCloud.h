#pragma once

namespace core {
	struct PointCloud: public SIMD {
		buffer<vec4s> points;
		buffer<vec4s> normals;

		bool loadObj(const char* path);
		bool saveRaw(const char* path);
		bool loadRaw(const char* path);
		bool saveAtomic(const char* path, const float& rad);
		bool loadAtomic(const char* path, float& rad);
		void normalize();
		void bbox(vec4s& p, vec4s& q) const;

		inline void dispose() { points.clear(); normals.clear(); }
	};
}

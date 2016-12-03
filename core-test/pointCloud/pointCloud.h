#pragma once

namespace core {
	struct PointCloud: public SIMD {
		buffer<vec4s> points;
		buffer<vec4s> normals;

		bool loadObj(const char* path);
		bool saveRaw(const char* path);
		bool loadRaw(const char* path);
		void normalize();
		void bbox(vec4s& p, vec4s& q) const;

		inline void dispose() { points.clear(); normals.clear(); }
	};
}

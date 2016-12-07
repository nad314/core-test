#include <main>

namespace core {
	bool PointCloud::loadObj(const char* path) {
		FILE* f;
		vec4 v;
		char s[256];
		int pointCount(0);
		int normalCount(0);
		printf("Loading %s\n", path);
		if (!(f = fopen(path, "r")))
			return 0;
		try {
			while (!feof(f)) {
				if (!fgets(s, 256, f))
					break;
				if (strncmp(s, "v ", 2) == 0)
					++pointCount;
				else if (strncmp(s, "vn ", 3) == 0)
					++normalCount;
			}
			points.reserve(pointCount);
			normals.reserve(normalCount);
			fseek(f, 0, SEEK_SET);
			while (!feof(f)) {
				if (!fgets(s, 256, f))
					break;
				if (strncmp(s, "v ", 2) == 0) {
					if (sscanf(s, "v %f %f %f", &v.x, &v.y, &v.z) != 3)
						continue;
					v.w = 1.0f;
					points.push_back(vec4s(v));
				}
				else if (strncmp(s, "vn ", 3) == 0) {
					if (sscanf(s, "vn %f %f %f", &v.x, &v.y, &v.z) != 3)
						continue;
					v.w = 1.0f;
					v.normalize3d();
					normals.push_back(vec4s(v));
				}
			}
		}
		catch (std::exception e) {
			points.clear();
			normals.clear();
			fclose(f);
			return 0;
		}

		printf("Loaded %d vertices and %d normals\n", points.count(), normals.count());

		fclose(f);
		return 1;
	}

	bool PointCloud::saveRaw(const char* path) {
		FILE* f;
		if (points.count() < 1 || !(f=fopen(path, "wb")))
			return 0;
		try {
			fwrite(&points.count(), sizeof(points.count()), 1, f);
			fwrite(points, 1, sizeof(vec4s)*points.count(), f);
			fwrite(&normals.count(), sizeof(normals.count()), 1, f);
			fwrite(normals, 1, sizeof(vec4s)*normals.count(), f);
		}
		catch (std::exception e) {
			core::Debug::print("%s\n", e.what());
			fclose(f);
			return 0;
		}
		fclose(f);
		return 1;
	}

	bool PointCloud::loadRaw(const char* path) {
		FILE* f;
		dispose();
		if (!(f = fopen(path, "rb")))
			return 0;
		try {
			int n; //change to size_t for 64 bit or whatever
			if (fread(&n, sizeof(n), 1, f)!=1) throw std::exception("Read Error");
			points.reserve(n);
			if (fread(points, sizeof(vec4s), n, f) != n) throw std::exception("Read Error");
			points.count() = n;
			if (fread(&n, sizeof(n), 1, f) != 1) throw std::exception("Read Error");
			normals.reserve(n);
			if (fread(normals, sizeof(vec4s), n, f) != n) throw std::exception("Read Error");
			normals.count() = n;
			core::Debug::print("Loaded %d points and %d normals\n", points.count(), normals.count());
		}
		catch (std::exception e) {
			core::Debug::print("%s\n", e.what());
			fclose(f);
			dispose();
			return 0;
		}
		fclose(f);
		return 1;
	}


	bool PointCloud::saveAtomic(const char* path, const float& rad) {
		FILE* f;
		if (points.count() < 1 || !(f = fopen(path, "wb")))
			return 0;
		try {
			fwrite(&points.count(), sizeof(points.count()), 1, f);
			fwrite(points, sizeof(vec4s), points.count(), f);
			fwrite(&normals.count(), sizeof(normals.count()), 1, f);
			fwrite(normals, sizeof(vec4s), normals.count(), f);
			fwrite(&rad, sizeof(rad), 1, f);
		}
		catch (std::exception e) {
			core::Debug::print("%s\n", e.what());
			fclose(f);
			return 0;
		}
		fclose(f);
		return 1;
	}

	bool PointCloud::loadAtomic(const char* path, float& rad) {
		FILE* f;
		dispose();
		rad = -1.0f;
		if (!(f = fopen(path, "rb")))
			return 0;
		try {
			int n; //change to size_t for 64 bit or whatever
			if (fread(&n, sizeof(n), 1, f) != 1) throw std::exception("Read Error");
			points.reserve(n);
			if (fread(points, sizeof(vec4s), n, f) != n) throw std::exception("Read Error");
			points.count() = n;
			if (fread(&n, sizeof(n), 1, f) != 1) throw std::exception("Read Error");
			normals.reserve(n);
			if (fread(normals, sizeof(vec4s), n, f) != n) throw std::exception("Read Error");
			normals.count() = n;
			if (fread(&rad, sizeof(rad), 1, f) != 1) core::Debug::print("Loading without radius\n");
			core::Debug::print("Loaded %d points and %d normals\n", points.count(), normals.count());
		}
		catch (std::exception e) {
			core::Debug::print("%s\n", e.what());
			fclose(f);
			dispose();
			return 0;
		}
		fclose(f);
		return 1;
	}


	void PointCloud::normalize() {
		vec4s p, q; //bounding box
		vec4s avg;
		float dist;

		if (!points.size())
			return;

		p = q = points[0];
		for (auto& i : points) {
			p = p.min(i);
			q = q.max(i);
		}

		avg = (p + q)*0.5f;
		dist = sqrt((p - q).magnitude())*0.5f;

		for (auto& i : points) {
			i = (i - avg) / dist;
			i.w1();
		}
	}

	void PointCloud::bbox(vec4s& p, vec4s& q) const {
		if (!points.size())
			return;

		p = q = points[0];
		for (auto& i : points) {
			p = p.min(i);
			q = q.max(i);
		}
	}


}

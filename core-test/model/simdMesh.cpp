#include <main>

void simdMesh::importgdev(const char *path) {
	gdevFileChunk chunk;
	FILE* f;
	long long pos;
	int nchunks(0);
	int unsupported(0);

	char currDir[MAX_PATH];
	char filepath[MAX_PATH];
	int filesize;
	const int buffersize = 1024;
	char buffer[buffersize];
	char fileVersionString[16];
	FILE *f2;
	vec4 tmpvec;

	memset(fileVersionString, 0, sizeof(fileVersionString));

	if (!(f = fopen(path, "rb")))return;

	while (!feof(f))
	{
		if (fread(&chunk, sizeof(chunk), 1, f) != 1)
			break;
		++nchunks;
		switch (chunk.type)
		{
		case sf_version_string:
		{
			fread(fileVersionString, 8, 1, f);
			printf("Version: %s\n", fileVersionString);
			break;
		}
		case sf_object:
		{
			if (chunk.subtype != 0x00000001) {
				++unsupported;
				fseek(f, chunk.size, SEEK_CUR);
			}
			else {
				int version;
				int fileVersion;
				int n(0);
				char name[256];
				char tname[256];
				int id;
				int components;
				int flags;
				int subs;
				int subLevel;

				// pObject fields
				fread(&id, sizeof(int), 1, f); // pObject.id
				fread(tname, 256, 1, f); // pObject.tname
				fread(name, 256, 1, f); // pObject.name
										// !pObject fields

				fread(&components, sizeof(int), 1, f);
				fread(&flags, sizeof(int), 1, f);
				fread(&subs, sizeof(int), 1, f);
				fread(&subLevel, sizeof(int), 1, f);

				fread(&n, sizeof(int), 1, f);
				printf("Points: %d\n", n);
				vecs.reserve(n);
				vecs.count() = n;
				fread(vecs, sizeof(vec4)*vecs.size(), 1, f);
				fread(&n, sizeof(int), 1, f);
				polys.reserve(n);
				printf("Polygons: %d\n", n);
				polys.count() = n;
				fread(polys, sizeof(polygon)*polys.size(), 1, f);
			}
			break;
		}
		default: fseek(f, chunk.size, SEEK_CUR); ++unsupported; break;
		};
	}

	fclose(f);
	printf("Loaded file: %d chunks total, %d unsupported\n", nchunks, unsupported);
}

void simdMesh::normalize() {
	vec4s p, q; //bounding box
	vec4s avg;
	float dist;

	if (!vecs.size())
		return;

	p = q = vecs[0];
	for (auto& i : vecs) {
		p = p.min(i);
		q = q.max(i);
	}

	avg = (p + q)*0.5f;
	dist = sqrt((p - q).magnitude())*0.5f;

	for (auto& i : vecs) {
		i = (i - avg) / dist;
		i.w1();
	}
}
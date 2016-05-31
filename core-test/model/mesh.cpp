#include <main>
#define ddprintf printf

void basicMesh::importObj( char* path ) {
	char s[256], st[256];
	polygon p;
	vec4 v;
	int i,j;
	FILE *f;
	if(!(f=fopen(path, "r"))){ddprintf("Error opening OBJ file\n"); return;}
	free();
	memset (&v, 0, 16);
	while (!feof(f)) {
		fgets( s, sizeof(s), f );
		if( s[0]=='#' )continue;
		memset(st, 0, sizeof(st) );
		sscanf(s, "%s", st );
		if( strcmp(st, "v" )==0 ){sscanf( s, "v %f %f %f", &v.x, &v.y, &v.z ); pushv(v);} else
		if( strcmp(st, "f" )==0 )
		{
			int r = 0;
			int pos = strlen(st)+1;
			if( sscanf( s+pos, "%d", &p.q[0] )==1 )++r; while( !isspace(*(s+pos)) )++pos; ++pos;
			if( sscanf( s+pos, "%d", &p.q[1] )==1 )++r; while( !isspace(*(s+pos)) )++pos; ++pos;
			if( sscanf( s+pos, "%d", &p.q[2] )==1 )++r; while( !isspace(*(s+pos)) )++pos; ++pos;
			if( sscanf( s+pos, "%d", &p.q[3] )==1 )++r; while( !isspace(*(s+pos)) )++pos; ++pos;
			if( r==4 )p.n=4; else p.n=3;
			for( i=0;i<p.n;++i )p.q[i]--;
			pushp(p);
		}
	}
	fclose(f);
	vecs.shrink_to_fit();
	polys.shrink_to_fit();
	ddprintf("Successfully loaded OBJ\n");
	ddprintf("Points: %d\n", vecs.size());
	ddprintf("Polygons: %d\n", polys.size() );
}

void basicMesh::importgdev (const char *path) {
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
	
	memset (fileVersionString, 0, sizeof(fileVersionString));
	
	if(!(f=fopen(path, "rb")))return;

	while (!feof(f))
	{
		if (fread (&chunk, sizeof(chunk), 1, f)!=1)
			break;
		++nchunks;
		switch (chunk.type)
		{
			case sf_version_string:
			{
				fread (fileVersionString, 8, 1, f);
				printf("Version: %s\n", fileVersionString);
				break;
			}
			case sf_object:
				{ 
					if(chunk.subtype!=0x00000001){ 
						++unsupported;
						fseek(f, chunk.size, SEEK_CUR); 
					} else {
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
						fread (&id, sizeof(int), 1, f); // pObject.id
						fread (tname, 256, 1, f); // pObject.tname
						fread (name, 256, 1, f); // pObject.name
						// !pObject fields
						
						fread (&components, sizeof(int), 1, f);
						fread (&flags, sizeof(int), 1, f);
						fread (&subs, sizeof(int), 1, f);
						fread (&subLevel, sizeof(int), 1, f);
						
						fread (&n, sizeof(int), 1, f);
						printf("Points: %d\n", n);
						vecs.reserve(n);
						vecs.count() = n;
						fread (vecs, sizeof(vec4)*vecs.size(), 1, f);
						fread (&n, sizeof(int), 1, f);
						polys.reserve(n);
						printf("Polygons: %d\n", n);
						polys.count() = n;
						fread (polys, sizeof(polygon)*polys.size(), 1, f);
					}
					break;
				}
			default: fseek(f, chunk.size, SEEK_CUR); ++unsupported; break;
		};
	}
	
	fclose(f);
	ddprintf ("Loaded file: %d chunks total, %d unsupported\n", nchunks, unsupported);
}

void basicMesh::normalize() {
	vec4 p, q; //bounding box
	vec4 avg;
	float dist;
	
	if (!vecs.size())
		return;
	
	p = q = vecs[0];
	for (auto& i:vecs) {
		p = p.min(i);
		q = q.max(i);
	}
	
	avg = (p+q)*0.5f;
	dist = sqrt((p-q).magnitude())*0.5f;
	
	for (auto& i:vecs) {
		i = (i-avg)/dist;
		i.w = 1.0f;
	}
	
}





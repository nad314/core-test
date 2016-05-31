#define sf_version_string 0x0000
#define sf_file 0x0001
#define sf_object 0x0002

struct gdevFileChunk {
	uint type;
	uint subtype;
	long long size;
};

struct polygon {
	byte n;
	uint q[4];
	polygon() {n=0;}
	~polygon() {n=0;}
	polygon (const polygon &p) {for( int i=0;i<4;++i ){q[i]=p.q[i];} n=p.n;}
	
	polygon &operator = (const polygon &p) {for( int i=0;i<4;++i ){q[i]=p.q[i]; } n=p.n; return *this;}
	inline void push (int id) {if(n>3)return;  q[n++]=id;}
	inline void to_array (int* x){if(x)memcpy(x, q, n*sizeof(int) );}
	inline void free(){n=0;}
	
	inline uint &operator[] (const int &i){return q[i];}
};


struct basicMesh {
	core::buffer<core::vec4> vecs;
	core::buffer<polygon> polys;
	
	void importObj (char* path);
	void importgdev (const char *path);
	void normalize();
	
	inline void pushp (polygon p) {polys.push_back(p);}
	inline void pushv (core::vec4 v) {vecs.push_back(v);}
	inline void free() {vecs.free(); polys.free();}
	
	basicMesh():vecs(), polys(){};
	~basicMesh(){polys.free(); vecs.free();}			
};




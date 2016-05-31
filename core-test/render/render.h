
struct pRenderer {
	void drawModel (basicMesh &mesh, pView* view) const;
	void drawPoints (simdMesh &mesh, pView* view) const;
	inline void putPixel (const int &x, const int &y, const core::vector3<byte> &v, core::image &img) const { 
		memcpy (img.data+(y*img.width+x)*4, v, 3);
	}

	inline void putPixel (const int &x, const int &y, const core::vector4<byte> &v, core::image &img) const { 
		memcpy (img.data+(y*img.width+x)*4, v, 4);
	}
	
};


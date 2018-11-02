#ifndef COBJBUFFERRAWH
#define COBJBUFFERRAWH

#include "CC3DMath.h"

class cObjLoadBufferRaw {
public:
	// === Direct Read for Obj File ===
	std::vector<Vec3> vecs;
	std::vector<Vec3> nors;
	std::vector<Vec2> uvs;

	std::vector<short> idx_v;// [DE_USHRT_MAX];
	std::vector<short> idx_n;// [DE_USHRT_MAX];
	std::vector<short> idx_u;// [DE_USHRT_MAX];

	std::vector<short> idx_of1;// [DE_USHRT_MAX];
	std::vector<short> num_of;// [DE_USHRT_MAX];

	std::vector<short> idx_fv1;// [DE_USHRT_MAX];
	std::vector<short> num_fv;// [DE_USHRT_MAX];

	short num_tri, num_face;
public:
	cObjLoadBufferRaw() {}
	~cObjLoadBufferRaw() {
		reserve(0, 0, 0, 0, 0);
	}

	void reserveperfilesize(int fsize);
	void reserve(ushort maxv, ushort maxu, ushort maxi, ushort maxo, ushort maxtri);
	void loadObj(const char *data, int len, float loadscale);
	void print();

};

#endif
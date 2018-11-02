#ifndef COBJBUFFERH
#define COBJBUFFERH

//#define OBJ_FACE_CCW 
//#define OBJ_FACE_CW

#include "CC3DMath.h"
#include "cG3DefModel.h"
#include "cObjBufferRaw.h"

class cG3DefModelGen;

const int DE_USHRT_MAX = 64000;

typedef struct objedge_t {
    ushort vidx1, vidx2; // sortedv
    ushort tri1, tri2; // sortedv
    ushort edgegroup;
    bool hard; //if the edge
    
    objedge_t(){
        vidx1 = vidx2 = edgegroup = -1;
        tri1 = tri2 = -1;
        hard = true;
    }
    
    bool equal(ushort edg2i1, ushort egd2i2){
        return ((edg2i1 == vidx1) && (egd2i2 == vidx2))
        || ((edg2i1 == vidx2) && (egd2i2 == vidx1));
    }
} objedge_t;

typedef struct vecmix_t {
    Vec3 nor, vec;
	Vec2 uv;
} vecmix_t;


typedef struct objtri_t {
    ushort vidxs[3];
	Vec2 uvs[3];
	Vec3 nors[3];
    ushort e[3]; // 3 edge //objedge
   
    ushort mixvidxs[3]; // 3 vec idx of //vecmix; //const mixed vecs

	Vec3 facenormal; //tri normal;
	ushort facegroup;


    objtri_t(){
        vectorSet(facenormal, 0, 0, 0);
		facegroup = 0;
        e[0] = e[1] = e[2] = -1;
		vidxs[0] = vidxs[1] = vidxs[2] = -1;
		mixvidxs[0] = mixvidxs[1] = mixvidxs[2] = -1;
        facegroup = -1;
    }
} objtri_t;


class cObjLoadBuffer {
public:
	std::vector<vecmix_t> mixvecs;

	std::vector<Vec3> corners;
	std::vector<ushort> hardcorners;

	std::vector<objedge_t> edges;
	std::vector<ushort> hardedges;

	std::vector<objtri_t> tris;
	ushort num_facegroup; // num of group of tris
public:
	cObjLoadBuffer() {
		clear();
	}
	~cObjLoadBuffer() {
		clear();
	}
	void clear();

	void process_rawbuffer(cObjLoadBufferRaw &r);

	cG3DefModel *makeModel();
	void makeFaceModels(std::vector<cG3DefModel *> &facemdls);
	void makeEdgeModels(std::vector<cG3DefModel *> &edgemdls, std::vector<Mat4> &m4s, float size);
	void makeCornerModels(std::vector<cG3DefModel *> &cornmdls, std::vector<Mat4> &m4s, float size);

	int hittestfaces(const Vec3 &vnear, const Vec3 &vdir);
	int hittestedges(const Vec3 &vnear, const Vec3 &vfar, float &distsq);

	void logdebug();
};

void OBJ_LoadTopo(cObjLoadBuffer &b, const char *data, int len, float loadscale);
Sprite3D *DefModel2Sprite3D(cG3DefModel *mdl);


#endif
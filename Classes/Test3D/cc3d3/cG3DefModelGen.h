#ifndef CG3DEFMODELGENH
#define CG3DEFMODELGENH

#include "cocos2d.h"
USING_NS_CC;

#include "CC3DMath.h"
#include "cG3DefModel.h"


void settriidx(std::vector<ushort> &idxs, int offset, int i1, int i2, int i3);

void avgnormal(const Vec3 &v1, const Vec3 &v2, Vec3 &ret);
void mkcrossnormal(const Vec3 &vbase, const Vec3 &vhead, const Vec3 &vbase2,
                   Vec3 &ret);
void mknormal(const Vec3 &vbase, const Vec3 &vhead, Vec3 &ret);


typedef struct sModelEdgeIdx{
    int head, tail, groupidx;
    float distsq;
} sModelEdgeIdx;

class cG3DefModelGen: public cG3DefModel {
	typedef cG3DefModel base;
protected:
    
public:
    bool iscone, isdonut, iscylinder, issphere, smoothside, isobj;
    
    int sidecnt; // number of sides
    float scaletop;
    float height, radius;
    float startangle;
    
    Vec3 centertop, centerbottom;
    
    std::vector<ushort> trigroupidxs;     // which face-group a tri belongs to? 0 = bottom, 1 = top, 2 = first side....
	int trigroupcnt;

    // which group is an edge belong to? -1 = all hidden edge,
    // 0 ~ n - 1 = bottom edges,
    // n ~ 2n - 1 = top edges,
    // 2n ~ 3n -1 = side edges
	std::vector<sModelEdgeIdx>edges; 

	std::vector<Vec3>corners;		// all position vertex ignored uv and noraml
    std::vector<bool>cornerhards;   // mark if position vertex is actual countable hard corner (not smoothed vertex that define curve face)

    cG3DefModelGen(const char *name):base(name){};//for subclasses
    
public:
    cG3DefModelGen(const char *name, int sidecnt, float goup, float godown,
                   float radius, float scaletop, float startangle, bool smoothside);
	virtual void virtualdelete();
    
    void print();
    
    int getHardCornerCount();
    int getHardEdgeCount();
    int getHardFaceCount();
    
protected:
    void allocateGLobjs(int veccnt, int tricnt);
    void allocateCorners(int ccnt);
    void copybaseattributes(cG3DefModelGen *mdlbase);
    
    void genbeltface(int vidx, int iidx, cG3DefModelGen *mdlbase);
    void genconeface(int vidx, int iidx, cG3DefModelGen *mdlbase);
    void gentopcap(int vidx, int iidx, cG3DefModelGen *mdlbase);
    void genbottomcap(int vidx, int iidx, cG3DefModelGen *mdlbase);
    
    void gensidequatface(int vidx, int iidx, int sidenum, cG3DefModelGen *mdlbase);
    void gensidetriface(int vidx, int iidx, int sidenum, cG3DefModelGen *mdlbase);

};

#endif
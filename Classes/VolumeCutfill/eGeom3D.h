#ifndef eGeom3DH__
#define eGeom3DH__

#include "cObjBuffer.h"

class CC3DLayer;

class eGeom3D {

public:

    //2D
    std::vector<Vec2> vecFree;
    std::vector<Vec2> vecFreeTris;

    //3D
    Vec3 vHeight, baseCenter;
    std::vector<Vec3> v3BtmSides;

    cG3DefModelGen *model, *modeltop, *modelbottom;

public:
    eGeom3D() :
    model(nullptr),
    modeltop(nullptr),
    modelbottom(nullptr),
    vHeight(0, 0, 0.3f) {}

    virtual ~eGeom3D() {
        unload();
    }

    void unload();

    static inline float pixelto3dunit(float pix) {
        return pix / 256.0f;
    }

    //for regular and cirlce polygon
    void poly2tri();

    //for 3D
    void unloadModel();

    bool loadModel();

    cG3DefModelGen *loadModelTris(const std::vector<Vec2> &tris, float z);

    cG3DefModelGen *loadModelPillarSides(const std::vector<Vec3> &vecs, const Vec3 sidehigh);
};

inline Sprite3D *addSprite3D(Node *pnode, Sprite3D *sp3d) {
    pnode->addChild(sp3d);
    sp3d->setCameraMask(pnode->getCameraMask());
    return sp3d;
}

Sprite3D *addSprite3DModel(cG3DefModelGen *mdl, Node *cc3dlayer, const Vec3 &pos);

cG3DefModelGen *preallocModel(int cntvec, int cntidx);

inline Vec3 poly2solid(const Vec2 &v2, float z) {
    return Vec3(eGeom3D::pixelto3dunit(v2.x - 512), eGeom3D::pixelto3dunit(v2.y - 384), z);
}

inline Vec3 polylist2solid(const std::vector<Vec2> &v2, std::vector<Vec3> &v3out, float z) {
    int vcnt = (int) v2.size();
    int scnt = vcnt;
    v3out.resize(vcnt);
    Vec3 vsum(0, 0, 0);
    for (int i = 0; i < scnt; i++) {
        Vec3 tmpv = poly2solid(v2[i], z);
        vsum += tmpv;
        v3out[i] = tmpv;
        //CCLOG("polylist2solid() %d (%0.3f, %0.3f, %0.3f", i, tmpv.x, tmpv.y, tmpv.z);
    }
//    v3out[scnt] = v3out[0];//no double counting, assume last is overlap;
    return (vsum * (1.0f / scnt));
}

inline void
modelSetTri(cG3DefModel *model, int idx0, const Vec3 &v0, const Vec3 &v1, const Vec3 &v2,
            const Vec3 &nor) {
    model->vers[idx0].set(v0);
    model->vers[idx0 + 1].set(v1);
    model->vers[idx0 + 2].set(v2);
    model->uvs[idx0].set(0, 0);
    model->uvs[idx0 + 1].set(0, 1);
    model->uvs[idx0 + 2].set(1, 1);

    model->nors[idx0].set(nor);
    model->nors[idx0 + 1].set(nor);
    model->nors[idx0 + 2].set(nor);

    model->idxs[idx0] = idx0;
    model->idxs[idx0 + 1] = idx0 + 1;
    model->idxs[idx0 + 2] = idx0 + 2;
}

inline void
modelSetTri(cG3DefModel *model, int idx0, const Vec3 &v0, const Vec3 &v1, const Vec3 &v2) {
    modelSetTri(model, idx0, v0, v1, v2, tri2normal(v1, v0, v2));
}

template<typename T>
inline std::vector<T> vectormakereverse(const std::vector<T> &src) {
    std::vector<T> ret(src.size());
    std::reverse_copy(src.begin(), src.end(), ret.begin());
    return ret;
}


inline void modelSetQuatVec(cG3DefModel *model, int idx0, int vi0, const Vec3 &v0, const Vec3 &v1,
                            const Vec3 &v2, const Vec3 &v3) {
    //4 vecs
    model->vers[vi0].set(v0);
    model->vers[vi0 + 1].set(v1);
    model->vers[vi0 + 2].set(v2);
    model->vers[vi0 + 3].set(v3);

    Vec3 nor = tri2normal(v1, v0, v2);
    model->nors[vi0].set(nor);
    model->nors[vi0 + 1].set(nor);
    model->nors[vi0 + 2].set(nor);
    model->nors[vi0 + 3].set(nor);

    model->uvs[vi0].set(0, 0);
    model->uvs[vi0 + 1].set(1, 0);
    model->uvs[vi0 + 2].set(0, 1);
    model->uvs[vi0 + 3].set(1, 1);

    //6 idxs
    model->idxs[idx0] = vi0;
    model->idxs[idx0 + 1] = vi0 + 1;
    model->idxs[idx0 + 2] = vi0 + 2;
    model->idxs[idx0 + 3] = vi0 + 1;
    model->idxs[idx0 + 4] = vi0 + 3;
    model->idxs[idx0 + 5] = vi0 + 2;
}


#endif

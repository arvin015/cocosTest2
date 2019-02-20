#ifndef Geom3DH__
#define Geom3DH__

#include "cObjBuffer.h"
#include "BasicData.h"

//namespace VolumeCutfill {

    inline bool isClockwise(std::vector<VertexInfo> pts) {
        if (pts.size() < 1) return false;
        pts.push_back(pts.at(0));

        int count = (int)pts.size();
        if (count < 4)
            return 0.0f;
        float a = 0.0f;
        for (int i = 1; i < count; ++i) {
            a += pts[i-1].p2d.x * pts[i].p2d.y - pts[i-1].p2d.y * pts[i].p2d.x;
        }
        return 0 > a;
    }

    class Geom3D {

    public:

        //2D
        std::vector<VertexInfo> vecFree; //初始2d顶点集
        std::vector<cocos2d::Vec2> vecFreeTris; //转换成三角形后的2d顶点集

        //3D
        Vec3 vHeight, baseCenter;
        std::vector<VertexInfo> v3BtmSides; //vecFree转成3d顶点集

        cG3DefModelGen *model, *modeltop, *modelbottom; //侧面、上面、下面数据模型

    public:
        Geom3D()
        : model(nullptr)
        , modeltop(nullptr)
        , modelbottom(nullptr)
        , vHeight(0, 0, 0.3f) {}

        virtual ~Geom3D() {
            unload();
        }

        void unload();

        static inline float pixelto3dunit(float pix) {
            return pix / 256.0f;
        }

        /**
         * 多边形分割为多个三角形---解决凹多边形填充问题
         */
        void poly2tri();

        /**
         * 释放3D
         */
        void unloadModel();

        /**
         * 加载3D
         */
        bool loadModel();

        /**
         * 创建上下面3D数据模型-若干个3D三角形组成
         */
        cG3DefModelGen* loadModelTris(const std::vector<Vec2> &tris, float z);

        /**
         * 创建侧面3D数据模型
         */
        cG3DefModelGen* loadModelPillarSides(const std::vector<VertexInfo> &vecs, const Vec3 sidehigh);
    };

    /**
     * 创建Sprite3D
     */
    inline Sprite3D* addSprite3D(Node *pnode, Sprite3D *sp3d) {
        pnode->addChild(sp3d);
        sp3d->setCameraMask(pnode->getCameraMask());
        return sp3d;
    }

    /**
     * 根据3D数据模型创建Sprite3D
     */
    Sprite3D* addSprite3DModel(cG3DefModelGen *mdl, Node *cc3dlayer, const Vec3 &pos);

    /**
     * 创建3D数据模型
     */
    cG3DefModelGen* preallocModel(int cntvec, int cntidx);

    /**
     * 2d坐标转3d坐标
     */
    inline Vec3 poly2solid(const Vec2 &v2, float z) {
        return Vec3(Vec3(Geom3D::pixelto3dunit(v2.x - 512), Geom3D::pixelto3dunit(v2.y - 384), z));
    }

    /**
     * 2d顶点集转3d
     * 返回3d顶点集中心点
     */
    inline Vec3 polylist2solid(const std::vector<VertexInfo> &v2, std::vector<VertexInfo> &v3out, float z) {
        int vcnt = (int) v2.size();
        int scnt = vcnt;
        v3out.resize(vcnt);
        Vec3 vsum(0, 0, 0);
        for (int i = 0; i < scnt; i++) {
            VertexInfo tmpv = VertexInfo(v2[i].vId, poly2solid(v2[i].p2d, z));
            vsum += tmpv.p3d;
            v3out[i] = tmpv;
        }
        return (vsum * (1.0f / scnt));
    }

    /**
     * 设置三角形3D模型数据
     */
    inline void
    modelSetTri(cG3DefModel* model, int idx0, const Vec3 &v0, const Vec3 &v1, const Vec3 &v2,
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

    /**
     * 设置单元三角形3D模型数据
     */
    inline void
    modelSetTri(cG3DefModel* model, int idx0, const Vec3 &v0, const Vec3 &v1, const Vec3 &v2) {
        modelSetTri(model, idx0, v0, v1, v2, tri2normal(v1, v0, v2));
    }

    /**
     * 集合反序
     */
    template<typename T>
    inline std::vector<T> vectormakereverse(const std::vector<T> &src) {
        std::vector<T> ret(src.size());
        std::reverse_copy(src.begin(), src.end(), ret.begin());
        return ret;
    }

    /**
     * 设置矩形3D模型数据
     */
    inline void modelSetQuatVec(cG3DefModel* model, int idx0, int vi0, const Vec3 &v0, const Vec3 &v1,
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
//}

#endif

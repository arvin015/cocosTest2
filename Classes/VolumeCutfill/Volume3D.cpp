//
//  Volume3D.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2019/2/15.
//

#include "Volume3D.h"
#include "CC3DLayer.h"
#include "CC2D3Util.h"
#include "cG3DefModelGen.h"

USING_NS_CC;
using namespace std;

const float frameradius = 0.003f;

Volume3D::Volume3D()
: cc3DLayer(nullptr)
, volume3d(nullptr)
, topSp3d(nullptr)
, sideSp3d(nullptr)
, bottomSp3d(nullptr)
, offset(0, 0, 0){

}

Volume3D::~Volume3D() {

}

void Volume3D::setData(CC3DLayer* cc3DLayer, vector<Vec2> vecs) {
    this->cc3DLayer = cc3DLayer;
    geom3D.vecFree = vecs;

    geom3D.poly2tri();
    geom3D.loadModel();
}

void Volume3D::showModel() {

    volume3d = addSprite3D(cc3DLayer, Sprite3D::create());
    loadFaces(); //加载面
    loadEdges(); //描边
}

Sprite3D* Volume3D::makeEdgeSprite(const Vec3 &v1, const Vec3 &v2, Node *cc3dlayer) {
    cG3DefModelGen* modeledge = new cG3DefModelGen("", 8, (v2-v1).length(), 0, frameradius, 1, 0, true);
    Sprite3D* sp = SAFE_RETAIN(addSprite3DModel(modeledge, cc3dlayer, v1));
    sp->setRotationQuat(lookatQuat(v1, v2, Vec3(0, 0, 1)));
    CC_SAFE_RELEASE_NULL(modeledge);
    return sp;
}

void Volume3D::loadFaces() {
    topSp3d = SAFE_RETAIN(addSprite3DModel(geom3D.modeltop, volume3d, Vec3(0, 0, geom3D.vHeight.z))); //上面
    sideSp3d = SAFE_RETAIN(addSprite3DModel(geom3D.model, volume3d, Vec3::ZERO)); //侧面
    bottomSp3d = SAFE_RETAIN(addSprite3DModel(geom3D.modelbottom, volume3d, Vec3::ZERO)); //下面
}

void Volume3D::loadEdges() {
    Vec3 height = Vec3(0,0, geom3D.vHeight.z);
    const std::vector<Vec3> &vs = geom3D.v3BtmSides;
    int cntside = (int) vs.size();
    spEdges.reserve(cntside * 3);
    for (int i = 0; i < cntside; i++) {
        spEdges.push_back(SAFE_RETAIN(makeEdgeSprite(vs[i], vs[(i+1)%cntside], volume3d)));
        spEdges.push_back(SAFE_RETAIN(makeEdgeSprite(vs[i], vs[i] + height, volume3d)));
        spEdges.push_back(SAFE_RETAIN(makeEdgeSprite(vs[i] + height, vs[(i+1)%cntside] + height, volume3d)));
    }

    for (Sprite3D* sp : spEdges) {
        sp->setColor(Color3B::BLACK);
    }
}

void Volume3D::findMinDisPoint(const Vec2 &point, const Vec2 &startPoint, Vec2 &rsPoint, float &rs) {
    int index = 0;
    rs = 1000;
    for (int i = 0; i < top2dVecs.size(); i++) {
        Vec2 v = top2dVecs.at(i);
        if (v == startPoint) continue; //去掉startPoint点
        float dis = point.distance(v);
        if (dis < rs) {
            index = i;
            rs = dis;
        }
    }
    rsPoint = top2dVecs.at(index);
}

void Volume3D::cutPolygon(const Vec2 &p1, const Vec2 &p2, vector<Vec2> &leftVecList, vector<Vec2> &rightVecList) {
    bool isStart = false;
    bool isStop = false;

    int i = 0;
    for (Vec2 v : top2dVecs) {
        if (!isStart) {
            if (v == p1 || v == p2) {
                isStart = true;
                leftVecList.push_back(geom3D.vecFree.at(i));
                rightVecList.push_back(geom3D.vecFree.at(i));
            } else {
                leftVecList.push_back(geom3D.vecFree.at(i));
            }
        } else {
            if (isStop) {
                leftVecList.push_back(geom3D.vecFree.at(i));
            } else {
                rightVecList.push_back(geom3D.vecFree.at(i));
                if (v == p1 || v == p2) {
                    isStop = true;
                    leftVecList.push_back(geom3D.vecFree.at(i));
                }
            }
        }
        i++;
    }

//    bool isStart = false;
//
//    for (int i = 0; i < top2dVecs.size(); i++) {
//        Vec2 v = top2dVecs.at(i);
//        if (!isStart) {
//            if (v == p1 || v == p2) {
//                isStart = true;
//                rightVecList.push_back(v);
//            }
//        } else {
//            rightVecList.push_back(v);
//            if (v == p1 || v == p2) break;
//            top2dVecs.erase(top2dVecs.begin() + i);
//            i--;
//        }
//    }
//    leftVecList = top2dVecs;
}

void Volume3D::playTranslateAnim(float duration, const Vec3 &offset) {
    this->offset = offset;
    volume3d->runAction(MoveBy::create(duration, offset));

    vector3dTo2d();
}

void Volume3D::clear() {
    volume3d->removeFromParent();
    dotDrawNode->removeFromParent();
}

void Volume3D::vector3dTo2d() {

    //顶部面3d坐标转成2d屏幕坐标
    for (Vec3 v : geom3D.v3BtmSides) {
        v.set(v.x, v.y, geom3D.vHeight.z);
        v = v + offset;
        top2dVecs.push_back(cc3DLayer->_camera->projectGL(v));
    }
    if (!shapeIsClockwise(top2dVecs)) { //转成顺时针
        top2dVecs = vectormakereverse(top2dVecs);
    }

    //test start----------
    dotDrawNode = DrawNode::create();
    cc3DLayer->addChild(dotDrawNode);
    int i = 0;
    for (Vec2 v : top2dVecs) {
        dotDrawNode->drawDot(v, i+2, Color4F::RED);
        i++;
    }
    //test end----------
}

void Volume3D::draw2D() {
    auto drawNode = DrawNode::create();
    cc3DLayer->addChild(drawNode);

    int tris = geom3D.vecFreeTris.size() / 3;
    Vec2 *vecs = geom3D.vecFreeTris.data();
    for (int i = 0; i < tris; i++) {
        drawNode->drawSolidPoly(vecs + (i * 3), 3, Color4F::YELLOW);
    }

    int edges = geom3D.vecFree.size();
    for (int i = 0; i < edges; i++) {
        drawNode->drawSegment(geom3D.vecFree[i], geom3D.vecFree[(i+1)%edges], 1, Color4F::BLACK);
    }
}

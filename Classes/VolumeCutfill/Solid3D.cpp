//
//  Solid3D.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2019/2/15.
//

#include "Solid3D.h"
#include "CC3DLayer.h"
#include "CC2D3Util.h"
#include "cG3DefModelGen.h"
#include "BasicData.h"
#include "MathUtils.h"

USING_NS_CC;
using namespace std;

const float frameradius = 0.003f;

inline Vec2 solid2poly(const Vec3 &v) {
    return Vec2(v.x*256+512, v.y*256+384);
}

Solid3D::Solid3D()
: cc3DLayer(nullptr)
, topSp3d(nullptr)
, sideSp3d(nullptr)
, bottomSp3d(nullptr)
, offset(0, 0, 0)
, dotDrawNode(nullptr){

}

Solid3D::~Solid3D() {

}

void Solid3D::onEnter() {
    Sprite3D::onEnter();

    //test start----------
    if (dotDrawNode == nullptr) {
        dotDrawNode = DrawNode::create();
        getParent()->addChild(dotDrawNode);
    }
    dotDrawNode->clear();
    int i = 0;
    for (VertexInfo v : top2dVecs) {
        dotDrawNode->drawDot(v.p2d, i+2, Color4F::RED);
        i++;
    }
    //test end----------
}

void Solid3D::onExit() {
    Sprite3D::onExit();
}

bool Solid3D::init() {
    if (!Sprite3D::init()) {
        return false;
    }
    return true;
}

void Solid3D::setData(CC3DLayer* cc3DLayer, const vector<VertexInfo> &vecs) {
    this->cc3DLayer = cc3DLayer;
    geom3D.vecFree = vecs;

    geom3D.poly2tri();
    geom3D.loadModel();
}

void Solid3D::showModel() {

    loadFaces(); //加载面
    loadEdges(); //描边

    vertex3dTo2d();
}

void Solid3D::loadFaces() {
    topSp3d = SAFE_RETAIN(addSprite3DModel(geom3D.modeltop, this, Vec3(0, 0, geom3D.vHeight.z))); //上面
    sideSp3d = SAFE_RETAIN(addSprite3DModel(geom3D.model, this, Vec3::ZERO)); //侧面
    bottomSp3d = SAFE_RETAIN(addSprite3DModel(geom3D.modelbottom, this, Vec3::ZERO)); //下面
}

void Solid3D::loadEdges() {
    Vec3 height = Vec3(0,0, geom3D.vHeight.z);
    std::vector<VertexInfo> vs = geom3D.v3BtmSides;
    //去掉非顶点的点
    for (int i = 0; i < vs.size(); i++) {
        Vec2 v = Vec2(vs.at(i).p3d.x, vs.at(i).p3d.y);
        Vec2 v1 = Vec2(vs.at(i == 0 ? vs.size() - 1 : i - 1).p3d.x, vs.at(i == 0 ? vs.size() - 1 : i - 1).p3d.y);
        Vec2 v2 = Vec2(vs.at((i + 1) % vs.size()).p3d.x, vs.at((i + 1) % vs.size()).p3d.y);
        if (Equal((v1 - v).cross(v2 - v), 0)) { //向量外积为0表示平行
            vs.erase(vs.begin() + i);
            i--;
        }
    }
    int cntside = (int) vs.size();
    spEdges.reserve(cntside * 3);
    for (int i = 0; i < cntside; i++) {
        spEdges.push_back(SAFE_RETAIN(makeEdgeSprite(vs[i].p3d, vs[(i+1)%cntside].p3d, this)));
        spEdges.push_back(SAFE_RETAIN(makeEdgeSprite(vs[i].p3d, vs[i].p3d + height, this)));
        spEdges.push_back(SAFE_RETAIN(makeEdgeSprite(vs[i].p3d + height, vs[(i+1)%cntside].p3d + height, this)));
    }

    for (Sprite3D* sp : spEdges) {
        sp->setColor(Color3B::BLACK);
    }
}

Sprite3D* Solid3D::makeEdgeSprite(const Vec3 &v1, const Vec3 &v2, Node *cc3dlayer) {
    cG3DefModelGen* modeledge = new cG3DefModelGen("", 8, (v2-v1).length(), 0, frameradius, 1, 0, true);
    Sprite3D* sp = SAFE_RETAIN(addSprite3DModel(modeledge, cc3dlayer, v1));
    sp->setRotationQuat(lookatQuat(v1, v2, Vec3(0, 0, 1)));
    CC_SAFE_RELEASE_NULL(modeledge);
    return sp;
}

bool Solid3D::getMinDisPoint(const Vec2 &point, const Vec2 &startPoint, float maxDis, VertexInfo &rs, float &dist) {
    int index = -1;
    dist = 1000;
    for (int i = 0; i < top2dVecs.size(); i++) {
        Vec2 v = top2dVecs.at(i).p2d;
        if (v == startPoint) continue; //去掉startPoint点
        float dis = point.distance(v);
        if (dis > maxDis) continue; //去掉超出最大距离点
        if (dis < dist) {
            index = i;
            dist = dis;
        }
    }

    if (index == -1) return false;

    rs = top2dVecs.at(index);
    return true;
}

bool Solid3D::checkCanCut(const Vec2 &p1, const Vec2 &p2) {
    //排除三种情况，1：重叠；2：相交线段条数大于4；3：中心点不在多边形内
    int count = 0; //统计相交个数
    for (int i = 0; i < top2dVecs.size(); i++) {
        Vec2 v1 = top2dVecs[i].p2d;
        Vec2 v2 = top2dVecs[(i+1)%top2dVecs.size()].p2d;
        Vec2 rs;
        int value = intersection(p1, p2, v1, v2, rs);
        if (value > 3 && value < 7) return false; //排除重合
        if (value > 0) { //相交
            count++;
        }
    }
    if (count != 4) return false;
    vector<Vec2> vecs;
    for(VertexInfo v : top2dVecs) {
        vecs.push_back(v.p2d);
    }
    if (!isInPolygon(vecs, p1.getMidpoint(p2))) return false;
    return true;
}

void Solid3D::cutPolygon(const Vec2 &p1, const Vec2 &p2, vector<VertexInfo> &leftVecList, vector<VertexInfo> &rightVecList) {
    bool isStart = false;
    bool isStop = false;

    int i = 0;
    for (VertexInfo v : top2dVecs) {
        VertexInfo vInfo = geom3D.v3BtmSides.at(top2dVecs.size()-1-i);
        Vec2 originalVec = solid2poly(vInfo.p3d); //vecFree动画之后的坐标
        VertexInfo vertexInfo = VertexInfo(vInfo.vId, originalVec);
        if (!isStart) {
            if (v.p2d == p1 || v.p2d == p2) {
                isStart = true;
                leftVecList.push_back(vertexInfo);
                rightVecList.push_back(vertexInfo);
            } else {
                leftVecList.push_back(vertexInfo);
            }
        } else {
            if (isStop) {
                leftVecList.push_back(vertexInfo);
            } else {
                if (v.p2d == p1 || v.p2d == p2) {
                    isStop = true;
                    leftVecList.push_back(vertexInfo);
                }
                rightVecList.push_back(vertexInfo);
            }
        }
        i++;
    }
}

void Solid3D::playTransAnim(float duration, const Vec2 &p1, const Vec2 &p2, const TRANS_DIRECTION &direction) {
    Vec3 offset;
    Vec2 center = getCenterPoint();
    if (direction == HOR) {
        Vec2 v1; //与x轴的交点
        float k, b;
        if (getLineFormula(p1, p2, k, b)) { //斜率存在
            v1 = Vec2(-b/k, 0);
        } else { //斜率不存在
            v1 = Vec2(p1.x, 0);
        }
        float ps = judgePointToLine(v1, p2, center);
        if (ps == 1) { //右边
            offset.set(Volume_MoveByValue, 0, 0);
        } else if (ps == -1) { //左边
            offset.set(-Volume_MoveByValue, 0, 0);
        }
    } else if (direction == VER) {
        Vec2 v1; //与y轴的交点
        float k, b;
        if (getLineFormula(p1, p2, k, b)) { //斜率存在
            v1 = Vec2(0, b);
        }
        float ps = judgePointToLine(v1, p2, center);
        if (ps == 1) { //右边
            offset.set(0, -Volume_MoveByValue, 0);
        } else if (ps == -1) { //左边
            offset.set(0, Volume_MoveByValue, 0);
        }
    }

    playTranslateAnim(duration, offset);
}

void Solid3D::playTranslateAnim(float duration, const Vec3 &offset) {
    this->offset = offset;
    runAction(MoveBy::create(duration, offset));

    vertex3dTo2d(); //3d顶点坐标转2d屏幕坐标
}

void Solid3D::vertex3dTo2d() {

    top2dVecs.clear();

    //顶部面3d坐标转成2d屏幕坐标
    for (int i = 0; i < geom3D.v3BtmSides.size(); i++) {
        Vec3 v3 = geom3D.v3BtmSides.at(i).p3d;
        geom3D.v3BtmSides.at(i).p3d.set(v3 + offset); //3D坐标相应改变
        v3 = geom3D.v3BtmSides.at(i).p3d;
        v3.set(v3.x, v3.y, geom3D.vHeight.z);
        top2dVecs.push_back(VertexInfo(geom3D.v3BtmSides.at(i).vId, cc3DLayer->_camera->projectGL(v3)));
    }
    if (!isClockwise(top2dVecs)) { //转成顺时针
        top2dVecs = vectormakereverse(top2dVecs);
    }

    if (getParent() != nullptr) {
        //test start----------
        if (dotDrawNode == nullptr) {
            dotDrawNode = DrawNode::create();
            getParent()->addChild(dotDrawNode);
        }
        dotDrawNode->clear();
        int i = 0;
        for (VertexInfo v : top2dVecs) {
            dotDrawNode->drawDot(v.p2d, i+2, Color4F::RED);
            i++;
        }
        //test end----------
    }
}

Vec2 Solid3D::getCenterPoint() {
    Vec2 total;
    for (VertexInfo v : top2dVecs) {
        total += v.p2d;
    }
    return total / top2dVecs.size();
}

void Solid3D::clear() {
    if (dotDrawNode != nullptr) {
        dotDrawNode->removeFromParent();
    }
}

void Solid3D::draw2D() {
    auto drawNode = DrawNode::create();
    cc3DLayer->addChild(drawNode);

    int tris = geom3D.vecFreeTris.size() / 3;
    Vec2 *vecs = geom3D.vecFreeTris.data();
    for (int i = 0; i < tris; i++) {
        drawNode->drawSolidPoly(vecs + (i * 3), 3, Color4F::YELLOW);
    }

    int edges = geom3D.vecFree.size();
    for (int i = 0; i < edges; i++) {
        drawNode->drawSegment(geom3D.vecFree[i].p2d, geom3D.vecFree[(i+1)%edges].p2d, 1, Color4F::BLACK);
    }
}

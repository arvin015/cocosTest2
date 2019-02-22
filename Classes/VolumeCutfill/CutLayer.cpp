//
//  CutLayer.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2019/2/21.
//

#include "CutLayer.h"

USING_NS_CC;
using namespace std;

CutLayer::CutLayer()
: BaseCutfillLayer() {

}

CutLayer::~CutLayer() {

}

void CutLayer::onEnter() {
    BaseCutfillLayer::onEnter();

    touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(CutLayer::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(CutLayer::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(CutLayer::onTouchEnded, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void CutLayer::onExit() {
    if (touchListener != nullptr) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(touchListener);
    }
    BaseCutfillLayer::onExit();
}

bool CutLayer::init() {
    if (!BaseCutfillLayer::init()) {
        return false;
    }
    return true;
}

bool CutLayer::onTouchBegan(Touch* touch, Event* event) {
    if (!isVisible()) return false;
    if (!touchEnabled) return false;
    startVertex = getStartPoint(touch->getLocation());
    drawNodeEx->drawDashLine(startVertex.p2d, touch->getLocation(), 1.0f, 5, 5, Color4F::BLACK);
    return true;
}

void CutLayer::onTouchMoved(Touch* touch, Event* event) {
    drawNodeEx->clear();
    drawNodeEx->drawDashLine(startVertex.p2d, touch->getLocation(), 1.0f, 5, 5, Color4F::BLACK);
}

void CutLayer::onTouchEnded(Touch* touch, Event* event) {
    drawNodeEx->clear();
    VertexInfo lastVertex;
    if (getEndPoint(touch->getLocation(), startVertex.p2d, lastVertex)) { //切割线满足条件
        drawNodeEx->drawDashLine(startVertex.p2d, lastVertex.p2d, 1.0f, 5, 5, Color4F::BLACK);
        if (curSolid3D && curSolid3D->checkCanCut(startVertex.p2d, lastVertex.p2d)) { //满足切割条件
            cutSolid(curSolid3D, startVertex, lastVertex);
        }
        drawNodeEx->clear();
    }
}

bool CutLayer::getEndPoint(const Vec2 &point, const Vec2 &startPoint, VertexInfo &rs) {
    if (curSolid3D == nullptr) return false;
    float dis;
    VertexInfo ver;
    if (curSolid3D->getMinDisPoint(point, startPoint, MaxDis, ver, dis)) {
        rs = ver;
        return true;
    }
    return false;
}

void CutLayer::cutSolid(Solid3D* solid3D, const VertexInfo &ver1, const VertexInfo &ver2) {
    if (solid3D == nullptr) return;

    //特殊情况处理
    CutInfo cutInfo;
    bool isSpecial = question->getCutInfo(ver1.vId, ver2.vId, cutInfo);
    if (isSpecial) {
        if (!cutInfo.canCut) return;
    }

    vector<VertexInfo> vecList1, vecList2;
    solid3D->cutPolygon(ver1.p2d, ver2.p2d, vecList1, vecList2);
    solid3D->clear(); //Test
    solid3D->removeFromParent();
    solid3DList.eraseObject(solid3D);
    CC_SAFE_RELEASE_NULL(solid3D);

    Solid3D* new3d1 = createSolid3D(vecList1);
    Solid3D* new3d2 = createSolid3D(vecList2);

    Solid3D::TRANS_DIRECTION direction = getTransDirection(ver1.p2d, ver2.p2d);

    //特殊处理---FUCK
    if (isSpecial) {

        new3d1->playTranslateAnim(Volume_Duration, cutInfo.direction == 2 ? Vec3(0, Volume_MoveByValue, 0) : Vec3(Volume_MoveByValue, 0, 0));
        new3d2->playTranslateAnim(Volume_Duration, cutInfo.direction == 2 ? Vec3(0, -Volume_MoveByValue, 0) : Vec3(-Volume_MoveByValue, 0, 0));

        for (int i = 0; i < solid3DList.size() - 2; i++) {
            solid3DList.at(i)->playTranslateAnim(Volume_Duration, cutInfo.direction == 2 ? Vec3(0, Volume_MoveByValue, 0) : Vec3(Volume_MoveByValue, 0, 0));
        }

    } else {
        for (int i = 0; i < solid3DList.size(); i++) {
            solid3DList.at(i)->playTransAnim(Volume_Duration, ver1.p2d, ver2.p2d, direction);
        }
    }
}

Solid3D::TRANS_DIRECTION CutLayer::getTransDirection(const Vec2 &p1, const Vec2 &p2) {
    float slope = getSlope(p1, p2);
    if (slope >= 0.5f || slope <= -0.5f) { //45°-135°左右方向
        return Solid3D::HOR;
    } else return Solid3D::VER;
}

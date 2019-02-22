//
//  FillLayer.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2019/2/21.
//

#include "FillLayer.h"

USING_NS_CC;
using namespace std;

FillLayer::FillLayer()
: BaseCutfillLayer() {

}

FillLayer::~FillLayer() {

}

void FillLayer::onEnter() {
    BaseCutfillLayer::onEnter();

    touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(FillLayer::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(FillLayer::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(FillLayer::onTouchEnded, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void FillLayer::onExit() {
    if (touchListener != nullptr) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(touchListener);
    }
    BaseCutfillLayer::onExit();
}

bool FillLayer::init() {
    if (!BaseCutfillLayer::init()) {
        return false;
    }
    return true;
}

bool FillLayer::onTouchBegan(Touch* touch, Event* event) {
    if (!isVisible()) return false;
    if (!touchEnabled) return false;
    startVertex = getStartPoint(touch->getLocation());
    drawNodeEx->drawDashLine(startVertex.p2d, touch->getLocation(), 1.0f, 5, 5, Color4F::BLACK);
    return true;
}

void FillLayer::onTouchMoved(Touch* touch, Event* event) {
    drawNodeEx->clear();
    drawNodeEx->drawDashLine(startVertex.p2d, touch->getLocation(), 1.0f, 5, 5, Color4F::BLACK);
}

void FillLayer::onTouchEnded(Touch* touch, Event* event) {
    drawNodeEx->clear();
    Vec2 vec = touch->getLocation();
    fillSolid(startVertex, vec);
}

void FillLayer::fillSolid(const VertexInfo &startVertex, const Vec2 &endPoint) {
    FillInfo* fillInfo = question->getFillInfo(startVertex.vId);
    if (fillInfo == nullptr) return; //此顶点不能填补
    if (fillInfo->isFilled) return; //此填充区域已填充
    float dis = startVertex.p2d.distance(endPoint);
    if (dis > fillInfo->maxLen || dis < fillInfo->minLen) return; //不在长度允许的范围内

    int friendId = fillInfo->getFriendId(startVertex.vId);
    VertexInfo friendVertex;
    if (!question->getVertexById(friendId, friendVertex)) return;

    Vec2 v = original2dToScreen2d(friendVertex.p2d);
    Vec2 vect1 = startVertex.p2d - v; //friendVertex指向startVertex向量
    Vec2 vect2 = endPoint - startVertex.p2d; //endPoint指向startVertex向量
    float dot = vect1.dot(vect2);
    float degree = CC_RADIANS_TO_DEGREES(acosf(dot / (vect1.getLength() * vect2.getLength())));
    if (degree < 40) { //满足填补条件
        vector<Vec2> fillVecs = fillInfo->fillVecs;
        vector<VertexInfo> vertexs;
        for (Vec2 v : fillVecs) {
            vertexs.push_back(VertexInfo(-1, v));
        }
        createSolid3D(vertexs);
        fillInfo->isFilled = true;
    }
}

Vec2 FillLayer::original2dToScreen2d(const Vec2 &originalVec) {
    Vec3 v3 = poly2solid(originalVec, 0.5f);
    return cc3DLayer->_camera->projectGL(v3);
}

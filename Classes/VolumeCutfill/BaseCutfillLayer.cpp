//
//  BaseCutfillLayer.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2019/2/21.
//

#include "BaseCutfillLayer.h"

USING_NS_CC;
using namespace std;

BaseCutfillLayer::BaseCutfillLayer()
: touchEnabled(false)
, _3dContainer(nullptr)
, drawNodeEx(nullptr)
, curSolid3D(nullptr) {

}

BaseCutfillLayer::~BaseCutfillLayer() {

}

void BaseCutfillLayer::onEnter() {
    Layer::onEnter();
}

void BaseCutfillLayer::onExit() {
    Layer::onExit();
}

bool BaseCutfillLayer::init() {
    if (!Layer::init()) {
        return false;
    }

    _3dContainer = Node::create();
    _3dContainer->setContentSize(Size(1024, 768));
    addChild(_3dContainer);

    drawNodeEx = DrawNodeEx::create();
    addChild(drawNodeEx);

    return true;
}

void BaseCutfillLayer::setData(CC3DLayer* cc3DLayer, VolumeQuestion* question) {

    clear();

    this->cc3DLayer = cc3DLayer;
    this->question = question;

    createSolid3D(question->vertexList);
}

Solid3D* BaseCutfillLayer::createSolid3D(const vector<VertexInfo> &vecs) {
    Solid3D* solid3D = new Solid3D();
    solid3D->setCameraMask((int)CameraFlag::USER1);
    solid3D->setData(cc3DLayer, vecs);
    solid3D->showModel();
    _3dContainer->addChild(solid3D);
    solid3DList.pushBack(solid3D);
    return solid3D;
}

VertexInfo BaseCutfillLayer::getStartPoint(const Vec2 &point) {
    VertexInfo rs;
    int index = 0;
    float minDis = 1000;
    for (int i = 0; i < solid3DList.size(); i++) {
        Solid3D* Solid3D = solid3DList.at(i);
        float dis;
        VertexInfo p;
        Solid3D->getMinDisPoint(point, Vec2(-1, -1), 1024, p, dis);
        if (dis < minDis) {
            index = i;
            rs = p;
            minDis = dis;
        }
    }
    curSolid3D = solid3DList.at(index);
    return rs;
}

void BaseCutfillLayer::toJson(rapidjson::Document &json) {

}

void BaseCutfillLayer::fromJson(const rapidjson::Value &json) {

}

void BaseCutfillLayer::reset() {
    clear();
    question->reset();

    createSolid3D(question->vertexList);
}

void BaseCutfillLayer::clear() {
    curSolid3D = nullptr;
    drawNodeEx->clear();
    _3dContainer->removeAllChildren();
    solid3DList.clear();
}

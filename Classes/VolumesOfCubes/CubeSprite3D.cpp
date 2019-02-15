//
//  CubeSprite3D.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2019/1/22.
//

#include "CubeSprite3D.h"

USING_NS_CC;
using namespace std;

CubeSprite3D::CubeSprite3D()
: modelName("")
, drawNode3D(nullptr)
, borderColor(Color4F::BLACK) {

}

CubeSprite3D::CubeSprite3D(const string &modelName)
: CubeSprite3D() {
    this->modelName = modelName;
}

CubeSprite3D::~CubeSprite3D() {

}

void CubeSprite3D::onEnter() {
    Sprite3D::onEnter();
}

void CubeSprite3D::onExit() {
    Sprite3D::onExit();
}

bool CubeSprite3D::init() {
    if (!Sprite3D::init()) {
        return false;
    }

    if (!modelName.empty()) {
        initWithFile(modelName);
    }

    drawNode3D = DrawNode3D::create();
    drawNode3D->setCameraMask((int)CameraFlag::USER1);
    addChild(drawNode3D);

    return true;
}

CubeSprite3D* CubeSprite3D::create(std::string modelName) {
    CubeSprite3D* cubeSprite3D = new (std::nothrow)CubeSprite3D(modelName);
    if (cubeSprite3D && cubeSprite3D->init()) {
        cubeSprite3D->_contentSize = cubeSprite3D->getBoundingBox().size;
        cubeSprite3D->autorelease();
        return cubeSprite3D;
    }
    CC_SAFE_DELETE(cubeSprite3D);
    return nullptr;
}

void CubeSprite3D::setBorderColor(const Color4F &borderColor) {
    this->borderColor = borderColor;
    drawCube();
}

void CubeSprite3D::initVertexs() {

    float offset = getContentSize().width / 2;

    vertexList.clear();

    //八个顶点
    vertexList.push_back(Vec3(-offset, offset, offset));
    vertexList.push_back(Vec3(offset, offset, offset));
    vertexList.push_back(Vec3(offset, -offset, offset));
    vertexList.push_back(Vec3(-offset, -offset, offset));

    vertexList.push_back(Vec3(-offset, offset, -offset));
    vertexList.push_back(Vec3(offset, offset, -offset));
    vertexList.push_back(Vec3(offset, -offset, -offset));
    vertexList.push_back(Vec3(-offset, -offset, -offset));

    drawCube();
}

void CubeSprite3D::drawCube() {
    drawNode3D->clear();

    //上面
    drawNode3D->drawLine(vertexList[0], vertexList[1], borderColor);
    drawNode3D->drawLine(vertexList[1], vertexList[2], borderColor);
    drawNode3D->drawLine(vertexList[2], vertexList[3], borderColor);
    drawNode3D->drawLine(vertexList[3], vertexList[0], borderColor);

    //下面
    drawNode3D->drawLine(vertexList[4], vertexList[5], borderColor);
    drawNode3D->drawLine(vertexList[5], vertexList[6], borderColor);
    drawNode3D->drawLine(vertexList[6], vertexList[7], borderColor);
    drawNode3D->drawLine(vertexList[7], vertexList[4], borderColor);

    //侧面
    drawNode3D->drawLine(vertexList[0], vertexList[4], borderColor);
    drawNode3D->drawLine(vertexList[1], vertexList[5], borderColor);
    drawNode3D->drawLine(vertexList[2], vertexList[6], borderColor);
    drawNode3D->drawLine(vertexList[3], vertexList[7], borderColor);
}

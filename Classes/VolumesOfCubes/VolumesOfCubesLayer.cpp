//
//  VolumesOfCubesLayer.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2019/1/15.
//

#include "VolumesOfCubesLayer.h"
#include "Cube3D.h"
#include "CC3DLayer.h"
#include "CC3DMath.h"
#include "CubeSprite3D.h"

USING_NS_CC;

inline cocos2d::Vec2 GLNormalized(const cocos2d::Vec2 &v) {
    cocos2d::Size size = cocos2d::Size(1024, 768);
    return cocos2d::Vec2((2.0f * v.x / size.width), (2.0f * v.y / size.height));
}

const Color4F DefaultColor = Color4F(76 / 255.0, 198 / 255.0, 198 / 255.0, 1);

VolumesOfCubesLayer::VolumesOfCubesLayer()
: cc3DLayer(nullptr)
, camtarget(0, 0, 0)
, camoffset(0, 0, 24) {

}

VolumesOfCubesLayer::~VolumesOfCubesLayer() {

}

void VolumesOfCubesLayer::onEnter() {
    BaseLayer::onEnter();
    
    touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(VolumesOfCubesLayer::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(VolumesOfCubesLayer::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(VolumesOfCubesLayer::onTouchEnded, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void VolumesOfCubesLayer::onExit() {
    if (touchListener != nullptr) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(touchListener);
    }
    
    BaseLayer::onExit();
}

bool VolumesOfCubesLayer::init() {
    if (!BaseLayer::init()) {
        return false;
    }
    
    auto ca = Camera::create();
    ca->setDepth(-1);
    ca->setCameraFlag(CameraFlag::USER2);
    addChild(ca);
    
    Sprite* bg = Sprite::create("white2.png");
    bg->setCameraMask((int)ca->getCameraFlag());
    bg->setAnchorPoint(Vec2::ZERO);
    bg->setScale(1024, 768);
    addChild(bg);

    cc3DLayer = CC3DLayer::create();
    cc3DLayer->userCameraFlag(CameraFlag::USER1);
    addChild(cc3DLayer);
    cc3DLayer->addChild(AmbientLight::create(Color3B(160, 160, 160)));
    light = DirectionLight::create(Vec3(1.0f, -3.0f, -5.0f).getNormalized(), Color3B(100, 100, 100));
    cc3DLayer->_camera->addChild(light);
    cc3DLayer->setCamLoc(camtarget, camquat, camoffset);

    createCube(Vec3::ZERO, Color3B::YELLOW);
    createCube(Vec3(2, 0, 0), Color3B::YELLOW);
    createCube(Vec3(4, 0, 0), Color3B::YELLOW);
    createCube(Vec3(6, 0, 0), Color3B::YELLOW);
    createCube(Vec3(8, 0, 0), Color3B::YELLOW);

    return true;
}

void VolumesOfCubesLayer::createCube(const Vec3 &position, const Color3B &fillColor) {
    CubeSprite3D* cubeSprite3D = CubeSprite3D::create("cube.obj");
    cubeSprite3D->setCameraMask((int)CameraFlag::USER1);
    cubeSprite3D->setColor(fillColor);
    cubeSprite3D->setPosition3D(position);
    cubeSprite3D->initVertexs();
    addChild(cubeSprite3D);
}

bool VolumesOfCubesLayer::onTouchBegan(Touch* touch, Event* event) {
    if (!isVisible()) return false;
    multitouch.OnTouchDown(touch->getID(), convertTouchToNodeSpaceAR(touch));
    return true;
}

void VolumesOfCubesLayer::onTouchMoved(Touch* touch, Event* event) {
    multitouch.OnTouchMove(touch->getID(), convertTouchToNodeSpaceAR(touch));
    if (multitouch.active) {
        if (multitouch.wasactive) {
            Quaternion qz = Z2Quat(multitouch.mtLast.rad - multitouch.mt.rad);
            camquat = camquat * qz;
        }
    } else {
        Vec2 t1 = GLNormalized(convertToNodeSpaceAR(touch->getLocation()));
        
        Vec2 delta = GLNormalized(touch->getDelta());
        Quaternion qx = X2Quat(delta.y * 2.0f * (1 - (t1.x * t1.x)));
        Quaternion qy = Y2Quat(-delta.x * 2.0f * (1 - (t1.y * t1.y)));
        
        Vec2 t2 = GLNormalized(convertToNodeSpaceAR(touch->getPreviousLocation()));
        float r1 = atan2f(t1.y, t1.x);
        float r2 = atan2f(t2.y, t2.x);
        camquat = camquat * (qx * qy) * Z2Quat(RadWrap(r2 - r1) * t1.lengthSquared());
    }
    cc3DLayer->camquat = camquat;
    cc3DLayer->updateCamLoc();
}

void VolumesOfCubesLayer::onTouchEnded(Touch* touch, Event* event) {
    multitouch.OnTouchUp(touch->getID(), convertTouchToNodeSpaceAR(touch));
}

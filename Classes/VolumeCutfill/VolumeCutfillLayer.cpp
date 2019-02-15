//
//  VolumeCutfillLayer.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2019/2/12.
//

#include "VolumeCutfillLayer.h"
#include "CC3DLayer.h"
#include "cG3DefModelGen.h"
#include "CC2D3Util.h"
#include "Volume3D.h"

USING_NS_CC;
using namespace std;

VolumeCutfillLayer::VolumeCutfillLayer()
: cc3DLayer(nullptr)
, camtarget(0, 0, 0)
, camoffset(0, 0, 3)
, camquat(0.222790, 0.015957, 0.096368, 0.969961)
, canRotateCamera(false)
, curVolume3D(nullptr) {
    
}

VolumeCutfillLayer::~VolumeCutfillLayer() {
    
}

void VolumeCutfillLayer::onEnter() {
    BaseLayer::onEnter();
    
    touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(VolumeCutfillLayer::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(VolumeCutfillLayer::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(VolumeCutfillLayer::onTouchEnded, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void VolumeCutfillLayer::onExit() {
    if (touchListener != nullptr) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(touchListener);
    }
    
    BaseLayer::onExit();
}

bool VolumeCutfillLayer::init() {
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

    drawNode = DrawNode::create();
    cc3DLayer->addChild(drawNode);

    //test data
    vector<Vec2> defaultVecs;
    defaultVecs.push_back(Vec2(360, 455));
    defaultVecs.push_back(Vec2(600, 455));
    defaultVecs.push_back(Vec2(600, 213));
    defaultVecs.push_back(Vec2(520, 213));
    defaultVecs.push_back(Vec2(520, 355));
    defaultVecs.push_back(Vec2(440, 355));
    defaultVecs.push_back(Vec2(440, 213));
    defaultVecs.push_back(Vec2(360, 213));

    createVolume3D(defaultVecs);

    return true;
}

Vec2 startPoint = Vec2(-1, -1);

bool VolumeCutfillLayer::onTouchBegan(Touch* touch, Event* event) {
    if (!isVisible()) return false;
    if (canRotateCamera) {
        multitouch.OnTouchDown(touch->getID(), convertTouchToNodeSpaceAR(touch));
    } else {
        startPoint = getStartPoint(touch->getLocation());
    }
    return true;
}

void VolumeCutfillLayer::onTouchMoved(Touch* touch, Event* event) {
    multitouch.OnTouchMove(touch->getID(), convertTouchToNodeSpaceAR(touch));
    if (canRotateCamera) { //旋转摄像头
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
    } else { //画线
        drawNode->clear();
        drawNode->drawLine(startPoint, touch->getLocation(), Color4F::RED);
    }
}

void VolumeCutfillLayer::onTouchEnded(Touch* touch, Event* event) {
    if (canRotateCamera) {
        multitouch.OnTouchUp(touch->getID(), convertTouchToNodeSpaceAR(touch));
    } else {
        drawNode->clear();
        Vec2 lastPoint = getEndPoint(touch->getLocation(), startPoint);
        drawNode->drawLine(startPoint, lastPoint, Color4F::RED);

        if (curVolume3D != nullptr) {
            vector<Vec2> leftVecList, rightVecList;
            curVolume3D->cutPolygon(startPoint, lastPoint, leftVecList, rightVecList);
            curVolume3D->clear();
            volume3DList.eraseObject(curVolume3D);
            CC_SAFE_RELEASE_NULL(curVolume3D);

            drawNode->clear();

            createVolume3D(leftVecList, Vec3(-0.05f, 0, 0), true);
            createVolume3D(rightVecList, Vec3(0.05f, 0, 0), true);
            
            if (volume3DList.size() > 2)
            volume3DList.at(0)->playTranslateAnim(0.3f, Vec3(0.05f, 0, 0));
        }
    }
}

Vec2 VolumeCutfillLayer::getStartPoint(const Vec2 &point) {
    if (volume3DList.size() < 1) {
        return point;
    }

    Vec2 rsPoint;
    int index = 0;
    float minDis = 1000;
    for (int i = 0; i < volume3DList.size(); i++) {
        Volume3D* volume3D = volume3DList.at(i);
        float dis;
        Vec2 p;
        volume3D->findMinDisPoint(point, Vec2(-1, -1), p, dis);
        if (dis < minDis) {
            index = i;
            rsPoint = p;
            minDis = dis;
        }
    }
    curVolume3D = volume3DList.at(index);
    return rsPoint;
}

Vec2 VolumeCutfillLayer::getEndPoint(const Vec2 &point, const Vec2 &startPoint) {
    if (curVolume3D == nullptr) return point;
    float dis;
    Vec2 rsPoint;
    curVolume3D->findMinDisPoint(point, startPoint, rsPoint, dis);
    return rsPoint;
}

void VolumeCutfillLayer::createVolume3D(const vector<Vec2> &vecs, const Vec3 &offset, bool playAnim) {
    Volume3D* volume3D = new Volume3D();
    volume3D->setData(cc3DLayer, vecs);
    volume3D->showModel();
    volume3DList.pushBack(volume3D);

    volume3D->playTranslateAnim(playAnim ? 0.3f : 0.01f, offset);
}

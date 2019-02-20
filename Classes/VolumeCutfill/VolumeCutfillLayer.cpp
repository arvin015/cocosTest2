//
//  VolumeCutfillLayer.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2019/2/12.
//

#include "VolumeCutfillLayer.h"
#include "cG3DefModelGen.h"
#include "CC2D3Util.h"
#include "Volume3D.h"
#include "BasicData.h"
#include "MathUtils.h"

USING_NS_CC;
using namespace std;

//namespace VolumeCutfill {

    const float MaxDis = 50;

    VolumeCutfillLayer::VolumeCutfillLayer()
    : cc3DLayer(nullptr)
    , camtarget(0, 0, 0)
    , camoffset(0, 0, 3)
    , camquat(0.222790, 0.015957, 0.096368, 0.969961)
    , canRotateCamera(false)
    , curVolume3D(nullptr)
    , drawNodeEx(nullptr)
    , resetBtn(nullptr) {

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

        resetBtn = Button::create();
        resetBtn->setTitleText("重置");
        resetBtn->setTitleColor(Color3B::BLACK);
        resetBtn->setTitleFontSize(22);
        resetBtn->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        resetBtn->setPosition(Vec2(ScreenSize().width / 2, 10));
        resetBtn->addClickEventListener([this](Ref* pSender) {
            reset();
        });
        addChild(resetBtn);

        cc3DLayer = CC3DLayer::create();
        cc3DLayer->userCameraFlag(CameraFlag::USER1);
        addChild(cc3DLayer);
        cc3DLayer->addChild(AmbientLight::create(Color3B(160, 160, 160)));
        light = DirectionLight::create(Vec3(1.0f, -3.0f, -5.0f).getNormalized(), Color3B(100, 100, 100));
        cc3DLayer->_camera->addChild(light);
        cc3DLayer->setCamLoc(camtarget, camquat, camoffset);

        drawNodeEx = DrawNodeEx::create();
        cc3DLayer->addChild(drawNodeEx);

        initDefaultSolid();

        return true;
    }

    void VolumeCutfillLayer::initDefaultSolid() {

        //test data
        vector<VertexInfo> defaultVecs;
        defaultVecs.push_back(VertexInfo(1, Vec2(360, 455)));
        defaultVecs.push_back(VertexInfo(2, Vec2(440, 455))); //非顶点
        defaultVecs.push_back(VertexInfo(3, Vec2(520, 455))); //非顶点
        defaultVecs.push_back(VertexInfo(4, Vec2(600, 455)));
        defaultVecs.push_back(VertexInfo(5, Vec2(600, 355))); //非顶点
        defaultVecs.push_back(VertexInfo(6, Vec2(600, 213)));
        defaultVecs.push_back(VertexInfo(7, Vec2(520, 213)));
        defaultVecs.push_back(VertexInfo(8, Vec2(520, 355)));
        defaultVecs.push_back(VertexInfo(9, Vec2(440, 355)));
        defaultVecs.push_back(VertexInfo(10, Vec2(440, 213)));
        defaultVecs.push_back(VertexInfo(11, Vec2(360, 213)));
        defaultVecs.push_back(VertexInfo(12, Vec2(360, 355))); //非顶点

        createVolume3D(defaultVecs);
    }

    bool VolumeCutfillLayer::onTouchBegan(Touch* touch, Event* event) {
        if (!isVisible()) return false;
        if (canRotateCamera) {
            multitouch.OnTouchDown(touch->getID(), convertTouchToNodeSpaceAR(touch));
        } else {
            startVertex = getStartPoint(touch->getLocation());
            drawNodeEx->drawDashLine(startVertex.p2d, touch->getLocation(), 1.0f, 5, 5, Color4F::BLACK);
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
            drawNodeEx->clear();
            drawNodeEx->drawDashLine(startVertex.p2d, touch->getLocation(), 1.0f, 5, 5, Color4F::BLACK);
        }
    }

    void VolumeCutfillLayer::onTouchEnded(Touch* touch, Event* event) {
        if (canRotateCamera) {
            multitouch.OnTouchUp(touch->getID(), convertTouchToNodeSpaceAR(touch));
        } else {
            drawNodeEx->clear();
            VertexInfo lastVertex;
            if (getEndPoint(touch->getLocation(), startVertex.p2d, lastVertex)) { //切割线满足条件
                drawNodeEx->drawDashLine(startVertex.p2d, lastVertex.p2d, 1.0f, 5, 5, Color4F::BLACK);
                if (curVolume3D && curVolume3D->checkCanCut(startVertex.p2d, lastVertex.p2d)) { //满足切割条件
                    cutSolid(curVolume3D, startVertex, lastVertex);
                }
                drawNodeEx->clear();
            }
        }
    }

    void VolumeCutfillLayer::cutSolid(Volume3D* volume3D, const VertexInfo &ver1, const VertexInfo &ver2) {
        if (volume3D == nullptr) return;

        vector<VertexInfo> vecList1, vecList2;
        volume3D->cutPolygon(ver1.p2d, ver2.p2d, vecList1, vecList2);
        volume3D->clear();
        volume3DList.eraseObject(volume3D);
        CC_SAFE_RELEASE_NULL(volume3D);

        Volume3D* new3d1 = createVolume3D(vecList1);
        Volume3D* new3d2 = createVolume3D(vecList2);

        Volume3D::TRANS_DIRECTION direction = getTransDirection(ver1.p2d, ver2.p2d);

        //特殊处理---FUCK
        if ((ver1.vId == 6 && ver2.vId == 8) || (ver1.vId == 8 && ver2.vId == 6) ||
                (ver1.vId == 9 && ver2.vId == 11) || (ver1.vId == 11 && ver2.vId == 9)) {

            new3d1->playTranslateAnim(Volume_Duration, Vec3(0, Volume_MoveByValue, 0));
            new3d2->playTranslateAnim(Volume_Duration, Vec3(0, -Volume_MoveByValue, 0));

            for (int i = 0; i < volume3DList.size() - 2; i++) {
                volume3DList.at(i)->playTranslateAnim(Volume_Duration, Vec3(0, Volume_MoveByValue, 0));
            }

        } else {
            for (int i = 0; i < volume3DList.size(); i++) {
                volume3DList.at(i)->playTransAnim(Volume_Duration, ver1.p2d, ver2.p2d, direction);
            }
        }
    }

    Volume3D::TRANS_DIRECTION VolumeCutfillLayer::getTransDirection(const Vec2 &p1, const Vec2 &p2) {
        float slope = getSlope(p1, p2);
        if (slope >= 0.5f || slope <= -0.5f) { //45°-135°左右方向
            return Volume3D::HOR;
        } else return Volume3D::VER;
    }

    VertexInfo VolumeCutfillLayer::getStartPoint(const Vec2 &point) {
        VertexInfo rs;
        int index = 0;
        float minDis = 1000;
        for (int i = 0; i < volume3DList.size(); i++) {
            Volume3D* volume3D = volume3DList.at(i);
            float dis;
            VertexInfo p;
            volume3D->getMinDisPoint(point, Vec2(-1, -1), 1024, p, dis);
            if (dis < minDis) {
                index = i;
                rs = p;
                minDis = dis;
            }
        }
        curVolume3D = volume3DList.at(index);
        return rs;
    }

    bool VolumeCutfillLayer::getEndPoint(const Vec2 &point, const Vec2 &startPoint, VertexInfo &rs) {
        if (curVolume3D == nullptr) return false;
        float dis;
        VertexInfo ver;
        if (curVolume3D->getMinDisPoint(point, startPoint, MaxDis, ver, dis)) {
            rs = ver;
            return true;
        }
        return false;
    }

    Volume3D* VolumeCutfillLayer::createVolume3D(const vector<VertexInfo> &vecs) {
        Volume3D* volume3D = new Volume3D();
        volume3D->setData(cc3DLayer, vecs);
        volume3D->showModel();
        volume3DList.pushBack(volume3D);
        return volume3D;
    }

    void VolumeCutfillLayer::reset() {
        for (Volume3D* volume3D : volume3DList) {
            volume3D->clear();
            CC_SAFE_RELEASE_NULL(volume3D);
        }
        volume3DList.clear();
        drawNodeEx->clear();
        initDefaultSolid();
    }
//}

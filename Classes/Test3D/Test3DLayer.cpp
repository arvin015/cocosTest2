//
//  Test3DLayer.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2018/10/30.
//

#include "Test3DLayer.h"
#include "CC3DMath.h"
#include "Fold3DLoader.h"

namespace Test3D {

    using namespace std;

    inline Vec2 GLNormalized(const Vec2 &v) {
        cocos2d::Size size = Size(V_WIDTH, V_HEIGHT);
        return Vec2((2.0f * v.x / size.width), (2.0f * v.y / size.height));
    }

    Test3DLayer::Test3DLayer()
    : camtarget(0,0,0),
      camoffset(0,0,8),
      rootS3d(nullptr),
      redS3d(nullptr),
      blueS3d(nullptr),
      greenS3d(nullptr),
      orangeS3d(nullptr),
      grayS3d(nullptr),
      curTime(0),
      isPlaying(false),
      isStepByStep(false),
      stepFoldBtn(nullptr),
      foldBtn(nullptr) {

    }

    Test3DLayer::~Test3DLayer() {

    }

    void Test3DLayer::onEnter() {
        BaseLayer::onEnter();

        touchListener = EventListenerTouchOneByOne::create();
        touchListener->onTouchBegan = CC_CALLBACK_2(Test3DLayer::onTouchBegan, this);
        touchListener->onTouchMoved = CC_CALLBACK_2(Test3DLayer::onTouchMoved, this);
        touchListener->onTouchEnded = CC_CALLBACK_2(Test3DLayer::onTouchEnded, this);
        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);

        scheduleUpdate();
    }

    void Test3DLayer::onExit() {

        if (touchListener != nullptr) {
            Director::getInstance()->getEventDispatcher()->removeEventListener(touchListener);
        }

        BaseLayer::onExit();
    }

    void Test3DLayer::update(float dt) {

        if (isPlaying) {
            if (!isStepByStep) {
                if (redS3d->getRotation3D().x <= -90) {
                } else {

                    curTime += dt * 0.3f;
                    rootS3d->setPosition3D(rootOriginalPos + curTime * (dstPos - rootOriginalPos));

                    redS3d->setRotation3D(Vec3(redS3d->getRotation3D().x - 0.5, redS3d->getRotation3D().y, redS3d->getRotation3D().z));
                    blueS3d->setRotation3D(Vec3(blueS3d->getRotation3D().x - 0.5, blueS3d->getRotation3D().y, blueS3d->getRotation3D().z));
                    greenS3d->setRotation3D(Vec3(greenS3d->getRotation3D().x - 0.5, greenS3d->getRotation3D().y, greenS3d->getRotation3D().z));
                    orangeS3d->setRotation3D(Vec3(orangeS3d->getRotation3D().x - 0.5, orangeS3d->getRotation3D().y, orangeS3d->getRotation3D().z));
                    grayS3d->setRotation3D(Vec3(grayS3d->getRotation3D().x - 0.5, grayS3d->getRotation3D().y, grayS3d->getRotation3D().z));
                }
            }
        }
    }

    bool Test3DLayer::init() {
        if (!BaseLayer::init()) {
            return false;
        }

        const CameraFlag bgcamflag = CameraFlag::USER2;
        auto ca = Camera::create();
        ca->setDepth(-1);
        ca->setCameraFlag(bgcamflag);
        addChild(ca);

        auto bg = ui::ImageView::create("white.png");
        bg->setContentSize(Size(V_WIDTH, V_HEIGHT));
        bg->setAnchorPoint(Vec2::ZERO);
        bg->setPosition(Vec2::ZERO);
        bg->setCameraMask((int)bgcamflag);
        bg->setScale9Enabled(true);
        addChild(bg);

        auto layer = Layer::create();
        addChild(layer);

        layer->addChild(AmbientLight::create(Color3B(160, 160, 160)));

        camera = Camera::createPerspective(60, (GLfloat)V_WIDTH / V_HEIGHT, 1, 1000);
        camera->setCameraFlag(CameraFlag::USER1);
        layer->addChild(camera);

        layer->setCameraMask((int)CameraFlag::USER1);

        camera->setPosition3D(Vec3(0, 0, 8));
        camera->lookAt(Vec3(0, 0, 0));

        rootS3d = Sprite3D::create("square_back.obj");
        rootS3d->setTexture("mappic_empty.png");
        rootS3d->setCameraMask((int)CameraFlag::USER1);
        rootS3d->setPosition3D(Vec3(1, -1, 0));
        layer->addChild(rootS3d);

        auto rootS3d1 = Sprite3D::create("square_front.obj");
        rootS3d1->setTexture("mappic_coloring.png");
        rootS3d1->setColor(Color3B::YELLOW);
        rootS3d1->setCameraMask((int)CameraFlag::USER1);
        rootS3d1->setPosition3D(Vec3(0, 1, 0));
        rootS3d->addChild(rootS3d1);

        redS3d = Sprite3D::create("square_back.obj");
        redS3d->setTexture("mappic_empty.png");
        redS3d->setCameraMask((int)CameraFlag::USER1);
        redS3d->setPosition3D(Vec3(-1, 1, 0));
        redS3d->setRotation3D(Vec3(0, 0, -90));
        rootS3d->addChild(redS3d);

        auto redS3d1 = Sprite3D::create("square_front.obj");
        redS3d1->setTexture("mappic_coloring.png");
        redS3d1->setColor(Color3B::RED);
        redS3d1->setCameraMask((int)CameraFlag::USER1);
        redS3d1->setPosition3D(Vec3(0, 1, 0));
        redS3d->addChild(redS3d1);

        greenS3d = Sprite3D::create("square_back.obj");
        greenS3d->setTexture("mappic_empty.png");
        greenS3d->setCameraMask((int)CameraFlag::USER1);
        greenS3d->setPosition3D(Vec3(1, 1, 0));
        greenS3d->setRotation3D(Vec3(0, 0, 90));
        redS3d->addChild(greenS3d);

        auto greenS3d1 = Sprite3D::create("square_front.obj");
        greenS3d1->setTexture("mappic_coloring.png");
        greenS3d1->setColor(Color3B::GREEN);
        greenS3d1->setCameraMask((int)CameraFlag::USER1);
        greenS3d1->setPosition3D(Vec3(0, 1, 0));
        greenS3d->addChild(greenS3d1);

        orangeS3d = Sprite3D::create("square_back.obj");
        orangeS3d->setTexture("mappic_empty.png");
        orangeS3d->setCameraMask((int)CameraFlag::USER1);
        orangeS3d->setPosition3D(Vec3(0, 2, 0));
        redS3d->addChild(orangeS3d);

        auto orangeS3d1 = Sprite3D::create("square_front.obj");
        orangeS3d1->setTexture("mappic_coloring.png");
        orangeS3d1->setColor(Color3B::ORANGE);
        orangeS3d1->setCameraMask((int)CameraFlag::USER1);
        orangeS3d1->setPosition3D(Vec3(0, 1, 0));
        orangeS3d->addChild(orangeS3d1);

        blueS3d = Sprite3D::create("square_back.obj");
        blueS3d->setTexture("mappic_empty.png");
        blueS3d->setCameraMask((int)CameraFlag::USER1);
        blueS3d->setPosition3D(Vec3(1, 1, 0));
        blueS3d->setRotation3D(Vec3(0, 0, 90));
        rootS3d->addChild(blueS3d);

        auto blueS3d1 = Sprite3D::create("square_front.obj");
        blueS3d1->setTexture("mappic_coloring.png");
        blueS3d1->setColor(Color3B::BLUE);
        blueS3d1->setCameraMask((int)CameraFlag::USER1);
        blueS3d1->setPosition3D(Vec3(0, 1, 0));
        blueS3d->addChild(blueS3d1);

        grayS3d = Sprite3D::create("square_back.obj");
        grayS3d->setTexture("mappic_empty.png");
        grayS3d->setCameraMask((int)CameraFlag::USER1);
        grayS3d->setPosition3D(Vec3(0, 0, 0));
        grayS3d->setRotation3D(Vec3(0, 0, 180));
        rootS3d->addChild(grayS3d);

        auto grayS3d1 = Sprite3D::create("square_front.obj");
        grayS3d1->setTexture("mappic_coloring.png");
        grayS3d1->setColor(Color3B::GRAY);
        grayS3d1->setCameraMask((int)CameraFlag::USER1);
        grayS3d1->setPosition3D(Vec3(0, 1, 0));
        grayS3d->addChild(grayS3d1);

        Vec2 p = camera->projectGL(rootS3d->getPosition3D());
        log("--------- s3d.x = %f, s3d.y = %f, w = %f, h = %f", p.x, p.y, rootS3d->getContentSize().width, rootS3d->getContentSize().height);

        rootOriginalPos = rootS3d->getPosition3D();
        dstPos = Vec3(0, -1, 0);

        foldBtn = ui::Button::create("mian_button_01_125x54.png");
        foldBtn->setPosition(Vec2(V_WIDTH / 2, 5));
        foldBtn->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        foldBtn->setTitleText("折叠");
        foldBtn->setTitleFontSize(22);
        foldBtn->addClickEventListener([this](Ref* pSender) {
            isPlaying = true;
            foldBtn->setEnabled(false);
            stepFoldBtn->setEnabled(false);
        });
        addChild(foldBtn);

        stepFoldBtn = ui::Button::create("mian_button_01_125x54.png");
        stepFoldBtn->setPosition(Vec2(V_WIDTH / 2, 65));
        stepFoldBtn->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        stepFoldBtn->setTitleText("分步折叠");
        stepFoldBtn->setTitleFontSize(22);
        stepFoldBtn->addClickEventListener([this](Ref* pSender) {
            isPlaying = true;
            isStepByStep = true;
            stepFoldBtn->setEnabled(false);
            foldBtn->setEnabled(false);

            foldQueue.push(redS3d);
            foldQueue.push(blueS3d);
            foldQueue.push(grayS3d);
            foldQueue.push(orangeS3d);
            foldQueue.push(greenS3d);

            foldPaper();

        });
        addChild(stepFoldBtn);

        return true;
    }

    bool Test3DLayer::onTouchBegan(Touch* touch, Event* event) {
        multitouch.OnTouchDown(touch->getID(), convertTouchToNodeSpaceAR(touch));
        return true;
    }

    void Test3DLayer::onTouchMoved(Touch* touch, Event* event) {
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
//        cc3dlayer->camquat = camquat;
//        cc3dlayer->updateCamLoc();
        camera->setRotationQuat(camquat);
        camera->setPosition3D(camtarget + (camquat * camoffset));
    }

    void Test3DLayer::onTouchEnded(Touch* touch, Event* event) {
        multitouch.OnTouchUp(touch->getID(), convertTouchToNodeSpaceAR(touch));
    }

    void Test3DLayer::foldPaper() {
        Sprite3D* s3d = foldQueue.front();
        if (s3d != nullptr) {
            play3DAnim(s3d, [this]() {
                foldQueue.pop();
                if (!foldQueue.empty()) {
                    foldPaper();
                }
            });
        }
    }

    void Test3DLayer::play3DAnim(Sprite3D* s3d, function<void()> callback) {
        auto anim = Sequence::create(Spawn::create(TargetedAction::create(s3d, RotateTo::create(2, Vec3(s3d->getRotation3D().x - 90, s3d->getRotation3D().y, s3d->getRotation3D().z))),
                                                   TargetedAction::create(rootS3d, MoveTo::create(2, Vec3(0, -1, 0))), NULL), CallFunc::create([callback]() {
            callback();
        }), NULL);
        runAction(anim);
    }
}


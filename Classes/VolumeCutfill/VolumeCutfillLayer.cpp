//
//  VolumeCutfillLayer.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2019/2/12.
//

#include "VolumeCutfillLayer.h"
#include "VolumeQuestion.h"
#include "MathUtils.h"
#include "CommonUtils.h"
#include "CC3DMath.h"
#include "Geom3D.h"

USING_NS_CC;
using namespace ui;
using namespace std;

string savePath;

inline cocos2d::Vec2 GLNormalized(const cocos2d::Vec2 &v) {
    cocos2d::Size size = cocos2d::Size(1024, 768);
    return cocos2d::Vec2((2.0f * v.x / size.width), (2.0f * v.y / size.height));
}

VolumeCutfillLayer::VolumeCutfillLayer()
: cc3DLayer(nullptr)
, camtarget(0, 0, 0)
, camoffset(0, 0, 3)
, cutBtn(nullptr)
, fillBtn(nullptr)
, resetBtn(nullptr)
, workTypeQ1(NONE)
, workTypeQ2(NONE)
, currentLayer(nullptr)
, cutLayer(nullptr)
, fillLayer(nullptr)
, currentQId(-1)
, cubeSp3d(nullptr)
, coneSp3d(nullptr)
, sphereSp3d(nullptr) {

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

    savePath = FileUtils::getInstance()->getWritablePath();

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

    cutLayer = CutLayer::create();
    cutLayer->setVisible(false);
    cc3DLayer->addChild(cutLayer);

    fillLayer = FillLayer::create();
    fillLayer->setVisible(false);
    cc3DLayer->addChild(fillLayer);

    currentLayer = cutLayer;
    
    cutBtn = Button::create();
    cutBtn->setTitleFontSize(24);
    cutBtn->setTitleColor(Color3B::BLACK);
    cutBtn->setTitleText("切割");
    cutBtn->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    cutBtn->setPosition(Vec2(20, 600));
    cutBtn->addClickEventListener([this](Ref* pSender) {
        setWorkType(CUT, false);
    });
    addChild(cutBtn);
    
    fillBtn = Button::create();
    fillBtn->setTitleFontSize(24);
    fillBtn->setTitleColor(Color3B::BLACK);
    fillBtn->setTitleText("填补");
    fillBtn->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    fillBtn->setPosition(Vec2(20, 560));
    fillBtn->addClickEventListener([this](Ref* pSender) {
        setWorkType(FILL, false);
    });
    addChild(fillBtn);
    
    resetBtn = Button::create();
    resetBtn->setTitleText("重置");
    resetBtn->setTitleColor(Color3B::BLACK);
    resetBtn->setTitleFontSize(24);
    resetBtn->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    resetBtn->setPosition(Vec2(1024 / 2, 10));
    resetBtn->addClickEventListener([this](Ref* pSender) {
        reset();
    });
    addChild(resetBtn);
    
    auto radioGroup = RadioButtonGroup::create();
    radioGroup->addEventListener([this](RadioButton* radioButton, int index, RadioButtonGroup::EventType) {
        if (currentQId != -1) {
           saveRecord(); //保存上一题
        }
        loadQuestion(index + 1);
        loadRecord(); //加载当前题旧记录
    });
    addChild(radioGroup);
    auto question1 = RadioButton::create("img_question1.png", "img_question1_selected.png");
    question1->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
    question1->setPosition(Vec2(1024 - 20, 768 - 30));
    radioGroup->addRadioButton(question1);
    radioGroup->addChild(question1);
    auto question2 = RadioButton::create("img_question2.png", "img_question2_selected.png");
    question2->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
    question2->setPosition(Vec2(1024 - 20, 768 - 70));
    radioGroup->addRadioButton(question2);
    radioGroup->addChild(question2);
    radioGroup->setSelectedButton(0);

    cubeBtn = Button::create();
    cubeBtn->setPosition(Vec2(20, 200));
    cubeBtn->setAnchorPoint(Vec2::ZERO);
    cubeBtn->setTitleColor(Color3B::BLACK);
    cubeBtn->setTitleText("立方体");
    cubeBtn->setTitleFontSize(24);
    cubeBtn->addClickEventListener([this](Ref* pSender) {
        if (curType3D == CUBE) return;

        if (cubeSp3d == nullptr) {
            cubeSp3d = addSprite3DModel(loadModelCube(0.5f), cc3DLayer, Vec3::ZERO);
            cubeSp3d->setColor(Color3B::YELLOW);
        }
        cubeSp3d->setVisible(true);

        camquat.set(0.169140, 0.088621, 0.079981, 0.978338);
        cc3DLayer->setCamLoc(camtarget, camquat, camoffset);

        curType3D = CUBE;

        cubeBtn->setTitleColor(Color3B::BLUE);
        coneBtn->setTitleColor(Color3B::BLACK);
        sphereBtn->setTitleColor(Color3B::BLACK);

        if (coneSp3d) coneSp3d->setVisible(false);
        if (sphereSp3d) sphereSp3d->setVisible(false);
    });
    addChild(cubeBtn);

    coneBtn = Button::create();
    coneBtn->setAnchorPoint(Vec2::ZERO);
    coneBtn->setPosition(Vec2(20, 150));
    coneBtn->setTitleColor(Color3B::BLACK);
    coneBtn->setTitleText("圆锥体");
    coneBtn->setTitleFontSize(24);
    coneBtn->addClickEventListener([this](Ref* pSender) {
        if (curType3D == CONE) return;

        if (coneSp3d == nullptr) {
            coneSp3d = addSprite3DModel(loadModelCone(128, 0.25f, 0.5f), cc3DLayer, Vec3::ZERO);
            coneSp3d->setColor(Color3B::YELLOW);
        }
        coneSp3d->setVisible(true);

        camquat.set(0.169140, 0.088621, 0.079981, 0.978338);
        cc3DLayer->setCamLoc(camtarget, camquat, camoffset);

        curType3D = CONE;

        coneBtn->setTitleColor(Color3B::BLUE);
        cubeBtn->setTitleColor(Color3B::BLACK);
        sphereBtn->setTitleColor(Color3B::BLACK);

        if (cubeSp3d) cubeSp3d->setVisible(false);
        if (sphereSp3d) sphereSp3d->setVisible(false);
    });
    addChild(coneBtn);

    sphereBtn = Button::create();
    sphereBtn->setAnchorPoint(Vec2::ZERO);
    sphereBtn->setPosition(Vec2(20, 100));
    sphereBtn->setTitleColor(Color3B::BLACK);
    sphereBtn->setTitleText("圆体");
    sphereBtn->setTitleFontSize(24);
    sphereBtn->addClickEventListener([this](Ref* pSender) {
        if (curType3D == SPHERE) return;

        if (sphereSp3d == nullptr) {
            sphereSp3d = addSprite3DModel(loadModelSphere(0.5f, 128, 128), cc3DLayer, Vec3::ZERO);
            sphereSp3d->setColor(Color3B::YELLOW);
        }
        sphereSp3d->setVisible(true);

        camquat.set(0.169140, 0.088621, 0.079981, 0.978338);
        cc3DLayer->setCamLoc(camtarget, camquat, camoffset);

        curType3D = SPHERE;

        sphereBtn->setTitleColor(Color3B::BLUE);
        cubeBtn->setTitleColor(Color3B::BLACK);
        coneBtn->setTitleColor(Color3B::BLACK);

        if (cubeSp3d) cubeSp3d->setVisible(false);
        if (coneSp3d) coneSp3d->setVisible(false);
    });
    addChild(sphereBtn);

    return true;
}

void VolumeCutfillLayer::loadQuestion(int qId) {
    this->currentQId = qId;

    if (qId == 1) {
        camquat.set(0.169140, 0.088621, 0.079981, 0.978338);
    } else {
        camquat.set(-0.150073, 0.188259, 0.094490, 0.965988);
    }
    cc3DLayer->setCamLoc(camtarget, camquat, camoffset);

    setWorkType(currentQId == 1 ? workTypeQ1 : workTypeQ2, true);

    VolumeQuestion* question = getQuestionById(qId);
    if (question == nullptr) {
        //test data
        vector<VertexInfo> defaultVecs;
        if (qId == 1) {
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
        } else {
            defaultVecs.push_back(VertexInfo(1, Vec2(340, 584)));
            defaultVecs.push_back(VertexInfo(2, Vec2(500, 584)));
            defaultVecs.push_back(VertexInfo(3, Vec2(500, 384))); //非顶点
            defaultVecs.push_back(VertexInfo(4, Vec2(500, 344)));
            defaultVecs.push_back(VertexInfo(5, Vec2(620, 344)));
            defaultVecs.push_back(VertexInfo(6, Vec2(620, 184)));
            defaultVecs.push_back(VertexInfo(7, Vec2(500, 184))); //非顶点
            defaultVecs.push_back(VertexInfo(8, Vec2(340, 184))); //非顶点
            defaultVecs.push_back(VertexInfo(9, Vec2(260, 184)));
            defaultVecs.push_back(VertexInfo(10, Vec2(260, 344))); //非顶点
            defaultVecs.push_back(VertexInfo(11, Vec2(260, 384)));
            defaultVecs.push_back(VertexInfo(12, Vec2(340, 384)));
        }

        vector<CutInfo> cutInfos;
        if (qId == 1) {
            cutInfos.push_back(CutInfo(6, 8, true, 2));
            cutInfos.push_back(CutInfo(9, 11, true, 2));
        } else {
            cutInfos.push_back(CutInfo(10, 12, true, 1));
            cutInfos.push_back(CutInfo(1, 3, false, 0));
            cutInfos.push_back(CutInfo(1, 4, false, 0));
            cutInfos.push_back(CutInfo(1, 7, false, 0));
            cutInfos.push_back(CutInfo(2, 8, false, 0));
            cutInfos.push_back(CutInfo(2, 9, false, 0));
            cutInfos.push_back(CutInfo(2, 12, false, 0));
            cutInfos.push_back(CutInfo(3, 8, false, 0));
            cutInfos.push_back(CutInfo(3, 9, false, 0));
            cutInfos.push_back(CutInfo(3, 10, false, 0));
            cutInfos.push_back(CutInfo(4, 11, false, 0));
            cutInfos.push_back(CutInfo(4, 12, false, 0));
            cutInfos.push_back(CutInfo(6, 12, false, 0));
            cutInfos.push_back(CutInfo(7, 12, false, 0));
        }

        vector<FillInfo*> fillInfos;
        if (qId == 1) {
            FillInfo* fillInfo = new FillInfo();
            map<int, int> fillVecMap;
            fillVecMap.insert(make_pair(7, 6));
            fillVecMap.insert(make_pair(10, 11));
            fillInfo->fillVecMap = fillVecMap;
            fillInfo->minLen = 20;
            fillInfo->maxLen = 100;
            vector<Vec2> fillVecs;
            fillVecs.push_back(Vec2(520, 213));
            fillVecs.push_back(Vec2(520, 355));
            fillVecs.push_back(Vec2(440, 355));
            fillVecs.push_back(Vec2(440, 213));
            fillInfo->fillVecs = fillVecs;
            fillInfos.push_back(fillInfo);
        } else {
            FillInfo* fillInfo = new FillInfo();
            map<int, int> fillVecMap;
            fillVecMap.insert(make_pair(1, 2));
            fillInfo->fillVecMap = fillVecMap;
            fillInfo->minLen = 20;
            fillInfo->maxLen = 100;
            vector<Vec2> fillVecs;
            fillVecs.push_back(Vec2(260, 584));
            fillVecs.push_back(Vec2(340, 584));
            fillVecs.push_back(Vec2(340, 384));
            fillVecs.push_back(Vec2(260, 384));
            fillInfo->fillVecs = fillVecs;
            fillInfos.push_back(fillInfo);

            FillInfo* fillInfo2 = new FillInfo();
            map<int, int> fillVecMap2;
            fillVecMap2.insert(make_pair(2, 1));
            fillInfo2->fillVecMap = fillVecMap2;
            fillInfo2->minLen = 20;
            fillInfo2->maxLen = 100;
            vector<Vec2> fillVecs2;
            fillVecs2.push_back(Vec2(500, 584));
            fillVecs2.push_back(Vec2(620, 584));
            fillVecs2.push_back(Vec2(620, 344));
            fillVecs2.push_back(Vec2(500, 344));
            fillInfo2->fillVecs = fillVecs2;
            fillInfos.push_back(fillInfo2);

            FillInfo* fillInfo3 = new FillInfo();
            map<int, int> fillVecMap3;
            fillVecMap3.insert(make_pair(3, 12));
            fillInfo3->fillVecMap = fillVecMap3;
            fillInfo3->minLen = 20;
            fillInfo3->maxLen = 100;
            vector<Vec2> fillVecs3;
            fillVecs3.push_back(Vec2(500, 384));
            fillVecs3.push_back(Vec2(620, 384));
            fillVecs3.push_back(Vec2(620, 344));
            fillVecs3.push_back(Vec2(500, 344));
            fillInfo3->fillVecs = fillVecs3;
            fillInfos.push_back(fillInfo3);
        }

        question = new VolumeQuestion();
        question->qId = qId;
        question->vertexList = defaultVecs;
        question->cutInfoList = cutInfos;
        question->fillInfoList = fillInfos;

        questionList.push_back(question);
    }

    cutLayer->setData(cc3DLayer, question);
    fillLayer->setData(cc3DLayer, question);
}

VolumeQuestion* VolumeCutfillLayer::getQuestionById(int qId) {
    for (VolumeQuestion* q : questionList) {
        if (q->qId == qId) {
            return q;
        }
    }
    return nullptr;
}

bool VolumeCutfillLayer::onTouchBegan(Touch* touch, Event* event) {
    if (!isVisible()) return false;
//    return false;
    multitouch.OnTouchDown(touch->getID(), convertTouchToNodeSpaceAR(touch));
    return true;
}

void VolumeCutfillLayer::onTouchMoved(Touch* touch, Event* event) {
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

void VolumeCutfillLayer::onTouchEnded(Touch* touch, Event* event) {
    multitouch.OnTouchUp(touch->getID(), convertTouchToNodeSpaceAR(touch));
}

string VolumeCutfillLayer::toJSON() {
    rapidjson::Document doc;
    doc.SetObject();
    doc.AddMember("currentQId", currentQId, doc.GetAllocator());
    doc.AddMember("workTypeQ1", workTypeQ1, doc.GetAllocator());
    doc.AddMember("workTypeQ2", workTypeQ2, doc.GetAllocator());

    rapidjson::Document doc1(&doc.GetAllocator());
    cutLayer->toJson(doc1);
    doc.AddMember("cut", rapidjson::Value(doc1, doc.GetAllocator()), doc.GetAllocator());

    rapidjson::Document doc2(&doc.GetAllocator());
    fillLayer->toJson(doc2);
    doc.AddMember("fill", rapidjson::Value(doc2, doc.GetAllocator()), doc.GetAllocator());

    return getStringFromJson(doc);
}

void VolumeCutfillLayer::fromJSON(const string &json) {
    if (json.empty()) return;

    rapidjson::Document doc;
    doc.Parse(json.c_str());
    if (!doc.IsObject()) return;

    this->currentQId = doc["currentQId"].GetInt();
    this->workTypeQ1 = (WorkType)doc["workTypeQ1"].GetInt();
    this->workTypeQ2 = (WorkType)doc["workTypeQ2"].GetInt();

    loadQuestion(currentQId);

    cutLayer->fromJson(doc["cut"]);
    fillLayer->fromJson(doc["fill"]);
}

void VolumeCutfillLayer::setWorkType(WorkType workType, bool isLoadQuestion) {
    if (currentQId == 1) {
        if (!isLoadQuestion) if (this->workTypeQ1 == workType) return;
        this->workTypeQ1 = workType;
    } else {
        if (!isLoadQuestion) if (this->workTypeQ2 == workType) return;
        this->workTypeQ2 = workType;
    }

    if (currentLayer != nullptr) {
        currentLayer->setVisible(false);
    }
    if (workType == CUT) {
        currentLayer = cutLayer;

        cutBtn->setTitleColor(Color3B::BLUE);
        fillBtn->setTitleColor(Color3B::BLACK);
    } else if (workType == FILL) {
        currentLayer = fillLayer;

        cutBtn->setTitleColor(Color3B::BLACK);
        fillBtn->setTitleColor(Color3B::BLUE);
    } else {
        cutBtn->setTitleColor(Color3B::BLACK);
        fillBtn->setTitleColor(Color3B::BLACK);
    }
//    currentLayer->setVisible(true);
    currentLayer->setTouchEnabled(workType != NONE);
}

void VolumeCutfillLayer::reset() {
    if (currentLayer != nullptr) {
        currentLayer->reset();
    }
}

void VolumeCutfillLayer::saveRecord() {
    string saveFile = savePath + StringUtils::format("q%d.txt", currentQId);
    if (FileUtils::getInstance()->isFileExist(saveFile)) {
        FileUtils::getInstance()->removeFile(saveFile);
    }

    FileUtils::getInstance()->writeStringToFile(toJSON(), saveFile);
}

void VolumeCutfillLayer::loadRecord() {
    string saveFile = savePath + StringUtils::format("q%d.txt", currentQId);
    if (FileUtils::getInstance()->isFileExist(saveFile)) {
        fromJSON(FileUtils::getInstance()->getStringFromFile(saveFile));
    }
}

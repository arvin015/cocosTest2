//
//  MainLayer.cpp
//  CocosTest2
//
//  Created by arvin on 2017/8/21.
//
//

#include "MainLayer.h"
#include "ui/UIButton.h"
#include "ui/UICheckBox.h"
#include "ui/UIScrollView.h"
#include <spine/spine-cocos2dx.h>
#include "spine/spine.h"
#include "HelloWorldScene.h"
#include "NavigationController.h"
#include "DrawViewTest.h"
#include "TableViewTest.h"
#include "ActionTest.h"
#include "ClippingTest.h"
#include "NumberInTenCompareLayer.h"
#include "NumberInTenSingularLayer.h"
#include "NumberInTenComBineLayer.h"
#include "NumberInTenFlyLayer.h"
#include "NumberInTwentySequence.h"
#include "SimpleAudioEngine.h"
#include "LoadDialog.h"
#include "ElectronicBoardsLayer.h"
#include "CommonTest.h"
#include "AddAndSubtractLayer.h"
#include "CuttingLayer.h"
#include "FoldPaperLayer.h"
#include "Test3DLayer.h"
#include "FractionBarLayer.h"
#include "VolumesOfCubesLayer.h"
#include "VolumeCutfillLayer.h"
#include "VectorTestLayer.h"

USING_NS_CC;
using namespace ui;
using namespace std;
using namespace CocosDenshion;
using namespace spine;

template <class T>
int getArrLen(T &array) {
    return (sizeof(array) / sizeof(array[0]));
}

#define V_WIDTH  Director::getInstance()->getVisibleSize().width
#define V_HEIGHT Director::getInstance()->getVisibleSize().height

const string names[] = {"画板", "GridView", "动作", "节点裁剪", "10_Compare", "10_Singular",
                        "10_Combine", "10_Fly", "20_Sequence", "电子钉板", "UICommon", "AddSubtract",
                        "Cutting", "FoldPaper", "Test3D", "分数棒", "数粒", "体积分割补偿", "向量测试"};

inline int getMRow(int index, int col) {
    return index / col;
}

inline int getMCol(int index, int col) {
    return index % col;
}

MainLayer::MainLayer() {

}

MainLayer::~MainLayer() {

}

bool MainLayer::init() {
    
    if(!Layer::init()) {
        return false;
    }

    LayerColor* bg = LayerColor::create(Color4B::WHITE);
    addChild(bg);
    
    float paddingLeft = 20;
    float paddingTop = 20;
    float width = 150;
    float height = 54;
    int column = 6;
    float startX = (V_WIDTH - column * width - (column - 1) * paddingLeft) / 2;
    float startY = V_HEIGHT - 50;
    
    for(int i = 0; i < getArrLen(names); i++) {
        Button* btn = Button::create("mian_button_01_125x54.png");
        btn->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
        btn->setContentSize(Size(150, 54));
        btn->setScale9Enabled(true);
        btn->setPosition(Vec2(startX + (getMCol(i, column) * (width + paddingLeft)),
                              startY - (getMRow(i, column) * (height + paddingTop))));
        btn->setTitleFontSize(24);
        btn->setTitleText(names[i]);
        btn->addClickEventListener(CC_CALLBACK_1(MainLayer::onBtnClick, this));
        btn->setTag(i);
        addChild(btn);
    }
    
    CheckBox* audioBtn = CheckBox::create("CloseNormal.png", "CloseSelected.png");
    audioBtn->setPosition(Vec2(50, 50));
    audioBtn->setSelected(true);
    audioBtn->addEventListener([this](Ref* pSender, CheckBox::EventType eventType) {
        if(eventType == CheckBox::EventType::SELECTED) {
            SimpleAudioEngine::getInstance()->playBackgroundMusic("background.mp3", true); //播放音乐
        } else {
            SimpleAudioEngine::getInstance()->stopBackgroundMusic(); //停止音乐
        }
    });
    addChild(audioBtn);
    
    SimpleAudioEngine::getInstance()->preloadBackgroundMusic("background.mp3"); //预加载音乐
//    scheduleOnce([](float dt){
//        SimpleAudioEngine::getInstance()->playBackgroundMusic("background.mp3", true); //播放音乐
//    }, 2.0f, "Audio");
    
    SimpleAudioEngine::getInstance()->preloadEffect("pew-pew-lei.wav"); //预加载音效

    LoadDialog* loadDialog = LoadDialog::create();
    loadDialog->setText("正在登录系统，请稍后...");
    loadDialog->setCancelable(true);
    loadDialog->show(this);
    
//    spine::SkeletonAnimation* skeleton = spine::SkeletonAnimation::createWithJsonFile("tank.json", "tank.atlas");
//    skeleton->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
//    skeleton->setPosition(Vec2(V_WIDTH - 100, V_HEIGHT / 2 - 200));
//    skeleton->setScale(0.3);
//    this->addChild(skeleton);
//    
//    skeleton->setAnimation(0, "drive", true);

    return true;
}

HelloWorld* MainLayer::getMainScene() {
    return dynamic_cast<HelloWorld*>(Director::getInstance()->getRunningScene());
}

void MainLayer::onBtnClick(Ref* pSender) {
    
    SimpleAudioEngine::getInstance()->playEffect("pew-pew-lei.wav"); //播放音效
    
    Button* btn = static_cast<Button*>(pSender);
    if(btn) {
        int tag = btn->getTag();
        switch (tag) {
            case 0: {
                DrawViewTest* drawViewTest = DrawViewTest::create();
                getMainScene()->getRootLayer()->controller->pushView(drawViewTest);
                break;
            }
            case 1: {
                TableViewTest* tableViewTest = TableViewTest::create();
                getMainScene()->getRootLayer()->controller->pushView(tableViewTest);
                break;
            }
            case 2: {
                ActionTest* actionTest = ActionTest::create();
                getMainScene()->getRootLayer()->controller->pushView(actionTest);
                break;
            }
            case 3: {
                ClippingTest* clipplingTest = ClippingTest::create();
                getMainScene()->getRootLayer()->controller->pushView(clipplingTest);
                break;
            }
            case 4: {
                BaseNumberInTenDragLayer* baseDragLayer = NumberInTenCompareLayer::create();
                getMainScene()->getRootLayer()->controller->pushView(baseDragLayer);
                break;
            }
            case 5: {
                BaseNumberInTenDragLayer* baseDragLayer = NumberInTenSingularLayer::create();
                getMainScene()->getRootLayer()->controller->pushView(baseDragLayer);
                break;
            }
            case 6: {
                BaseNumberInTenDragLayer* baseDragLayer = NumberInTenCombineLayer::create();
                getMainScene()->getRootLayer()->controller->pushView(baseDragLayer);
                break;
            }
            case 7: {
                NumberInTenFlyLayer* flyLayer = NumberInTenFlyLayer::create();
                getMainScene()->getRootLayer()->controller->pushView(flyLayer);
                break;
            }
            case 8: {
                NumberInTwentySequenceLayer* sequenceLayer = NumberInTwentySequenceLayer::create();
                getMainScene()->getRootLayer()->controller->pushView(sequenceLayer);
                break;
            }
            case 9: {
                ElectronicBoardsLayer* boardsLayer = ElectronicBoardsLayer::create();
                getMainScene()->getRootLayer()->controller->pushView(boardsLayer);
                break;
            }
            case 10: {
                CommonTest* commonTest = CommonTest::create();
                getMainScene()->getRootLayer()->controller->pushView(commonTest);
                break;
            }
            case 11: {
                AddAndSubtractLayer* addAndSubtractLayer = AddAndSubtractLayer::create();
                getMainScene()->getRootLayer()->controller->pushView(addAndSubtractLayer);
                break;
            }
            case 12: {
                CuttingLayer* cuttingLayer = CuttingLayer::create();
                getMainScene()->getRootLayer()->controller->pushView(cuttingLayer);
                break;
            }
            case 13: {
                auto foldPaperLayer = FoldPaper::FoldPaperLayer::create();
                getMainScene()->getRootLayer()->controller->pushView(foldPaperLayer);
                break;
            }
            case 14: {
                auto test3DLayer = Test3D::Test3DLayer::create();
                getMainScene()->getRootLayer()->controller->pushView(test3DLayer);
                break;
            }
            case 15: {
                auto fractionBarLayer = FractionBar::FractionBarLayer::create();
                getMainScene()->getRootLayer()->controller->pushView(fractionBarLayer);
                break;
            }
            case 16: {
                auto volumesOfCubesLayer = VolumesOfCubesLayer::create();
                getMainScene()->getRootLayer()->controller->pushView(volumesOfCubesLayer);
                break;
            }
            case 17: {
                auto volumeCutfillLayer = VolumeCutfillLayer::create();
                getMainScene()->getRootLayer()->controller->pushView(volumeCutfillLayer);
                break;
            }
            case 18: {
                auto* vecotrTestLayer = VectorTestLayer::create();
                getMainScene()->getRootLayer()->controller->pushView(vecotrTestLayer);
                break;
            }
            default:
                break;
        }
    }
}

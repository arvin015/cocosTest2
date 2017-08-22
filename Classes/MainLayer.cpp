//
//  MainLayer.cpp
//  CocosTest2
//
//  Created by arvin on 2017/8/21.
//
//

#include "MainLayer.h"
#include "ui/UIButton.h"
#include "HelloWorldScene.h"
#include "NavigationController.h"
#include "DrawViewTest.h"
#include "TableViewTest.h"
#include "ActionTest.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace ui;
using namespace std;
using namespace CocosDenshion;

#define V_WIDTH  Director::getInstance()->getVisibleSize().width
#define V_HEIGHT Director::getInstance()->getVisibleSize().height

const string names[] = {"画板", "GridView", "动作"};

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
    
    float paddingLeft = 20;
    float paddingTop = 20;
    float width = 125;
    float height = 54;
    int column = 6;
    float startX = (V_WIDTH - column * width - (column - 1) * paddingLeft) / 2;
    float startY = V_HEIGHT - 50;
    
    for(int i = 0; i < 3; i++) {
        Button* btn = Button::create("mian_button_01_125x54.png");
        btn->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
        btn->setContentSize(Size(125, 54));
        btn->setPosition(Vec2(startX + (getMCol(i, column) * (width + paddingLeft)),
                              startY - (getMRow(i, column) * (height + paddingTop))));
        btn->setTitleFontSize(24);
        btn->setTitleText(names[i]);
        btn->addClickEventListener(CC_CALLBACK_1(MainLayer::onBtnClick, this));
        btn->setTag(i);
        addChild(btn);
    }
    
    //播放背景音乐
    SimpleAudioEngine::getInstance()->preloadBackgroundMusic("background.mp3"); //预加载音乐
    scheduleOnce([](float dt){
        SimpleAudioEngine::getInstance()->playBackgroundMusic("background.mp3", true); //播放音乐
    }, 2.0f, "Audio");
//    SimpleAudioEngine::getInstance()->pauseBackgroundMusic(); //暂停播放音乐
//    SimpleAudioEngine::getInstance()->resumeBackgroundMusic(); //重新播放音乐
//    SimpleAudioEngine::getInstance()->stopBackgroundMusic(); //停止播放音乐
    
    //播放音效
    SimpleAudioEngine::getInstance()->preloadEffect("pew-pew-lei.wav"); //预加载音效
    
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
                
            default:
                break;
        }
    }
}

//
//  BaseLayer.cpp
//  CocosTest2
//
//  Created by arvin on 2017/8/21.
//
//

#include "BaseLayer.h"
#include "HelloWorldScene.h"
#include "NavigationController.h"

USING_NS_CC;
using namespace ui;
using namespace std;

BaseLayer::BaseLayer()
: helloworld(nullptr)
, backBtn(nullptr) {
}

BaseLayer::~BaseLayer() {

}

bool BaseLayer::init() {
    
    if(!Layer::init()) {
        return false;
    }
    
    getMainScene();
    
    backBtn = Button::create();
    backBtn->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    backBtn->setPosition(Vec2(20, V_HEIGHT - 20));
    backBtn->setTitleText("返回");
    backBtn->setTitleColor(Color3B::BLACK);
    backBtn->setTitleFontSize(22);
    backBtn->addClickEventListener([this](Ref* pSender){
        goBack();
        this->onBackHandle();
    });
    addChild(backBtn);
    
    return true;
}

void BaseLayer::getMainScene() {
    helloworld = dynamic_cast<HelloWorld*>(Director::getInstance()->getRunningScene());
}
void BaseLayer::goBack() {
    helloworld->getRootLayer()->controller->popView();
}

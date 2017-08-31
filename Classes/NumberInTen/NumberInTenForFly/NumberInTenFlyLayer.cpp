//
//  NumberInTenFlyLayer.cpp
//  CocosTest2
//
//  Created by arvin on 2017/8/31.
//
//

#include "NumberInTenFlyLayer.h"
#include "FlyView.h"

USING_NS_CC;
using namespace ui;
using namespace std;

NumberInTenFlyLayer::NumberInTenFlyLayer()
: flyView(nullptr) {

}

NumberInTenFlyLayer::~NumberInTenFlyLayer() {

}

void NumberInTenFlyLayer::onEnter() {
    BaseLayer::onEnter();
}

void NumberInTenFlyLayer::onExit() {

    BaseLayer::onExit();
}

bool NumberInTenFlyLayer::init() {
    if(!BaseLayer::init()) {
        return false;
    }
    
    flyView = FlyView::create();
    flyView->setPosition(Vec2(100, V_HEIGHT / 2 + 20));
    this->addChild(flyView);
    
    Button* danceBtn = Button::create();
    danceBtn->setTitleFontSize(24);
    danceBtn->setTitleColor(Color3B::BLACK);
    danceBtn->setTitleText("舞動");
    danceBtn->setPosition(Vec2(V_WIDTH - 100, 300));
    danceBtn->addClickEventListener([this](Ref* pSender){
        flyView->dance();
    });
    this->addChild(danceBtn);
    
    return true;
}


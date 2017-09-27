//
// Created by arvin on 2017/9/25.
//

#include "LoadDialog.h"
#include "ui/UILayout.h"

USING_NS_CC;
using namespace ui;
using namespace std;

LoadDialog::LoadDialog()
: eventListener(nullptr)
, cancelable(false) {

}

LoadDialog::~LoadDialog() {

}

void LoadDialog::onEnter() {
    Layer::onEnter();

    eventListener = EventListenerTouchOneByOne::create();
    eventListener->setSwallowTouches(true);
    eventListener->onTouchBegan = [this](Touch* touch, Event* event){
        if(cancelable) {
            dismiss();
        }
        return true;
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, this);

}

void LoadDialog::onExit() {

    if(eventListener) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(eventListener);
    }
    
    if(loadingImage) {
        loadingImage->stopAllActions();
    }
    
    stopTimer(); //取消计时

    Layer::onExit();
}

bool LoadDialog::init() {

    if(!Layer::init()) {
        return false;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    
    LayerColor* grayBg = LayerColor::create(Color4B(0, 0, 0, 160));
    this->addChild(grayBg);
    
    Layout* bg = Layout::create();
    bg->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    bg->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    bg->setContentSize(Size(visibleSize.width - 200, 70));
    bg->setBackGroundImage("white.png");
    bg->setBackGroundImageScale9Enabled(true);
    this->addChild(bg);

    loadingImage = ImageView::create("loading.png");
    loadingImage->setPosition(Vec2(50, bg->getContentSize().height / 2));
    loadingImage->setScale(0.4f);
    bg->addChild(loadingImage);

    loadingImage->runAction(RepeatForever::create(RotateBy::create(1.0f, 360)));

    loadText = Text::create();
    loadText->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    loadText->setPosition(Vec2(loadingImage->getPositionX() + 50, bg->getContentSize().height / 2));
    loadText->setTextColor(Color4B::BLACK);
    loadText->setFontSize(24);
    loadText->setTextVerticalAlignment(TextVAlignment::CENTER);
    loadText->setTextAreaSize(Size(bg->getContentSize().width - 100, 0));
    bg->addChild(loadText);

    return true;
}

void LoadDialog::setCancelable(bool cancelable) {
    this->cancelable = cancelable;
}

void LoadDialog::show(Node* parent) {
    parent->addChild(this);
    startTimer(); //计时
}

void LoadDialog::dismiss() {
    removeFromParentAndCleanup(true);
}

void LoadDialog::setText(const string &text) {
    loadText->setString(text);
}

void LoadDialog::setTextColor(const Color4B &textColor) {
    loadText->setTextColor(textColor);
}

void LoadDialog::setTextSize(int textSize) {
    loadText->setFontSize(textSize);
}

void LoadDialog::startTimer() {
    scheduleOnce([this](float d) {
        dismiss();
    }, 30.0f, "timerSchedule");
}

void LoadDialog::stopTimer() {
    unschedule("timerSchedule");
}



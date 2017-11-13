//
//  SnapView.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/11/13.
//

#include "SnapView.h"

USING_NS_CC;
using namespace std;

SnapView::SnapView()
: preSnapView(nullptr)
, nextSnapView(nullptr)
, isInArea(false) {

}

SnapView::~SnapView() {

}

void SnapView::onEnter() {
    Sprite::onEnter();

    eventListener = EventListenerTouchOneByOne::create();
    eventListener->onTouchBegan = CC_CALLBACK_2(SnapView::onTouchBegan, this);
    eventListener->onTouchMoved = CC_CALLBACK_2(SnapView::onTouchMoved, this);
    eventListener->onTouchEnded = CC_CALLBACK_2(SnapView::onTouchEnded, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, this);
}

void SnapView::onExit() {

    if (eventListener) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(eventListener);
    }

    Sprite::onExit();
}

SnapView* SnapView::create(const string &imageName) {
    SnapView* snapView = new (std::nothrow)SnapView;
    if (snapView && snapView->init(imageName)) {
        snapView->autorelease();
        return snapView;
    }

    CC_SAFE_DELETE(snapView);
    return nullptr;
}

bool SnapView::init(const string &imageName) {
    this->imageName = imageName;
    return init();
}

bool SnapView::init() {
    if (!Sprite::init()) {
        return false;
    }

    if (!imageName.empty()) {
        setTexture(imageName);
    }

    return true;
}

bool SnapView::onTouchBegan(Touch* touch, Event* event) {
    if (getBoundingBox().containsPoint(touch->getLocation())) {

        eventListener->setSwallowTouches(true);

        isInArea = true;
        if (touchDown) {
            this->touchDown();
        }
    }
    return true;
}

void SnapView::onTouchMoved(Touch* touch, Event* event) {
    if (isInArea) {
        this->setPosition(touch->getLocation());
        if (touchMove) {
            this->touchMove();
        }
    }
}

void SnapView::onTouchEnded(Touch* touch, Event* event) {
    if (isInArea) {
        isInArea = false;
        eventListener->setSwallowTouches(false);
        if (touchEnd) {
            this->touchEnd(getPositionX(), getPositionY());
        }
    }
}

//
//  CatSprite.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/12/5.
//

#include "CatSprite.h"

USING_NS_CC;
using namespace std;

CatSprite::CatSprite()
: eventListener(nullptr)
, isTouchEnabled(true)
, isSelect(false) {

}

CatSprite::~CatSprite() {

}

void CatSprite::onEnter() {
    Sprite::onEnter();

    eventListener = EventListenerTouchOneByOne::create();
    eventListener->onTouchBegan = CC_CALLBACK_2(CatSprite::onTouchBegan, this);
    eventListener->onTouchMoved = CC_CALLBACK_2(CatSprite::onTouchMoved, this);
    eventListener->onTouchEnded = CC_CALLBACK_2(CatSprite::onTouchEnded, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, this);
}

void CatSprite::onExit() {

    if (eventListener) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(eventListener);
    }

    Sprite::onExit();
}

CatSprite* CatSprite::create(const string &imageName) {
    CatSprite* catSprite = new (std::nothrow) CatSprite;
    if (catSprite && catSprite->init(imageName)) {
        catSprite->autorelease();
        return catSprite;
    }
    CC_SAFE_DELETE(catSprite);
    return nullptr;
}

bool CatSprite::init(const string &imageName) {
    if (!init()) {
        return false;
    }
    
    setTexture(imageName);
    
    return true;
}

bool CatSprite::init() {
    if (!Sprite::init()) {
        return false;
    }
    return true;
}

bool CatSprite::onTouchBegan(Touch* touch, Event* event) {
    if (!isTouchEnabled) {
        return false;
    }

    if (getBoundingBox().containsPoint(getParent()->convertToNodeSpace(touch->getLocation()))) {
        isSelect = true;
        eventListener->setSwallowTouches(true);

        if (touchDownCallback) {
            this->touchDownCallback(getParent()->convertToNodeSpace(touch->getLocation()).x);
        }
    }

    return true;
}

void CatSprite::onTouchMoved(Touch* touch, Event* event) {
    if (isSelect) {
        float x = getPositionX() + touch->getDelta().x;
        if (x < 0) x = 1;
        if (x > getParent()->getContentSize().width) x = getParent()->getContentSize().width;
        setPositionX(x);

        if (touchMoveCallback) {
            this->touchMoveCallback(x);
        }
    }
}

void CatSprite::onTouchEnded(Touch* touch, Event* event) {
    if (isSelect) {
        if (touchUpCallback) {
            this->touchUpCallback(getPositionX());
        }
    }
    isSelect = false;
    eventListener->setSwallowTouches(false);
}

void CatSprite::playAnim(float toX) {
    this->stopAllActions();
    auto anim = MoveTo::create(0.5f, Vec2(toX, getPositionY()));
    this->runAction(anim);
}

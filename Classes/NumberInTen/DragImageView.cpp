//
// Created by Administrator on 2017/8/27.
//

#include "DragImageView.h"

USING_NS_CC;
using namespace ui;
using namespace std;

DragImageView::DragImageView()
: eventListener(nullptr)
, isMoveEnabled(true)
, isMoving(false) {

}

DragImageView::~DragImageView() {

}

void DragImageView::onEnter() {
    ImageView::onEnter();

    eventListener = EventListenerTouchOneByOne::create();
    eventListener->onTouchBegan = CC_CALLBACK_2(DragImageView::onTouchBegan, this);
    eventListener->onTouchMoved = CC_CALLBACK_2(DragImageView::onTouchMoved, this);
    eventListener->onTouchEnded = CC_CALLBACK_2(DragImageView::onTouchEnded, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, this);
}
void DragImageView::onExit() {

    if(eventListener) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(eventListener);
    }

    ImageView::onExit();
}

bool DragImageView::init() {
    if(!ImageView::init()) {
        return false;
    }

    return true;
}

bool DragImageView::init(const std::string &imageFileName, TextureResType texType)
{
    bool bRet = true;
    do
    {
        if (!init())
        {
            bRet = false;
            break;
        }

        this->loadTexture(imageFileName, texType);
    } while (0);
    return bRet;
}

DragImageView* DragImageView::create(const string &filename) {
    DragImageView *widget = new (std::nothrow) DragImageView;
    if (widget && widget->init(filename))
    {
        widget->autorelease();
        return widget;
    }
    CC_SAFE_DELETE(widget);
    return nullptr;
}

bool DragImageView::onTouchBegan(Touch* touch, Event* event) {

    if(!isMoveEnabled) {
        return false;
    }

    if(touch->getID() != 0) {
        return false;
    }

    if(getBoundingBox().containsPoint(touch->getLocation())) {
        isMoving = true;
        eventListener->setSwallowTouches(true);
    }
}

void DragImageView::onTouchMoved(Touch* touch, Event* event) {

    if(touch->getID() != 0) {
        return;
    }

    if(isMoving) {
        this->setPosition(getPosition() + touch->getDelta());
    }
}

void DragImageView::onTouchEnded(Touch* touch, Event* event) {
    if(isMoving) {

        this->moveEndedCallback(this); //回调

        eventListener->setSwallowTouches(false);
        isMoving = false;
    }
}

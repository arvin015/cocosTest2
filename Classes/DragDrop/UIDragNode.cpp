//
// Created by mac_arvin on 2019-08-02.
//

#include "UIDragNode.h"

USING_NS_CC;
using namespace ui;

DragNode::DragNode()
: touchListener(nullptr)
, enableDrag(true)
, longClickCounter(false)
, isLongClick(false)
, timeCounter(0)
, renderTexture(nullptr) {

}

DragNode::~DragNode() {

}

void DragNode::update(float dt) {

    if (longClickCounter) {
        timeCounter += dt;
        if (timeCounter >= 1.0f) {
            isLongClick = true;
            timeCounter = 0;
            longClickCounter = false;
            onLongClickHandle();
        }
    }
}

void DragNode::onEnter() {
    base::onEnter();

    touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(DragNode::onTouchDown, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(DragNode::onTouchMove, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(DragNode::onTouchUp, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);

    scheduleUpdate();

    //场景中加入虚像Node
    if (renderTexture == nullptr) {
        renderTexture = RenderTexture::create(getContentSize().width, getContentSize().height);
        renderTexture->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        renderTexture->setVisible(false);
        Director::getInstance()->getRunningScene()->addChild(renderTexture, 2);
    }
}

void DragNode::onExit() {

    if (touchListener) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(touchListener);
    }

    unscheduleUpdate();

    base::onExit();
}

bool DragNode::init() {
    if (!base::init()) {
        return false;
    }

    return true;
}

bool DragNode::onTouchDown(Touch* touch, Event* event) {
    if (!enableDrag) {
        return false;
    }

    if (touch->getID() == 0) {
        if (getBoundingBox().containsPoint(getParent()->convertToNodeSpace(touch->getLocation()))) {
            beginWorldVec = touch->getLocation();
            longClickCounter = true;
            timeCounter = 0;
        } else {
            return false;
        }
    }
    return true;
}

void DragNode::onTouchMove(Touch* touch, Event* event) {
    if (fabsf(touch->getDelta().x) > 2.0f || fabsf(touch->getDelta().y) > 2.0f) {
        longClickCounter = false;
        timeCounter = 0;
    }
    
    if (isLongClick) {
        setVirtualImagePosition(renderTexture->getPosition() + touch->getDelta());
        touchListener->setSwallowTouches(true);
        
        if (onDragMove) {
            this->onDragMove(touch->getLocation());
        }
    }
}

void DragNode::onTouchUp(Touch* touch, Event* event) {
    longClickCounter = false;
    timeCounter = 0;
    
    if (isLongClick) {
        isLongClick = false;
        renderTexture->setVisible(false);
        touchListener->setSwallowTouches(false);
        
        if (onDragEnd) {
            this->onDragEnd(touch->getLocation());
        }
    }
}

void DragNode::onLongClickHandle() {
    renderTexture->setVisible(true);
    setVirtualImagePosition(beginWorldVec);
    
    Vec2 oldPos = getPosition(); //记录实体原位置
    
    renderTexture->begin();
    //注意，RenderTexture截图是从锚点开始截
    Vec2 anchorPoint = getAnchorPoint();
    if (anchorPoint == Vec2::ANCHOR_TOP_LEFT) {
        setPosition(Vec2(0, getContentSize().height));
    } else if (anchorPoint == Vec2::ANCHOR_MIDDLE_LEFT) {
        setPosition(Vec2(0, getContentSize().height / 2));
    } else if (anchorPoint == Vec2::ZERO) {
        setPosition(Vec2::ZERO);
    } else if (anchorPoint == Vec2::ANCHOR_MIDDLE_TOP) {
        setPosition(Vec2(getContentSize().width / 2, getContentSize().height));
    } else if (anchorPoint == Vec2::ANCHOR_MIDDLE) {
        setPosition(Vec2(getContentSize() / 2));
    } else if (anchorPoint == Vec2::ANCHOR_MIDDLE_BOTTOM) {
        setPosition(Vec2(getContentSize().width / 2, 0));
    } else if (anchorPoint == Vec2::ANCHOR_TOP_RIGHT) {
        setPosition(Vec2(getContentSize()));
    } else if (anchorPoint == Vec2::ANCHOR_MIDDLE_RIGHT) {
        setPosition(Vec2(getContentSize().width, getContentSize().height / 2));
    } else if (anchorPoint == Vec2::ANCHOR_BOTTOM_RIGHT) {
        setPosition(Vec2(getContentSize().width, 0));
    }
    visit();
    renderTexture->end();

    setPosition(oldPos + Vec2(0.01f, 0.01f)); //恢复实体原位置，不加0.01会有问题

    Director::getInstance()->getRenderer()->render();
    
    if (onDragStart) {
        this->onDragStart(beginWorldVec);
    }
}

void DragNode::setVirtualImagePosition(const Vec2 &position) {
    renderTexture->setPosition(position);
}

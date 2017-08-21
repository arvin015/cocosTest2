//
// Created by arvin on 2017/8/16.
//

#include "UIDrawView.h"

USING_NS_CC;

UIDrawView::UIDrawView()
: eventListener(nullptr)
, lastPos(-1, -1)
, rt(nullptr)
, drawType(DrawType::DRAW) {

}

#define S_WIDTH  Director::getInstance()->getVisibleSize().width
#define S_HEIGHT Director::getInstance()->getVisibleSize().height

UIDrawView::~UIDrawView() {

}

void UIDrawView::onEnter() {
    Layer::onEnter();
    
    eventListener = EventListenerTouchOneByOne::create();
    eventListener->onTouchBegan = CC_CALLBACK_2(UIDrawView::onTouchBegan, this);
    eventListener->onTouchMoved = CC_CALLBACK_2(UIDrawView::onTouchMoved, this);
    eventListener->onTouchEnded = CC_CALLBACK_2(UIDrawView::onTouchEnded, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, this);
}

void UIDrawView::onExit() {

    if(eventListener) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(eventListener);
    }
    
    Layer::onExit();
}

bool UIDrawView::init() {
    if(!Layer::init()) {
        return false;
    }

    rt = RenderTexture::create(S_WIDTH, S_HEIGHT);
    rt->setPosition(Vec2(S_WIDTH / 2, S_HEIGHT / 2));
    rt->retain();
    rt->clear(0.0f, 0.0f, 0.0f, 0.0f);
    addChild(rt);
    
    return true;
}

bool UIDrawView::onTouchBegan(Touch* touch, Event* event) {
    if(lastPos == Vec2(-1, -1)) {
        lastPos = touch->getLocation();
    }
    return true;
}

void UIDrawView::onTouchMoved(Touch* touch, Event* event) {
    Vec2 currentPos = touch->getLocation();

    onDraw(lastPos, currentPos);

    lastPos = currentPos;
}

void UIDrawView::onTouchEnded(Touch* touch, Event* event) {
    lastPos = Vec2(-1, -1);
}

void UIDrawView::setDrawType(DrawType drawType) {
    this->drawType = drawType;
}

void UIDrawView::onDraw(Vec2 from, Vec2 to) {

    DrawNode* drawNode = DrawNode::create();
    Color4F color;
    if(drawType == DRAW) {
        drawNode->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);
        color = Color4F::RED;
    } else if(drawType == CLEAR) {
        drawNode->setBlendFunc(BlendFunc::DISABLE);
        color = Color4F(255.0f, 255.0f, 255.0f, 0.0f);
    }
    addChild(drawNode);

    drawNode->drawSegment(from, to, 2, color);

    if(drawType == DRAW) {
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glBlendFunc(GL_ONE, GL_ZERO);
    }

    rt->begin();
    drawNode->visit();
    rt->end();

    Director::getInstance()->getRenderer()->render();

    drawNode->clear();
    drawNode->removeFromParent();
}

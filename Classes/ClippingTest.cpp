//
//  ClippingTest.cpp
//  CocosTest2
//
//  Created by arvin on 2017/8/23.
//
//

#include "ClippingTest.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace ui;
using namespace std;
using namespace CocosDenshion;

ClippingTest::ClippingTest()
: BaseLayer()
, eventListener(nullptr)
, isInArea(false)
, isMove(false)
, contentSprite(nullptr){
    
}

ClippingTest::~ClippingTest() {
    
}

void ClippingTest::onEnter() {
    BaseLayer::onEnter();
    
    eventListener = EventListenerTouchOneByOne::create();
    eventListener->onTouchBegan = CC_CALLBACK_2(ClippingTest::onTouchBegan, this);
    eventListener->onTouchMoved = CC_CALLBACK_2(ClippingTest::onTouchMoved, this);
    eventListener->onTouchEnded = CC_CALLBACK_2(ClippingTest::onTouchEnded, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, this);
}

void ClippingTest::onExit() {
    BaseLayer::onExit();
    
    if(eventListener) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(eventListener);
    }
}

bool ClippingTest::onTouchBegan(Touch* touch, Event* event) {
    if(clippingNode->getBoundingBox().containsPoint(touch->getLocation())) {
        isInArea = true;
    }
    return true;
}

void ClippingTest::onTouchMoved(Touch* touch, Event* event) {
    if(abs(touch->getPreviousLocation().x - touch->getLocation().x) > 2.0f ||
       abs(touch->getPreviousLocation().y - touch->getLocation().y) > 2.0f) {
        isMove = true;
    }
}

void ClippingTest::onTouchEnded(Touch* touch, Event* event) {
    if(isInArea && !isMove) {
        Point point = clippingNode->convertToNodeSpace(touch->getLocation());
        shoot(point);
        playShootAnimation();
        
        SimpleAudioEngine::getInstance()->playEffect("effect2.mp3");
    }
    
    isMove = false;
    isInArea = false;
}

bool ClippingTest::init() {
    if(!BaseLayer::init()) {
        return false;
    }
    
    LayerColor* bg = LayerColor::create(Color4B(0, 0, 0, 255));
    this->addChild(bg);
    
    clippingNode = ClippingNode::create();
    clippingNode->setPosition(Vec2(V_WIDTH / 2, V_HEIGHT / 2));
    clippingNode->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    clippingNode->setContentSize(Size(480, 320));
    clippingNode->setInverted(true);
    clippingNode->setAlphaThreshold(0);
    this->addChild(clippingNode);
    
    contentSprite = Sprite::create("Hello.png");
    contentSprite->setPosition(Vec2(contentSprite->getContentSize().width / 2, contentSprite->getContentSize().height / 2));
    clippingNode->addChild(contentSprite);
    
    holoContents = Node::create();
    holoContents->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    holoContents->setContentSize(clippingNode->getContentSize());
    holoContents->setPosition(Vec2(contentSprite->getContentSize().width / 2, contentSprite->getContentSize().height / 2));
    contentSprite->addChild(holoContents);
    
    holoStencils = Node::create();
    holoStencils->setContentSize(clippingNode->getContentSize());
    clippingNode->setStencil(holoStencils);
    
    //预加载射击音效
    SimpleAudioEngine::getInstance()->preloadEffect("effect2.mp3");
    
    return true;
}

void ClippingTest::shoot(Vec2 position) {
    Sprite* holeEffect = Sprite::create("hole_effect.png");
    holeEffect->setScale(2);
    holeEffect->setPosition(position);
    holoContents->addChild(holeEffect);
    
    Sprite* holeStencil = Sprite::create("hole_stencil.png");
    holeStencil->setScale(2);
    holeStencil->setPosition(position);
    holoStencils->addChild(holeStencil);
}

void ClippingTest::playShootAnimation() {
    auto scaleAction = ScaleBy::create(0.1f, 0.8);
    clippingNode->runAction(Sequence::create(scaleAction, scaleAction->reverse(), CallFunc::create([this](){
        clippingNode->setScale(1.0f);
    }), NULL));
}

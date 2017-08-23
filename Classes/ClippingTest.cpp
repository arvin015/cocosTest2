//
//  ClippingTest.cpp
//  CocosTest2
//
//  Created by arvin on 2017/8/23.
//
//

#include "ClippingTest.h"

USING_NS_CC;
using namespace ui;
using namespace std;

ClippingTest::ClippingTest() {
    
}

ClippingTest::~ClippingTest() {
    
}

void ClippingTest::onEnter() {
    
}

void ClippingTest::onExit() {
    
}

bool ClippingTest::init() {
    if(!BaseLayer::init()) {
        return false;
    }
    
    clippingNode = ClippingNode::create();
    clippingNode->setPosition(Vec2(V_WIDTH / 2, V_HEIGHT / 2));
    clippingNode->setInverted(true);
    clippingNode->setAlphaThreshold(0);
    this->addChild(clippingNode);
    
    Sprite* content = Sprite::create("HelloWorld.png");
    clippingNode->addChild(content);
    
    holoContents = Node::create();
    this->addChild(holoContents);
    
    holoStencils = Node::create();
    clippingNode->setStencil(holoStencils);
    
    return true;
}

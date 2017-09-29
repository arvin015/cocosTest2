//
//  ClippingTest.h
//  CocosTest2
//
//  Created by arvin on 2017/8/23.
//
//

#ifndef ClippingTest_h
#define ClippingTest_h

#include <stdio.h>
#include "BaseLayer.h"

class ClippingTest : public BaseLayer {
    
public:
    
    ClippingTest();
    ~ClippingTest();
    
    virtual void onEnter();
    virtual void onExit();
    
    virtual bool init();
    
    CREATE_FUNC(ClippingTest);
    
private:
    
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
    void shoot(cocos2d::Vec2 position);
    void playShootAnimation();
    
private:
    cocos2d::EventListenerTouchOneByOne* eventListener;
    cocos2d::Node* holoContents;
    cocos2d::Node* holoStencils;
    cocos2d::ClippingNode* clippingNode;
    cocos2d::Sprite* contentSprite;
    bool isInArea;
    bool isMove;
};

#endif /* ClippingTest_h */

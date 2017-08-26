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
#include "Main/BaseLayer.h"

class ClippingTest : public BaseLayer {
    
public:
    
    ClippingTest();
    ~ClippingTest();
    
    virtual void onEnter();
    virtual void onExit();
    
    virtual bool init();
    
    CREATE_FUNC(ClippingTest);
    
private:
    cocos2d::Node* holoContents;
    cocos2d::Node* holoStencils;
    cocos2d::ClippingNode* clippingNode;
};

#endif /* ClippingTest_h */

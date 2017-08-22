//
//  ActionTest.h
//  CocosTest2
//
//  Created by arvin on 2017/8/22.
//
//

#ifndef ActionTest_h
#define ActionTest_h

#include <stdio.h>
#include "BaseLayer.h"

class ActionTest : public BaseLayer {

public:
    
    ActionTest();
    ~ActionTest();
    
    virtual void onEnter();
    virtual void onExit();
    
    virtual bool init();
    
    CREATE_FUNC(ActionTest);
    
private:
    void onBtnClick(cocos2d::Ref* pSender);
    
private:
    cocos2d::Sprite* testSprite;
};

#endif /* ActionTest_h */

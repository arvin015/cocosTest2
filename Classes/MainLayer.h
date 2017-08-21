//
//  MainLayer.h
//  CocosTest2
//
//  Created by arvin on 2017/8/21.
//
//

#ifndef MainLayer_h
#define MainLayer_h

#include <stdio.h>
#include "cocos2d.h"

class HelloWorld;

class MainLayer : public cocos2d::Layer {
    
public:
    
    MainLayer();
    ~MainLayer();
    
    virtual bool init();
    
    CREATE_FUNC(MainLayer);
    
private:
    HelloWorld* getMainScene();
    void onBtnClick(cocos2d::Ref* pSender);
};

#endif /* MainLayer_h */

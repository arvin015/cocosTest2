//
//  BaseLayer.h
//  CocosTest2
//
//  Created by arvin on 2017/8/21.
//
//

#ifndef BaseLayer_h
#define BaseLayer_h

#include <stdio.h>
#include "cocos2d.h"
#include "ui/UIButton.h"

class HelloWorld;

#define V_WIDTH  Director::getInstance()->getVisibleSize().width
#define V_HEIGHT Director::getInstance()->getVisibleSize().height

class BaseLayer : public cocos2d::Layer {
    
public:
    BaseLayer();
    ~BaseLayer();
    
    virtual bool init();
    
    void getMainScene();
    
    void goBack();

public:
    
    HelloWorld* helloworld;
    
private:
    cocos2d::ui::Button* backBtn;
};

#endif /* BaseLayer_h */

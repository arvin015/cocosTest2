//
//  NavigationController.h
//  CocosTest2
//
//  Created by arvin on 2017/8/21.
//
//

#ifndef NavigationController_h
#define NavigationController_h

#include <stdio.h>
#include "cocos2d.h"

class NavigationController : public cocos2d::Layer {

public:
    NavigationController();
    ~NavigationController();
    
    virtual bool init();
    
    CREATE_FUNC(NavigationController);
    
    void pushView(cocos2d::Layer* layer);
    
    void popView();
    
    cocos2d::Layer* getTopView();
    
private:
    cocos2d::Vector<cocos2d::Layer*> viewList;
};

#endif /* NavigationController_h */

//
//  ScrollViewTest.h
//  CocosTest2-mobile
//
//  Created by arvin on 2020/11/23.
//

#ifndef ScrollViewTest_h
#define ScrollViewTest_h

#include "BaseLayer.h"
#include "ui/UIScrollView.h"

class ScrollViewTest : public BaseLayer {
    
    typedef BaseLayer base;
    typedef ScrollViewTest thisclass;
    
public:
    
    ScrollViewTest();
    virtual ~ScrollViewTest();
    
    virtual void onEnter();
    virtual void onExit();
    
    virtual bool init();
    CREATE_FUNC(ScrollViewTest);
    
private:
    cocos2d::LayerColor* bgLayer;
    cocos2d::ui::ScrollView* scrollView;
};

#endif /* ScrollViewTest_h */

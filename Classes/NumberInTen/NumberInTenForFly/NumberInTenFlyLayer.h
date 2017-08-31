//
//  NumberInTenFlyLayer.h
//  CocosTest2
//
//  Created by arvin on 2017/8/31.
//
//

#ifndef NumberInTenFlyLayer_h
#define NumberInTenFlyLayer_h

#include <stdio.h>
#include "BaseLayer.h"

class FlyView;

class NumberInTenFlyLayer : public BaseLayer {
  
public:

    NumberInTenFlyLayer();
    ~NumberInTenFlyLayer();
    
    virtual void onEnter();
    virtual void onExit();
    
    virtual bool init();
    
    CREATE_FUNC(NumberInTenFlyLayer);
    
private:
    FlyView* flyView;
};

#endif /* NumberInTenFlyLayer_h */

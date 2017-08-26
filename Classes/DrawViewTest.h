//
//  DrawViewTest.h
//  CocosTest2
//
//  Created by arvin on 2017/8/21.
//
//

#ifndef DrawViewTest_h
#define DrawViewTest_h

#include <stdio.h>
#include "Main/BaseLayer.h"

class DrawViewTest : public BaseLayer {

public:
    DrawViewTest();
    ~DrawViewTest();
    
    virtual bool init();
    
    CREATE_FUNC(DrawViewTest);
};

#endif /* DrawViewTest_h */

//
//  CommonTest.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/11/22.
//

#ifndef CommonTest_h
#define CommonTest_h

#include <stdio.h>
#include "BaseLayer.h"

class CommonTest : public BaseLayer {
    
public:
    
    CommonTest();
    virtual ~CommonTest();

    virtual bool init();

    CREATE_FUNC(CommonTest);
    
private:
    
};

#endif /* CommonTest_h */

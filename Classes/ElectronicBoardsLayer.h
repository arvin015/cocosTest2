//
//  ElectronicBoardsLayer.h
//  CocosTest2
//
//  Created by arvin on 2017/10/12.
//
//

#ifndef ElectronicBoardsLayer_h
#define ElectronicBoardsLayer_h

#include <stdio.h>
#include "BaseLayer.h"

class ElectronicBoardsLayer : public BaseLayer {

public:
    
    ElectronicBoardsLayer();
    ~ElectronicBoardsLayer();
    
    virtual void onEnter();
    virtual void onExit();
    
    virtual bool init();
    
    CREATE_FUNC(ElectronicBoardsLayer);
    
private:
};

#endif /* ElectronicBoardsLayer_h */

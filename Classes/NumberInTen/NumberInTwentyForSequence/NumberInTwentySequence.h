//
//  NumberInTwentySequence.h
//  CocosTest2
//
//  Created by arvin on 2017/9/27.
//
//

#ifndef NumberInTwentySequence_h
#define NumberInTwentySequence_h

#include <stdio.h>
#include "BaseLayer.h"

class SequenceView;

class NumberInTwentySequenceLayer : public BaseLayer {

public:
    
    NumberInTwentySequenceLayer();
    ~NumberInTwentySequenceLayer();
    
    virtual void onEnter();
    virtual void onExit();
    
    virtual bool init();
    
    CREATE_FUNC(NumberInTwentySequenceLayer);
    
private:
    
    SequenceView* sequenceView;
    
};

#endif /* NumberInTwentySequence_h */

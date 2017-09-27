//
//  SequenceView.h
//  CocosTest2
//
//  Created by arvin on 2017/9/27.
//
//

#ifndef SequenceView_h
#define SequenceView_h

#include <stdio.h>
#include "cocos2d.h"
#include "ui/UILayout.h"

class SequenceView : public cocos2d::ui::Layout {

public:
    
    SequenceView();
    ~SequenceView();
    
    virtual void onEnter();
    virtual void onExit();
    
    virtual bool init();
    
    CREATE_FUNC(SequenceView);
    
private:
    
};

#endif /* SequenceView_h */

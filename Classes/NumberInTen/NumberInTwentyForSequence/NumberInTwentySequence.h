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
#include "json/document.h"

class SequenceView;

class NumberInTwentySequenceLayer : public BaseLayer {

public:
    
    NumberInTwentySequenceLayer();
    ~NumberInTwentySequenceLayer();
    
    virtual void onEnter();
    virtual void onExit();
    
    virtual bool init();
    
    CREATE_FUNC(NumberInTwentySequenceLayer);
    
    /**
     * 返回处理
     */
    virtual void onBackHandle();
    
    /**
     * fromJson
     */
    void fromJson(const rapidjson::Value &json);
    
    /**
     * toJson
     */
    void toJson(rapidjson::Document &json);
    
private:
    SequenceView* sequenceView;
    bool isDanced;
};

#endif /* NumberInTwentySequence_h */

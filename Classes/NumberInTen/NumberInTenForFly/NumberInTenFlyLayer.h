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
#include "json/document.h"

class FlyView;

class NumberInTenFlyLayer : public BaseLayer {
  
public:

    NumberInTenFlyLayer();
    ~NumberInTenFlyLayer();
    
    virtual void onEnter();
    virtual void onExit();
    
    virtual bool init();
    
    CREATE_FUNC(NumberInTenFlyLayer);
    
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
    FlyView* flyView;
    bool isDanced;
};

#endif /* NumberInTenFlyLayer_h */

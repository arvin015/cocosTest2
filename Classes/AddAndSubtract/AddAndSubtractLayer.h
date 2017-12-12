//
//  AddAndSubtractLayer.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/12/5.
//

#ifndef AddAndSubtractLayer_h
#define AddAndSubtractLayer_h

#include <stdio.h>
#include "BaseLayer.h"
#include "json/document.h"

class AddAndSubtractNode;

class AddAndSubtractLayer : public BaseLayer {

public:

    AddAndSubtractLayer();
    virtual ~AddAndSubtractLayer();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();

    CREATE_FUNC(AddAndSubtractLayer);

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

    AddAndSubtractNode* addAndSubtractNode;
    cocos2d::ui::Button* restoreBtn;
    bool isDanced;
};

#endif /* AddAndSubtractLayer_h */

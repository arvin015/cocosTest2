//
//  FillLayer.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2019/2/21.
//

#ifndef FillLayer_h
#define FillLayer_h

#include <stdio.h>
#include "BaseCutfillLayer.h"

class FillLayer : public BaseCutfillLayer {

public:

    FillLayer();
    virtual ~FillLayer();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();
    CREATE_FUNC(FillLayer);

private:

    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    /**
     * 填补
     */
    void fillSolid(const VertexInfo &startVertex, const cocos2d::Vec2 &endPoint);

    /**
     * 初始坐标转摄像头旋转后的屏幕坐标---未发生移动
     */
    Vec2 original2dToScreen2d(const Vec2 &originalVec);

private:

    cocos2d::EventListenerTouchOneByOne* touchListener;
};

#endif /* FillLayer_h */

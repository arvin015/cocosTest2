//
//  CutLayer.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2019/2/21.
//

#ifndef CutLayer_h
#define CutLayer_h

#include <stdio.h>
#include "BaseCutfillLayer.h"

class CutLayer : public BaseCutfillLayer {

public:

    CutLayer();
    virtual ~CutLayer();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();
    CREATE_FUNC(CutLayer);

private:

    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    bool getEndPoint(const cocos2d::Vec2 &point, const cocos2d::Vec2 &startPoint, VertexInfo &rs);

    /**
     * 切割
     */
    void cutSolid(Solid3D* solid3D, const VertexInfo &ver1, const VertexInfo &ver2);

    /**
     * 获取切割分离移位方向
     */
    Solid3D::TRANS_DIRECTION getTransDirection(const cocos2d::Vec2 &p1, const cocos2d::Vec2 &p2);

private:

    cocos2d::EventListenerTouchOneByOne* touchListener;
};

#endif /* CutLayer_h */

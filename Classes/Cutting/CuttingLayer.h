//
//  CuttingLayer.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/12/13.
//

#ifndef CuttingLayer_h
#define CuttingLayer_h

#include <stdio.h>
#include "BaseLayer.h"
#include "PolygonView.h"

class CuttingLayer : public BaseLayer {

public:

    CuttingLayer();
    virtual ~CuttingLayer();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();

    CREATE_FUNC(CuttingLayer);

private:

    /**
     * 创建多边形
     */
    void createPolygon(std::vector<PolygonPoint> pointList);

    /**
     * 改变玩法
     */
    void changeMethod(PolygonView::OperateType operateType);

private:

    cocos2d::Vector<PolygonView*> polygonList;

};

#endif /* CuttingLayer_h */

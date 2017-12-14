//
//  PolygonSegment.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/12/13.
//

#ifndef PolygonSegment_h
#define PolygonSegment_h

#include <stdio.h>
#include "cocos2d.h"
#include "PolygonPoint.h"

class PolygonSegment : public cocos2d::Ref {

public:

    PolygonSegment();
    virtual ~PolygonSegment();

    PolygonSegment(PolygonPoint &prePoint, PolygonPoint &nextPoint, cocos2d::Node* dependNode)
            : dependNode(
            nullptr) {
        this->prePoint = prePoint;
        this->nextPoint = nextPoint;
        this->dependNode = dependNode;
    }

    /**
     * 判断两线段是否相交
     */
    bool checkIsIntersect(float x1, float y1, float x2, float y2, PolygonPoint &prePoint, float &intersectX, float &intersectY);

private:
    PolygonPoint prePoint;
    PolygonPoint nextPoint;

    cocos2d::Node* dependNode; //线段依赖Node
};

#endif /* PolygonSegment_h */

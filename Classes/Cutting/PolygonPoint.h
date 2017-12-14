//
//  PolygonPoint.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/12/13.
//

#ifndef PolygonPoint_h
#define PolygonPoint_h

#include <stdio.h>
#include "cocos2d.h"

USING_NS_CC;

class PolygonPoint : public cocos2d::Point {

public:

    PolygonPoint()
    : Point()
    , isBreakPoint(false) {
    };
    ~PolygonPoint() {};

    PolygonPoint(float x, float y)
    : Point(x, y)
    , isBreakPoint(false) {
    };

public:

    bool isBreakPoint;

};

#endif /* PolygonPoint_h */

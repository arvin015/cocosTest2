//
//  PolygonSegment.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/12/13.
//

#include "PolygonSegment.h"
#include "MathUtils.h"

USING_NS_CC;

PolygonSegment::PolygonSegment()
: dependNode(nullptr) {

}

PolygonSegment::~PolygonSegment() {

}

bool PolygonSegment::checkIsIntersect(float x1, float y1, float x2, float y2, PolygonPoint &p, float &intersectX, float &intersectY) {

    //转换成世界坐标
    Point preP = dependNode->convertToWorldSpace(prePoint);
    Point nextP = dependNode->convertToWorldSpace(nextPoint);

    Point c = Point(x1, y1);
    Point d = Point(x2, y2);

    prePoint.x = preP.x;
    prePoint.y = preP.y;

    Point crossPoint;

    if (intersection(preP, nextP, c, d, crossPoint) == 1) {
        p = prePoint;
        intersectX = crossPoint.x;
        intersectY = crossPoint.y;
        return true;
    }

    return false;
}

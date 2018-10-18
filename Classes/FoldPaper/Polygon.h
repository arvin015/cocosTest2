//
//  Polygon.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2018/10/18.
//

#ifndef Polygon_h
#define Polygon_h

#include <stdio.h>
#include "cocos2d.h"
#include "MathUtils.h"

namespace FoldPaper {

    class Edge : public cocos2d::Ref {

    public:

        cocos2d::Vec2 prePoint;
        cocos2d::Vec2 nextPoint;

        Edge() {}

        Edge(const cocos2d::Vec2 &prePoint, const cocos2d::Vec2 &nextPoint) {
            this->prePoint = prePoint;
            this->nextPoint = nextPoint;
        }

        ~Edge() {}

        float getLength() {
            return prePoint.distance(nextPoint);
        }

        /**
         * 判断两线段是否相交
         * @param p1
         * @param p2
         * @param intersect
         * @return
         */
        bool checkIsIntersect(const Point &p1, const Point &p2, Point &intersect) {
            if (intersection(prePoint, nextPoint, p1, p2, intersect) == 1) {
                return true;
            }
            return false;
        }
    };

    class Polygon : public cocos2d::Ref {

    public:
        Polygon();

        Polygon(std::vector<cocos2d::Vec2> vertexList);

        ~Polygon();

        /**
         * 检测点是否在多边形内
         */
        bool checkIsInPolygon(const cocos2d::Vec2 &point);
        
        /**
         * 获取顶点个数
         */
        int getVertexNum() {
            return vertexList.size();
        }

    private:

        /**
         * 生成边
         */
        void productEdges();

    public:
        std::vector<cocos2d::Vec2> vertexList; //点集合
        std::vector<Edge> edgeList; //边集合
    };
}

#endif /* Polygon_h */

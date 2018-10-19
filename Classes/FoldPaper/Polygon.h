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
        bool checkIsIntersect(const cocos2d::Vec2 &p1, const cocos2d::Vec2 &p2, cocos2d::Vec2 &intersect) {
            if (intersection(prePoint, nextPoint, p1, p2, intersect) > 0) {
                return true;
            }
            return false;
        }
    };

    class Polygon : public cocos2d::Ref {

    public:
        Polygon();

        Polygon(std::vector<cocos2d::Vec2> vertexList, const cocos2d::Vec2 &centerPoint);

        ~Polygon();

        /**
         * 检测点是否在多边形内
         */
        bool checkIsInPolygon(const cocos2d::Vec2 &point);
        
        /**
         * 获取顶点个数
         */
        int getVertexNum() {
            return (int)vertexList.size();
        }
        
        /**
         * 获取顶点坐标
         */
        cocos2d::Vec2 getVertexPos(int i) {
            return vertexList.at(i);
        }

        /**
         * 将世界坐标转成Node坐标
         * @param targetNode
         */
        void changePointsToNode(cocos2d::Node* targetNode);

    private:

        /**
         * 生成边
         */
        void productEdges();

        /**
         * 获取多边形边框
         * @return
         */
        Rect getBoundBox();

    public:
        Rect boundBox; //边框
        cocos2d::Vec2 centerPoint; //中心点
        std::vector<cocos2d::Vec2> vertexList; //点集合
        std::vector<Edge> edgeList; //边集合
    };
}

#endif /* Polygon_h */

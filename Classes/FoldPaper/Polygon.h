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

#define PI 3.14159

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

        /**
         * 获取边长
         */
        float getLength() const {
            return prePoint.distance(nextPoint);
        }

        /**
         * 获取边中点
         */
        Vec2 getMidPoint() {
            return prePoint.getMidpoint(nextPoint);;
        }

        /**
         * 比较两条边是否相等
         */
        bool isEqual(const Edge &otherEdge) {
            return abs(getLength() - otherEdge.getLength()) < FLT_EPSILON;;
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

        /**
         * 计算两条边的夹角
         */
        inline static float crossDegree(const cocos2d::Vec2 &pA, const cocos2d::Vec2 &pB,
                                        const cocos2d::Vec2 &pC, const cocos2d::Vec2 &pD) {

            if (abs(pA.x - pB.x) < FLT_EPSILON || abs(pC.x - pD.x) < FLT_EPSILON) {
                float r = getDifferDegree(pA, pB, pC, pD);
                return abs(r);
            }

            float k1, b1;
            float k2, b2;
            getLineFormula(pA, pB, k1, b1);
            getLineFormula(pC, pD, k2, b2);

            return atanf(abs((k2 - k1) / (1 + k1 * k2))) * 180.0  / PI;
        }

        /**
         * 计算两条边的相差角度
         */
        inline static float getDifferDegree(const cocos2d::Vec2 &referPA, const cocos2d::Vec2 &referPB,
                                        const cocos2d::Vec2 &pC, const cocos2d::Vec2 &pD) {

            float referDegree, degree;

            if (abs(referPA.x - referPB.x) < FLT_EPSILON) {
                referDegree = 90;
            } else {
                float k1, b1;
                getLineFormula(referPA, referPB, k1, b1);
                referDegree = atanf(k1) * 180.0 / PI;
                if (referDegree < 0) referDegree = 180 + referDegree;
            }

            if (abs(pC.x - pD.x) < FLT_EPSILON) {
                degree = 90;
            } else {
                float k2, b2;
                getLineFormula(pC, pD, k2, b2);
                degree = atanf(k2) * 180.0  / PI;
                if (degree < 0) degree = 180 + degree;
            }

            float result = degree - referDegree;
            if (result > 90) result = result - 180;
            if (result < -90) result = 180 + result;

            return result;
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

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

    class Vertex2D {
    public:
        Vertex2D() {}
        Vertex2D(const cocos2d::Vec2 &position, const cocos2d::Vec2 &uv) {
            this->position = position;
            this->uv = uv;
        }
        ~Vertex2D() {}

        cocos2d::Vec2 position;
        cocos2d::Vec2 uv;
    };

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
            return fabsf(getLength() - otherEdge.getLength()) < 0.5;
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
         * 计算两条边的夹角---夹角小于90
         */
        inline static float crossDegree(const cocos2d::Vec2 &pA, const cocos2d::Vec2 &pB,
                                        const cocos2d::Vec2 &pC, const cocos2d::Vec2 &pD) {

            if (fabsf(pA.x - pB.x) < FLT_EPSILON && fabsf(pC.x - pD.x) < FLT_EPSILON) {
                return 0;
            } else if (fabsf(pA.x - pB.x) < FLT_EPSILON) {
                return fabsf(90 - CC_RADIANS_TO_DEGREES(atanf(fabsf((pD.y - pC.y) / (pD.x - pC.x)))));
            } else if (fabsf(pC.x - pD.x) < FLT_EPSILON) {
                return fabsf(90 - CC_RADIANS_TO_DEGREES(atanf(fabsf((pA.y - pB.y) / (pA.x - pB.x)))));
            }

            float k1 = (pB.y - pA.y) / (pB.x - pA.x);
            float k2 = (pD.y - pC.y) / (pD.x - pC.x);

            return CC_RADIANS_TO_DEGREES(atanf(fabsf((k2 - k1) / (1 + k1 * k2))));
        }

        /**
         * 计算两条边的相差角度---最小角度
         */
        inline static float getDifferDegree(const cocos2d::Vec2 &referPA, const cocos2d::Vec2 &referPB,
                                        const cocos2d::Vec2 &pC, const cocos2d::Vec2 &pD) {

            float referDegree, degree;

            if (fabsf(referPA.x - referPB.x) < FLT_EPSILON) {
                referDegree = 90;
            } else {
                float k1 = (referPB.y - referPA.y) / (referPB.x - referPA.x);
                referDegree = CC_RADIANS_TO_DEGREES(atanf(k1));
                if (referDegree < 0) referDegree = 180 + referDegree;
            }

            if (fabsf(pC.x - pD.x) < FLT_EPSILON) {
                degree = 90;
            } else {
                float k2 = (pD.y - pC.y) / (pD.x - pC.x);
                degree = CC_RADIANS_TO_DEGREES(atanf(k2));
                if (degree < 0) degree = 180 + degree;
            }

            float result = degree - referDegree;
            if (result > 90) result = result - 180;
            if (result < -90) result = 180 + result;

            return result;
        }

        /**
         * 两条边是否满足吸附
         * @param edgePP1
         * @param edgeNP1
         * @param edgePP2
         * @param edgeNP2
         * @param minDegree
         * @param minDis
         * @return
         */
        inline static bool isCloseEnough(const cocos2d::Vec2 &edgePP1, const cocos2d::Vec2 &edgeNP1,
                                         const cocos2d::Vec2 &edgePP2, const cocos2d::Vec2 &edgeNP2,
                                         float minDegree, float minDis) {

            Vec2 midPoint1 = edgePP1.getMidpoint(edgeNP1);
            Vec2 midPoint2 = edgePP2.getMidpoint(edgeNP2);

            float length1 = edgePP1.distance(edgeNP1);
            float length2 = edgePP2.distance(edgeNP2);

            float midDistance = midPoint1.distance(midPoint2);
            float crossDegree = Edge::crossDegree(edgePP1, edgeNP1, edgePP2, edgeNP2);

            if (fabsf(length1 - length2) < 0.5f && midDistance <= minDis && crossDegree <= minDegree) { //满足吸附条件
                return true;
            }

            return false;
        }

        /**
         * 圆心是否可吸附该边
         */
        inline bool isCircleCloseEnough(const cocos2d::Vec2 &centerPoint, float minDis, float radius, cocos2d::Vec2 &c) {
            if (fabsf(2 * M_PI * radius - getLength()) > 0.2f) return false;
            float dot;
            float dis = Dis_PointToLineSegment(prePoint, nextPoint, centerPoint, dot, c);
            if (dis < (minDis + radius) && (dot > 0 && dot < 1)) return true;
            return false;
        }
    };

    class Polygon : public cocos2d::Ref {

    public:
        Polygon();

        Polygon(std::vector<Vertex2D> vertexList, const cocos2d::Vec2 &centerPoint);

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
            return vertexList.at(i).position;
        }

        /**
         * 将世界坐标转成Node坐标
         * @param targetNode
         */
        void changePointsToNode(cocos2d::Node* targetNode);

        /**
         * 获取中心点坐标
         * @return
         */
        cocos2d::Vec2 getCenterPoint();

        /**
         * 圆是否可吸附该Polygon
         */
        bool isCircleCloseEnough(const cocos2d::Vec2 &centerPoint, float minDis, float radius, cocos2d::Vec2 &c);

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
        std::vector<Vertex2D> vertexList; //点集合
        std::vector<Edge> edgeList; //边集合
    };
}

#endif /* Polygon_h */

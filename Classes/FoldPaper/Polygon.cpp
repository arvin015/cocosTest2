//
//  Polygon.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2018/10/18.
//

#include "Polygon.h"

USING_NS_CC;
using namespace std;

namespace FoldPaper {

    Polygon::Polygon() {

    }

    Polygon::Polygon(vector<Vec2> vertexList, const Vec2 &centerPoint)
    : Polygon() {
        this->vertexList = vertexList;
        this->centerPoint = centerPoint;
        boundBox = getBoundBox();
    }

    Polygon::~Polygon() {

    }

    bool Polygon::checkIsInPolygon(const Vec2 &point) {
        int intersect = 0;
        for (Edge edge : edgeList) {
            Vec2 crossPoint;
            if (edge.checkIsIntersect(Vec2(-10, point.y), point, crossPoint)) {
                intersect++;
            }
        }

        if (intersect % 2 != 0) {
            return true;
        } else {
            return false;
        }
    }

    void Polygon::changePointsToNode(Node* targetNode) {
        vector<Vec2> list;
        for (Vec2 vertex : vertexList) {
            Vec2 p = targetNode->convertToNodeSpace(vertex);
            list.push_back(p);
        }
        vertexList = list;
        boundBox = getBoundBox();
        productEdges();
    }

    Vec2 Polygon::getCenterPoint() {
        Vec2 center;
        for (Vec2 vertex : vertexList) {
            center += vertex;
        }
        centerPoint = center / vertexList.size();
        return centerPoint;
    }

    void Polygon::productEdges() {

        edgeList.clear();

        for (int i = 0; i < vertexList.size(); i++) {
            if (i == vertexList.size() - 1) {
                edgeList.push_back(Edge(vertexList[i], vertexList[0]));
            } else {
                edgeList.push_back(Edge(vertexList[i], vertexList[i + 1]));
            }
        }
    }

    Rect Polygon::getBoundBox() {
        Rect rect;
        float minX = vertexList[0].x, minY = vertexList[0].y;
        float maxX = vertexList[0].x, maxY = vertexList[0].y;

        for (int i = 1; i < vertexList.size(); i++) {
            Vec2 vertex = vertexList[i];
            if (vertex.x < minX) {
                minX = vertex.x;
            } else if (vertex.x > maxX) {
                maxX = vertex.x;
            }
            if (vertex.y < minY) {
                minY = vertex.y;
            } else if (vertex.y > maxY) {
                maxY = vertex.y;
            }
        }

        rect.setRect(minX, minY, maxX - minX, maxY - minY);

        return rect;
    }
}

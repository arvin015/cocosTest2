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

    Polygon::Polygon(vector<Vertex2D> vertexList, const Vec2 &centerPoint)
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
        vector<Vertex2D> list;
        for (Vertex2D vertex : vertexList) {
            Vec2 p = targetNode->convertToNodeSpace(vertex.position);
            list.push_back(Vertex2D(p, vertex.uv));
        }
        vertexList = list;
        boundBox = getBoundBox();
        productEdges();
    }

    Vec2 Polygon::getCenterPoint() {
        Vec2 center;
        for (Vertex2D vertex : vertexList) {
            center += vertex.position;
        }
        centerPoint = center / vertexList.size();
        return centerPoint;
    }

    void Polygon::productEdges() {

        edgeList.clear();

        for (int i = 0; i < vertexList.size(); i++) {
            if (i == vertexList.size() - 1) {
                edgeList.push_back(Edge(vertexList[i].position, vertexList[0].position));
            } else {
                edgeList.push_back(Edge(vertexList[i].position, vertexList[i + 1].position));
            }
        }
    }

    Rect Polygon::getBoundBox() {
        Rect rect;
        float minX = vertexList[0].position.x, minY = vertexList[0].position.y;
        float maxX = vertexList[0].position.x, maxY = vertexList[0].position.y;

        for (int i = 1; i < vertexList.size(); i++) {
            Vertex2D vertex = vertexList[i];
            if (vertex.position.x < minX) {
                minX = vertex.position.x;
            } else if (vertex.position.x > maxX) {
                maxX = vertex.position.x;
            }
            if (vertex.position.y < minY) {
                minY = vertex.position.y;
            } else if (vertex.position.y > maxY) {
                maxY = vertex.position.y;
            }
        }

        rect.setRect(minX, minY, maxX - minX, maxY - minY);

        return rect;
    }
}

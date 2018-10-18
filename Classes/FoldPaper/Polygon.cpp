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

    Polygon::Polygon(vector<Vec2> vertexList) {
        this->vertexList = vertexList;
        productEdges();
    }

    Polygon::~Polygon() {

    }

    bool Polygon::checkIsInPolygon(const Vec2 &point) {
        int intersect = 0;
        for (Edge edge : edgeList) {
            Vec2 crossPoint;
            if (edge.checkIsIntersect(Vec2(0, point.y), point, crossPoint)) {
                intersect++;
            }
        }

        if (intersect % 2 != 0) {
            return true;
        } else {
            return false;
        }
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
}

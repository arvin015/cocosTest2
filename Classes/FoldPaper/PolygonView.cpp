//
//  PolygonView.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2018/10/18.
//

#include "PolygonView.h"
#include "Polygon.h"

USING_NS_CC;
using namespace std;

#define PI 3.14

namespace FoldPaper {

    PolygonView::PolygonView() :
            polygonDrawNode(nullptr) {

    }

    PolygonView::~PolygonView() {

    }

    void PolygonView::onEnter() {

    }

    void PolygonView::onExit() {

    }

    bool PolygonView::init() {
        if (!Node::init()) {
            return false;
        }
        
        polygonDrawNode = DrawNode::create();
        
        return true;
    }
    
    void PolygonView::initView() {
        polygonDrawNode->drawPolygon(polygon.vertexList, polygon.getVertexNum(), Color4F::BLUE, 2, Color4F::BLACK)
    }

    void PolygonView::createSquare(const Vec2 &centerPoint, float width, float height) {
        if (width <= 0.0f || height <= 0.0f)
            return;

        vector<Vec2> vertexList;
        
        float cx = centerPoint.x;
        float cy = centerPoint.y;
        
        vertexList.push_back(Vec2(cx - width / 2, cy + height / 2));
        vertexList.push_back(Vec2(cx + width / 2, cy + height / 2));
        vertexList.push_back(Vec2(cx + width / 2, cy - height / 2));
        vertexList.push_back(Vec2(cx - width / 2, cy - height / 2));
        
        polygon = Polygon(vertexList);
    }

    void PolygonView::createRegularPolygonWithRadius(const Vec2 &centerPoint, int edge, float radius) {
        if (edge < 3)
            return;

        vector<Vec2> vertexList;
        
        float angle_inc = -2 * PI / (float)edge;
        float angle = 0.5f * PI - 0.5f * angle_inc;
        for (int i = 0; i < edge; i++) {
            float x = radius * cos(angle);
            float y = radius * sin(angle);
            vertexList.push_back(centerPoint + Vec2(x, y));
            angle += angle_inc;
        }
        
        polygon = Polygon(vertexList);
    }

    void PolygonView::createRegularPolygonWithEdgeLength(const Vec2 &center, int edge, float edgeLength) {
        if (edge < 3)
            return;
        
        float angle = PI / (float) edge;
        float radius = edgeLength / (2.0f * sin(angle));
        createRegularPolygonWithRadius(center, edge, radius);
        
        polygon = Polygon(vertexList);
    }
}

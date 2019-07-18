//
// Created by Administrator on 2019/6/1.
//

#include "VectorTestLayer.h"
#include "PolyView.h"

USING_NS_CC;

VectorTestLayer::VectorTestLayer() : base()
, curPolyView(nullptr) {

}

VectorTestLayer::~VectorTestLayer() {

}

void VectorTestLayer::onEnter() {
    base::onEnter();
}

void VectorTestLayer::onExit() {
    base::onExit();
}

bool VectorTestLayer::init() {
    if (!base::init()) {
        return false;
    }

    auto polyView1 = createPolyView(Vec2(300, 300), 160, PolyView::ARC);
    addChild(polyView1);

    auto polyView2 = createPolyView(Vec2(100, 100), 80, PolyView::CIRCLE);
    addChild(polyView2);

    polyViewList.pushBack(polyView1);
    polyViewList.pushBack(polyView2);
    
    Vec2 lp1 = Vec2(1, 1);
    Vec2 lp2 = Vec2(2, 2);
    Vec2 lp3 = Vec2(2, 0);
    Vec2 lp4 = Vec2(1, 2);
    
    Vec2 v1 = (lp2 - lp1).getNormalized();
    Vec2 v2 = (lp3 - lp4).getNormalized();
    float radian = acosf(v1.dot(v2) / (v1.length() * v2.length()));
    log("-----------------degree = %f", CC_RADIANS_TO_DEGREES(radian));

    return true;
}

PolyView* VectorTestLayer::createPolyView(const Vec2 &centerP, float radius, int polyType) {
    auto polyView = PolyView::create();
    if (polyType == PolyView::ARC) {
        polyView->createArc(centerP, radius);
    } else if (polyType == PolyView::CIRCLE) {
        polyView->createCircle(centerP, radius);
    }
    polyView->setOnTouchEndListener([this, polyView]() {
        for (PolyView* targetView : polyViewList) {
            if (targetView == polyView) continue;
            if (polyView->checkIsCloseEnough(targetView)) {
                polyView->attach(targetView);
            }
        }
    });
    return polyView;
}

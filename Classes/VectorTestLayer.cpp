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

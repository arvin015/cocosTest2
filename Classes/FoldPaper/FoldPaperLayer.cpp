//
//  FoldPaperLayer.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2018/10/18.
//

#include "FoldPaperLayer.h"
#include "PolygonView.h"

USING_NS_CC;

#define RangeDegree 15
#define RangeDistance 20

namespace FoldPaper {

    FoldPaperLayer::FoldPaperLayer():
            ids(1) {

    }

    FoldPaperLayer::~FoldPaperLayer() {

    }

    void FoldPaperLayer::onEnter() {
        BaseLayer::onEnter();
    }

    void FoldPaperLayer::onExit() {

        BaseLayer::onExit();
    }

    bool FoldPaperLayer::init() {
        if (!BaseLayer::init()) {
            return false;
        }

        //test PolygonView
        auto polygonView = PolygonView::create();
        polygonView->createSquare(Vec2(1024 / 2, 768 / 2), 150, 150);
        polygonView->initView();
        polygonView->setTag(ids);
        polygonView->setOnTouchEndCallback([this, polygonView]() {
            attachPolygons(polygonView);
        });
        addChild(polygonView);
        polygonViewList.pushBack(polygonView);

        ids++;

        auto polygonView3 = PolygonView::create();
        polygonView3->createSquare(Vec2(1024 / 2 - 200, 768 / 2), 150, 150);
        polygonView3->initView();
        polygonView3->setTag(ids);
        polygonView3->setOnTouchEndCallback([this, polygonView3]() {
            attachPolygons(polygonView3);
        });
        addChild(polygonView3);
        polygonViewList.pushBack(polygonView3);

        ids++;

        auto polygonView1 = PolygonView::create();
        polygonView1->createRegularPolygonWithRadius(Vec2(1024 / 2, 768 / 2 + 200), 6, 80);
        polygonView1->initView();
        polygonView1->setTag(ids);
        polygonView1->setOnTouchEndCallback([this, polygonView1]() {
            attachPolygons(polygonView1);
        });
        addChild(polygonView1);
        polygonViewList.pushBack(polygonView1);

        ids++;

        auto polygonView4 = PolygonView::create();
        polygonView4->createRegularPolygonWithRadius(Vec2(1024 / 2 - 200, 768 / 2 + 200), 6, 80);
        polygonView4->initView();
        polygonView4->setTag(ids);
        polygonView4->setOnTouchEndCallback([this, polygonView4]() {
            attachPolygons(polygonView4);
        });
        addChild(polygonView4);
        polygonViewList.pushBack(polygonView4);

        ids++;

        auto polygonView2 = PolygonView::create();
        polygonView2->createRegularPolygonWithRadius(Vec2(1024 / 2, 768 / 2 - 200), 5, 80);
        polygonView2->initView();
        polygonView2->setTag(ids);
        polygonView2->setOnTouchEndCallback([this, polygonView2]() {
            attachPolygons(polygonView2);
        });
        addChild(polygonView2);
        polygonViewList.pushBack(polygonView2);

        ids++;

        auto polygonView5 = PolygonView::create();
        polygonView5->createRegularPolygonWithRadius(Vec2(1024 / 2 - 200, 768 / 2 - 200), 5, 80);
        polygonView5->initView();
        polygonView5->setTag(ids);
        polygonView5->setOnTouchEndCallback([this, polygonView5]() {
            attachPolygons(polygonView5);
        });
        addChild(polygonView5);
        polygonViewList.pushBack(polygonView5);

        return true;
    }

    void FoldPaperLayer::attachPolygons(PolygonView* polygonView) {

        for (Edge edge : polygonView->polygon.edgeList) {

            Vec2 preWorldPoint = polygonView->getPolygonViewWorldPoint(edge.prePoint);
            Vec2 nextWorldPoint = polygonView->getPolygonViewWorldPoint(edge.nextPoint);
            Vec2 midPoint = polygonView->getPolygonViewWorldPoint(edge.getMidPoint());

            for (PolygonView* referPolygonView : polygonViewList) {
                if (referPolygonView->getTag() != polygonView->getTag()) {

                    for (Edge e : referPolygonView->polygon.edgeList) {
                        Vec2 point1 = referPolygonView->getPolygonViewWorldPoint(e.prePoint);
                        Vec2 point2 = referPolygonView->getPolygonViewWorldPoint(e.nextPoint);
                        Vec2 midPoint1 = referPolygonView->getPolygonViewWorldPoint(e.getMidPoint());

                        float midDistance = midPoint.distance(midPoint1);
                        float crossDegree = Edge::crossDegree(point1, point2, preWorldPoint, nextWorldPoint);
                        if (edge.isEqual(e) && midDistance < RangeDistance && crossDegree < RangeDegree) { //满足吸附条件

                            //旋转至与参照多边形相同角度
                            float differDegree = Edge::getDifferDegree(point1, point2, preWorldPoint, nextWorldPoint);
                            polygonView->rotatePolygon(differDegree);

                            //平移至与参照多边形贴合
                            Vec2 newMidPoint = polygonView->getPolygonViewWorldPoint(edge.getMidPoint());
                            polygonView->movePolygon(Vec2(midPoint1.x - newMidPoint.x, midPoint1.y - newMidPoint.y));

                            return;
                        }
                    }
                }
            }
        }
    }
}

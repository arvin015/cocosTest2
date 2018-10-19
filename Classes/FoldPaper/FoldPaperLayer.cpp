//
//  FoldPaperLayer.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2018/10/18.
//

#include "FoldPaperLayer.h"
#include "PolygonView.h"

USING_NS_CC;

namespace FoldPaper {
    
    FoldPaperLayer::FoldPaperLayer() {
        
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
        addChild(polygonView);
        
        auto polygonView1 = PolygonView::create();
        polygonView1->createRegularPolygonWithRadius(Vec2(1024 / 2, 768 / 2 + 200), 6, 80);
        polygonView1->initView();
        addChild(polygonView1);

        auto polygonView2 = PolygonView::create();
        polygonView2->createRegularPolygonWithRadius(Vec2(1024 / 2, 768 / 2 - 200), 5, 80);
        polygonView2->initView();
        addChild(polygonView2);
        
        return true;
    }
}

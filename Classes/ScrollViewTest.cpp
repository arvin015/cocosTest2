//
//  ScrollViewTest.cpp
//  CocosTest2-mobile
//
//  Created by arvin on 2020/11/23.
//

#include "ScrollViewTest.h"
#include "ui/UIImageView.h"

USING_NS_CC;
using namespace ui;

ScrollViewTest::ScrollViewTest() {
    
}

ScrollViewTest::~ScrollViewTest() {
    
}

void ScrollViewTest::onEnter() {
    base::onEnter();
}

void ScrollViewTest::onExit() {
    base::onExit();
}

bool ScrollViewTest::init() {
    if (!base::init()) {
        return false;
    }
    
    bgLayer = LayerColor::create(Color4B::BLACK);
    bgLayer->setOpacity(0);
    addChild(bgLayer);
    
    auto topImage = ImageView::create("Hello.png");
    topImage->setContentSize(Size(632, 320));
    topImage->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    topImage->setPosition(Vec2(V_WIDTH / 2, V_HEIGHT));
    topImage->setScale9Enabled(true);
    addChild(topImage);
    
    scrollView = ScrollView::create();
    scrollView->setContentSize(Size(632, V_HEIGHT));
    scrollView->setIgnoreAnchorPointForPosition(false);
    scrollView->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    scrollView->setPosition(Vec2(V_WIDTH / 2, V_HEIGHT));
    scrollView->setScrollBarOpacity(0);
    scrollView->setBounceEnabled(true);
    scrollView->addEventListener([this](Ref* pSender, ScrollView::EventType type) {
        if (type == ScrollView::EventType::CONTAINER_MOVED) {
            float offsetY = scrollView->getInnerContainerPosition().y - (scrollView->getContentSize().height - scrollView->getInnerContainerSize().height);
            if (offsetY >= 330) {
                bgLayer->setOpacity(100);
            } else {
                bgLayer->setOpacity(offsetY / 330 * 100);
            }
        }
    });
    addChild(scrollView);
    
    float innerHeight = 20 * 100 + 330;
    scrollView->setInnerContainerSize(Size(632, innerHeight));
    for (int i = 0; i < 20; i++) {
        Sprite* spr = Sprite::create("edx_function_03.png");
        spr->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
        spr->setPosition(Vec2(scrollView->getContentSize().width / 2, innerHeight - 330 - i * 100));
        scrollView->addChild(spr);
    }
    return true;
}

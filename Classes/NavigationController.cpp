//
//  NavigationController.cpp
//  CocosTest2
//
//  Created by arvin on 2017/8/21.
//
//

#include "NavigationController.h"

USING_NS_CC;

NavigationController::NavigationController() {

}

NavigationController::~NavigationController() {

}

bool NavigationController::init() {
    if(!Layer::init()) {
        return false;
    }
    
    return true;
}

void NavigationController::pushView(cocos2d::Layer* layer) {

    layer->retain();
    
    if(viewList.size() > 0) {
        viewList.at(viewList.size() - 1)->removeFromParent();
    }
    
    addChild(layer);
    viewList.pushBack(layer);
}

void NavigationController::popView() {

    if(viewList.size() > 1) {
        Layer* topLayer = getTopView();
        if(topLayer) {
            topLayer->removeFromParent();
            topLayer = nullptr;
        }
        
        viewList.popBack();
        
        Layer* showLayer = getTopView();
        addChild(showLayer);
    }
}

Layer* NavigationController::getTopView() {
    if(viewList.size() > 0) {
        return viewList.at(viewList.size() - 1);
    }
    
    return nullptr;
}

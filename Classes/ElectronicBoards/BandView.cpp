//
//  BandView.cpp
//  CocosTest2
//
//  Created by arvin on 2017/10/13.
//
//

#include "BandView.h"

USING_NS_CC;
using namespace ui;
using namespace std;

const float BAND_WIDTH = 4;

BandView::BandView() {
    eventListener = nullptr;
    lineDraw = nullptr;
    lineColor = Color4F::YELLOW;
    enableTouch = true;
    operateSnapImage = nullptr;
    touchEndedCallback = nullptr;
}

BandView::~BandView() {

}

void BandView::onEnter() {
    Node::onEnter();
    
    eventListener = EventListenerTouchOneByOne::create();
    eventListener->onTouchBegan = CC_CALLBACK_2(BandView::onTouchBegan, this);
    eventListener->onTouchMoved = CC_CALLBACK_2(BandView::onTouchMoved, this);
    eventListener->onTouchEnded = CC_CALLBACK_2(BandView::onTouchEnded, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, this);
}

void BandView::onExit() {

    if(eventListener) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(eventListener);
    }
    
    Node::onExit();
}

bool BandView::init() {
    if(!Node::init()) {
        return false;
    }
    
    lineDraw = DrawNode::create(BAND_WIDTH);
    this->addChild(lineDraw);
    
    return true;
}

void BandView::initData(const Vec2 &startPoint, const Vec2 &endPoint, const Color4F &color) {
    this->lineColor = color;
    
    lineDraw->clear();
    lineDraw->drawSegment(startPoint, endPoint, BAND_WIDTH, lineColor);
    
    ImageView* startSnapImage = createSnapImage("select.png", startPoint);
    this->addChild(startSnapImage, 2);
    snapImageList.pushBack(startSnapImage);
    
    ImageView* endSnapImage = createSnapImage("select.png", endPoint);
    this->addChild(endSnapImage, 2);
    snapImageList.pushBack(endSnapImage);
    
    bandLineMap.insert(make_pair(startSnapImage, endSnapImage));
}

bool BandView::onTouchBegan(Touch* touch, Event* event) {
    if(!enableTouch) {
        return false;
    }
    
    operateSnapImage = checkIsSelectSnapImage(touch->getLocation());
    if(operateSnapImage) { //获取依赖的钉子
        
        eventListener->setSwallowTouches(true);
        
        bringToFront(); //置顶
        
        dependSnapImageList.clear();
        
        //检测以它为key值是否有对应的value值
//        map<ImageView*, ImageView*>::iterator it = bandLineMap.find(operateSnapImage);
//        if(it != bandLineMap.end()) {
//            dependSnapImage.pushBack(it->second);
//        }
        
        //检测以它为key值是否有对应的value值
        //检测以它为value值是否有对应的key值
        map<ImageView*, ImageView*>::iterator it1;
        for(it1 = bandLineMap.begin(); it1 != bandLineMap.end(); it1++) {
            ImageView* keyImage = it1->first;
            ImageView* valueImage = it1->second;
            if(keyImage == operateSnapImage) {
                dependSnapImageList.pushBack(valueImage);
            }
            if(valueImage == operateSnapImage) {
                dependSnapImageList.pushBack(keyImage);
            }
        }
    }
    
    return true;
}

void BandView::onTouchMoved(Touch* touch, Event* event) {
    if(operateSnapImage && dependSnapImageList.size() > 0) {
        update(touch->getLocation());
    }
}

void BandView::onTouchEnded(Touch* touch, Event* event) {
    
    if(operateSnapImage) {
        float x = touch->getLocation().x;
        float y = touch->getLocation().y;
        
        if(touchEndedCallback) {
            this->touchEndedCallback(this, x, y);
        }
    }
    
    operateSnapImage = nullptr;
    dependSnapImageList.clear();

    eventListener->setSwallowTouches(false);
}

void BandView::update(const Vec2 &curPosition) {

    //更新位置
    if(operateSnapImage) {
        operateSnapImage->setPosition(curPosition);
    }
    
    //更新皮筋
    if(dependSnapImageList.size() > 0) {
        lineDraw->clear();
        for(ImageView* image : dependSnapImageList) {
            lineDraw->drawSegment(image->getPosition(), operateSnapImage->getPosition(), BAND_WIDTH, lineColor);
        }
    }
}

ImageView* BandView::checkIsSelectSnapImage(const Vec2 &point) {
    
    for(ImageView* snapImage : snapImageList) {
        if(snapImage->getBoundingBox().containsPoint(point)) {
            return snapImage;
        }
    }
    
    return nullptr;
}

ImageView* BandView::createSnapImage(const string &imageName, const Vec2 &position) {
    ImageView* imageView = ImageView::create(imageName);
    imageView->setPosition(position);
    return imageView;
}

void BandView::bringToFront() {
    Vector<Node*> childNodes = getParent()->getChildren();
    int localZOrder = childNodes.at(childNodes.size() - 1)->getTag();
    getParent()->reorderChild(this, localZOrder + 1);
}

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
    keyImage = nullptr;
    touchEndedCallback = nullptr;
    isUnfold = false;
    isHangBand = false;
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
    if(operateSnapImage) { //touch 在钉子上
        
        eventListener->setSwallowTouches(true);
        
        bringToFront(); //置顶
        
        isHangBand = isUnfold; //是否挂橡皮筋
        
    } else  { //touch不在钉子上，判断是否在橡皮筋上
        keyImage = checkIsOnBand(touch->getLocation());
        if(keyImage) { //touch在橡皮筋上
            
            ImageView* valueImage = bandLineMap.at(keyImage);
            
            ImageView* newImage = createSnapImage("select.png", touch->getLocation());
            this->addChild(newImage);
            snapImageList.pushBack(newImage);

            operateSnapImage = newImage;
            
            if(isUnfold) { //已展开，删除展开的那条
                map<ImageView*, ImageView*>::iterator it = bandLineMap.find(keyImage);
                if(it != bandLineMap.end()) {
                    bandLineMap.erase(it);
                }
                
                bandLineMap.insert(make_pair(newImage, valueImage));
                bandLineMap.insert(make_pair(keyImage, newImage));
                
            } else { //未展开，直接添加两条线段
                isUnfold = true;
                
                bandLineMap.insert(make_pair(newImage, keyImage));
                bandLineMap.insert(make_pair(valueImage, newImage));
            }
            
            isHangBand = true; //挂橡皮筋
        }
    }

    return true;
}

void BandView::onTouchMoved(Touch* touch, Event* event) {
    if(operateSnapImage) {
        update(touch->getLocation());
    }
}

void BandView::onTouchEnded(Touch* touch, Event* event) {
    
    if(operateSnapImage) {
        float x = touch->getLocation().x;
        float y = touch->getLocation().y;
        
        if(touchEndedCallback) {
            this->touchEndedCallback(this, x, y, isHangBand);
        }
    }
    
    operateSnapImage = nullptr;

    eventListener->setSwallowTouches(false);
}

void BandView::checkBandIsHanged(const vector<Vec2> &pointList) {

    if(operateSnapImage && keyImage) {
        ImageView* valueImage = bandLineMap.at(keyImage);
        Vector<Vec2> hangPointList;

        for(int i = 0; i < pointList.size(); i++) {

            Vec2 point = pointList.at(i);

            if(checkIsInTrig(operateSnapImage->getPosition(), keyImage->getPosition(), valueImage->getPosition(), point)) {
                hangPointList.pushBack(point);
            }
        }

        if(hangPointList.size() > 0) {
            if(hangPointList.size() > 1) {
                map<ImageView*, ImageView*>::iterator it1 = bandLineMap.find(valueImage);
                if(it1 != bandLineMap.end()) {
                    bandLineMap.erase(it1);
                }
                map<ImageView*, ImageView*>::iterator it2 = bandLineMap.find(operateSnapImage);
                if(it2 != bandLineMap.end()) {
                    bandLineMap.erase(it2);
                }

                Vector<ImageView*> tempImageList;
                ImageView* prevImage = keyImage;
                ImageView* nextImage = nullptr;

                for(int i = 1; i < hangPointList.size(); i++) {
                    ImageView *newImage = createSnapImage("select.png", hangPointList.at(i));
                    this->addChild(newImage);
                    snapImageList.pushBack(newImage);

                    tempImageList.pushBack(newImage);
                }
                for(int j = 1; j < hangPointList.size(); j++) {

                    ImageView* newImage = tempImageList.at(j - 1);

                    if(j == hangPointList.size() - 1) {
                        nextImage = valueImage;
                    } else {
                        nextImage = tempImageList.at(j);
                    }

                    bandLineMap.insert(make_pair(newImage, prevImage));
                    bandLineMap.insert(make_pair(nextImage, newImage));
                }
            }

            update(hangPointList.at(0));
        }
    }
}

void BandView::update(const Vec2 &curPosition) {

    //更新位置
    if(operateSnapImage) {
        operateSnapImage->setPosition(curPosition);
    }
    
    //更新皮筋
    lineDraw->clear();
    if(bandLineMap.size() > 0) {
        map<ImageView*, ImageView*>::iterator it;
        for(it = bandLineMap.begin(); it != bandLineMap.end(); it++) {
            ImageView* image1 = it->first;
            ImageView* image2 = it->second;
            lineDraw->drawSegment(image1->getPosition(), image2->getPosition(), BAND_WIDTH, lineColor);
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

ImageView* BandView::checkIsOnBand(const cocos2d::Vec2 &point) {

    map<ImageView*, ImageView*>::iterator it;
    for(it = bandLineMap.begin(); it != bandLineMap.end(); it++) {
        ImageView* image1 = it->first;
        ImageView* image2 = it->second;

        if(checkIsOnSegment(image1->getPosition(), image2->getPosition(), point)) {
            return image1;
        }
    }

    return nullptr;
}

bool BandView::checkIsOnSegment(const Vec2 &point1, const Vec2 &point2, const Vec2 &point) {
    float AC = point.distance(point1);
    float BC = point.distance(point2);
    float AB = point1.distance(point2);

    if(fabs(AC + BC - AB) <= 1.0f) {
        return true;
    }

    return false;
}

bool BandView::checkIsInTrig(const cocos2d::Vec2 &point1, const cocos2d::Vec2 &point2, const cocos2d::Vec2 &point3,
                   const cocos2d::Vec2 &point) {

    float signOfTrig = (point2.x - point1.x)*(point3.y - point1.y) - (point2.y - point1.y)*(point3.x - point1.x);
    float signOfAB = (point2.x - point1.x)*(point.y - point1.y) - (point2.y - point1.y)*(point.x - point1.x);
    float signOfCA = (point1.x - point3.x)*(point.y - point3.y) - (point1.y - point3.y)*(point.x - point3.x);
    float signOfBC = (point3.x - point2.x)*(point.y - point3.y) - (point3.y - point2.y)*(point.x - point3.x);

    bool d1 = (signOfAB * signOfTrig > 0);
    bool d2 = (signOfCA * signOfTrig > 0);
    bool d3 = (signOfBC * signOfTrig > 0);

    return d1 && d2 && d3;
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

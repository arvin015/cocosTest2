//
//  PolygonView.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/12/13.
//

#include "PolygonView.h"

USING_NS_CC;
using namespace ui;
using namespace std;

PolygonView::PolygonView()
: eventListener(nullptr)
, polygonDrawNode(nullptr)
, lineDrawNode(nullptr)
, isTouchEnabled(true)
, isSelect(false)
, operateType(CUT)
, cutcallback(nullptr) {

}

PolygonView::~PolygonView() {

}

void PolygonView::onEnter() {
    Node::onEnter();

    eventListener = EventListenerTouchOneByOne::create();
    eventListener->onTouchBegan = CC_CALLBACK_2(PolygonView::onTouchBegan, this);
    eventListener->onTouchMoved = CC_CALLBACK_2(PolygonView::onTouchMoved, this);
    eventListener->onTouchEnded = CC_CALLBACK_2(PolygonView::onTouchEnded, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, this);
}

void PolygonView::onExit() {
    Node::onExit();
}

bool PolygonView::onTouchBegan(Touch* touch, Event* event) {
    if (!isTouchEnabled) {
        return false;
    }

    if (operateType == CUT) { //切割
        firstPosition = touch->getLocation();
    } else if (operateType == MOVE) { //移位
        if (checkIsInPolygon(touch->getLocation())) {
            isSelect = true;
            eventListener->setSwallowTouches(true);
        }
    }

    return true;
}

void PolygonView::onTouchMoved(Touch* touch, Event* event) {

    if (operateType == CUT) { //切割
        curPosition = touch->getLocation();
        onDrawLine();
    } else if (operateType == MOVE) { //移位
        if (isSelect) {
            polygonDrawNode->setPosition(polygonDrawNode->getPosition() + touch->getDelta());
        }
    }
}

void PolygonView::onTouchEnded(Touch* touch, Event* event) {
    if (operateType == CUT) { //切割
        lineDrawNode->clear();

        map<PolygonPoint, PolygonPoint> crossMap = checkIntersectPoints(firstPosition, curPosition);
        if (crossMap.size() == 2) { //有两个交点
            addCrossPoints(crossMap);
            cutPolygon();
        }

    } else if (operateType == MOVE) { //移位
        isSelect = false;
        eventListener->setSwallowTouches(false);
    }
}

bool PolygonView::init() {
    if (!Node::init()) {
        return false;
    }

    setContentSize(Size(1024, 768));

    polygonDrawNode = DrawNode::create();
    this->addChild(polygonDrawNode);

    lineDrawNode = DrawNode::create();
    this->addChild(lineDrawNode);

    return true;
}

void PolygonView::setValue(vector<PolygonPoint> pointList) {

    for (PolygonPoint &point : pointList) {
        point.isBreakPoint = false;
    }

    this->pointList = pointList;

    for (int i = 0; i < pointList.size() - 1; i++) {
        segmentList.push_back(PolygonSegment(pointList[i], pointList[i+1], polygonDrawNode));
    }
    segmentList.push_back(PolygonSegment(pointList[pointList.size() - 1], pointList[0], polygonDrawNode));

    onDrawPolygon();
}

void PolygonView::onDrawPolygon() {

    polygonDrawNode->clear();

    Vec2* points = new Vec2[pointList.size()];
    for (int i = 0; i < pointList.size(); i++) {
        points[i] = Vec2(pointList[i].x, pointList[i].y);
    }

    polygonDrawNode->drawPolygon(points, (int)pointList.size(), Color4F::YELLOW, 2, Color4F::BLACK);
    delete [] points;
}

void PolygonView::onDrawLine() {
    lineDrawNode->clear();
    lineDrawNode->drawSegment(firstPosition, curPosition, 1, Color4F::BLACK);
}

bool PolygonView::checkIsInPolygon(const Vec2 &point) {
    int intersect = 0;
    for (PolygonSegment segment : segmentList) {
        PolygonPoint p;
        float x, y;
        if (segment.checkIsIntersect(0, point.y, point.x, point.y, p, x, y)) intersect++;
    }

    if (intersect == 1) {
        return true;
    } else {
        return false;
    }
}

map<PolygonPoint, PolygonPoint> PolygonView::checkIntersectPoints(const Vec2 &p1, const Vec2 &p2) {

    map<PolygonPoint, PolygonPoint> pointMap;

    for (PolygonSegment segment : segmentList) {
        PolygonPoint p;
        float x, y;
        if (segment.checkIsIntersect(p1.x, p1.y, p2.x, p2.y, p, x, y)) {
            Vec2 v = convertToNodeSpace(Vec2(x, y));
            PolygonPoint crossPoint = PolygonPoint(v.x, v.y);
            crossPoint.isBreakPoint = true;
            pointMap.insert(make_pair(p, crossPoint));
        }
    }

    return pointMap;
}

void PolygonView::addCrossPoints(map<PolygonPoint, PolygonPoint> pointMap) {
    map<PolygonPoint, PolygonPoint>::iterator it;
    for (it = pointMap.begin(); it != pointMap.end(); it++) {
        PolygonPoint p = it->first;
        PolygonPoint crossPoint = it->second;
        
        int index = -1;
        for (int i = 0; i < pointList.size(); i++) {
            if (p == polygonDrawNode->convertToWorldSpace(pointList[i])) {
                index = i;
                break;
            }
        }

        if (index != -1) {
            vector<PolygonPoint>::iterator it1 = pointList.begin();
            it1 += index + 1;
            pointList.insert(it1, crossPoint);
        }
    }
}

void PolygonView::cutPolygon() {

    for (PolygonPoint &point : pointList) {
        Vec2 p = polygonDrawNode->convertToWorldSpace(point);
        point.x = p.x;
        point.y = p.y;
    }

    vector<PolygonPoint> pointList1;

    bool isCollecting = false;

    for (int i = 0; i < pointList.size(); i++) {

        PolygonPoint point = pointList[i];

        if (!isCollecting) {
            if (point.isBreakPoint) {
                pointList1.push_back(point);
                isCollecting = true;
            }
        } else {
            pointList1.push_back(point);
            if (point.isBreakPoint) {
                break;
            } else {
                pointList.erase(pointList.begin() + i);
                i--;
            }
        }
    }

    if (cutcallback) {
        this->cutcallback(pointList1);
        this->cutcallback(pointList);
    }

    removeFromParent();
}



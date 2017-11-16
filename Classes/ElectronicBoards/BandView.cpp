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
    operateSnapView = nullptr;
    touchEndedCallback = nullptr;
    firstSnapView = nullptr;
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
    
    SnapView* startSnapView = createSnapView("select.png", startPoint);
    this->addChild(startSnapView, 2);

    SnapView* endSnapView = createSnapView("select.png", endPoint);
    this->addChild(endSnapView, 2);

    firstSnapView = startSnapView;
    firstSnapView->nextSnapView = endSnapView;
}

bool BandView::onTouchBegan(Touch* touch, Event* event) {
    if (!enableTouch) {
        return false;
    }

    //判断touch点是否在橡皮筋上
    SnapView* snapView = checkIsOnBand(touch->getLocation());
    if (snapView) { //touch在橡皮筋上
        
        bringToFront();
        eventListener->setSwallowTouches(true);

        SnapView* newSnapView = createSnapView("select.png", touch->getLocation());
        this->addChild(newSnapView);

        operateSnapView = newSnapView;

        newSnapView->preSnapView = snapView;
        newSnapView->nextSnapView = snapView->nextSnapView;
        snapView->nextSnapView->preSnapView = newSnapView;
        snapView->nextSnapView = newSnapView;
        
        if (newSnapView->nextSnapView->nextSnapView == nullptr) { //线段时，要形成环路
            newSnapView->nextSnapView->nextSnapView = snapView;
        }
    }

    return true;
}

void BandView::onTouchMoved(Touch* touch, Event* event) {
    if(operateSnapView) {
        operateSnapView->setPosition(touch->getLocation());
        updateBands();
    }
}

void BandView::onTouchEnded(Touch* touch, Event* event) {
    
    if(operateSnapView) {
        float x = touch->getLocation().x;
        float y = touch->getLocation().y;

        trigPoints.clear();
        trigPoints.push_back(Vec2(x, y));
        trigPoints.push_back(operateSnapView->preSnapView->getPosition());
        trigPoints.push_back(operateSnapView->nextSnapView->getPosition());

        if(touchEndedCallback) {
            this->touchEndedCallback(this, x, y, true);
        }
    }

    operateSnapView = nullptr;

    eventListener->setSwallowTouches(false);
}

void BandView::checkBandIsHanged(const vector<Vec2> &pointList) {

    operateSnapView->removeFromParent();
    
    //拉拽前是否线段
    bool isSegment = false;
    if (operateSnapView->preSnapView == operateSnapView->nextSnapView->nextSnapView) {
        isSegment = true;
    }

    //去掉拖拽点依赖
    operateSnapView->preSnapView->nextSnapView = operateSnapView->nextSnapView;
    operateSnapView->nextSnapView->preSnapView = operateSnapView->preSnapView;

    vector<P> hangPointList = checkBandForHangIsOk(pointList);
    
    if(hangPointList.size() > 0) { //有挂点

        SnapView* operatePreSnapView = operateSnapView->preSnapView;
        SnapView* operateNextSnapView = operateSnapView->nextSnapView;

        for (int i = 0; i < hangPointList.size(); i++) {
            SnapView* newSnapView = createSnapView("select.png", Vec2(hangPointList.at(i).point));
            this->addChild(newSnapView);

            newSnapView->preSnapView = operatePreSnapView;
            newSnapView->nextSnapView = operateNextSnapView;
            operatePreSnapView->nextSnapView = newSnapView;
            operateNextSnapView->preSnapView = newSnapView;
            
            if (isSegment) { //一开始是线段
                operateNextSnapView->nextSnapView = operatePreSnapView;
                isSegment = false;
            }
            
            operatePreSnapView = newSnapView;
            operateNextSnapView = newSnapView->nextSnapView;
        }

    } else { //没有挂点
        if (isSegment) {
            operateSnapView->preSnapView->preSnapView = nullptr;
            operateSnapView->nextSnapView->preSnapView = nullptr;
            operateSnapView->nextSnapView->nextSnapView = nullptr;
        }
        
        if (operateSnapView == firstSnapView) {
            firstSnapView = operateSnapView->nextSnapView;
        }
    }
    
    updateBands();
}

bool compa(const P &p1, const P &p2) {
    return p1.dis < p2.dis;
}

vector<P> BandView::checkBandForHangIsOk(const vector<Vec2> &pointList) {
    
    vector<P> resultPList;
    if (pointList.size() == 0) {
        return resultPList;
    } else if (pointList.size() <= 1) {
        resultPList.push_back(P(pointList.at(0), 0));
    } else {
        map<int, Vec2> distanceList;
        
        for(Vec2 point : pointList) {
            int dis = getPoint2LineDistance(operateSnapView->preSnapView->getPosition(), operateSnapView->nextSnapView->getPosition(), point);
            distanceList.insert(make_pair(dis, point));
        }
        
        float max = 0;
        map<int, Vec2>::iterator it;
        for(it = distanceList.begin(); it != distanceList.end(); it++) {
            int dis = it->first;
            if(dis > max) {
                max = dis;
            }
        }

        Vec2 targetP = operateSnapView->preSnapView->getPosition();

        Vec2 p = distanceList.at(max);
        resultPList.push_back(P(p, p.distance(targetP))); //加入最远的那个点
        
        for(Vec2 point : pointList) {
            
            if(point != p) {
                if(!checkPointIsInTrig1(p, operateSnapView->preSnapView->getPosition(), operateSnapView->nextSnapView->getPosition(), point)) { //不在最远点组成的三角形内需加入
                    resultPList.push_back(P(point, point.distance(targetP)));
                }
            }
        }
    }
    
    //根据靠近prePoint点排序
    if (resultPList.size() > 1) {
        sort(resultPList.begin(), resultPList.end(), compa);
    }
    
    return resultPList;
}

void BandView::updateAfterTouchEnd(const Vec2 &point) {
    if (operateSnapView) {
        operateSnapView->setPosition(point);
    }
    updateBands();
}

void BandView::updateBands() {
    
    lineDraw->clear();

    SnapView* curSnapView = firstSnapView;

    do {

        lineDraw->drawSegment(curSnapView->getPosition(), curSnapView->nextSnapView->getPosition(), BAND_WIDTH, lineColor);
        curSnapView = curSnapView->nextSnapView;

    } while (curSnapView->nextSnapView != nullptr && curSnapView != firstSnapView);
}

SnapView* BandView::checkIsOnBand(const cocos2d::Vec2 &point) {

    SnapView* curSnapView = firstSnapView;

    do {

        if (checkIsOnSegment(curSnapView->getPosition(), curSnapView->nextSnapView->getPosition(), point)) {
            return curSnapView;
        }

        curSnapView = curSnapView->nextSnapView;

    } while (curSnapView->nextSnapView != nullptr && curSnapView != firstSnapView);

    return nullptr;
}

bool BandView::checkIsOnSegment(const Vec2 &point1, const Vec2 &point2, const Vec2 &point) {
    float AC = point.distance(point1);
    float BC = point.distance(point2);
    float AB = point1.distance(point2);

    if(fabs(AC + BC - AB) <= 4.0f) {
        return true;
    }

    return false;
}

bool BandView::checkIsInTrig(const Vec2 &point) {

    Vec2 point1 = trigPoints.at(0);
    Vec2 point2 = trigPoints.at(1);
    Vec2 point3 = trigPoints.at(2);
    
    return checkPointIsInTrig(point1, point2, point3, point);
}

bool BandView::checkPointIsInTrig(const Vec2 &point1, const Vec2 &point2, const Vec2 &point3, const Vec2 &point) {
    
    float signOfTrig = (point2.x - point1.x) * (point3.y - point1.y) - (point2.y - point1.y) * (point3.x - point1.x);
    float signOfAB = (point2.x - point1.x) * (point.y - point1.y) - (point2.y - point1.y) * (point.x - point1.x);
    float signOfCA = (point1.x - point3.x) * (point.y - point3.y) - (point1.y - point3.y) * (point.x - point3.x);
    float signOfBC = (point3.x - point2.x) * (point.y - point3.y) - (point3.y - point2.y) * (point.x - point3.x);
    
    bool d1 = (signOfAB * signOfTrig > 0);
    bool d2 = (signOfCA * signOfTrig > 0);
    bool d3 = (signOfBC * signOfTrig > 0);
    
    return d1 && d2 && d3;
}

bool BandView::checkPointIsInTrig1(const Vec2 &point1, const Vec2 &point2, const Vec2 &point3, const Vec2 &point) {
    
    float signOfTrig = (point2.x - point1.x) * (point3.y - point1.y) - (point2.y - point1.y) * (point3.x - point1.x);
    float signOfAB = (point2.x - point1.x) * (point.y - point1.y) - (point2.y - point1.y) * (point.x - point1.x);
    float signOfCA = (point1.x - point3.x) * (point.y - point3.y) - (point1.y - point3.y) * (point.x - point3.x);
    float signOfBC = (point3.x - point2.x) * (point.y - point3.y) - (point3.y - point2.y) * (point.x - point3.x);
    
    bool d1 = (signOfAB * signOfTrig >= 0);
    bool d2 = (signOfCA * signOfTrig >= 0);
    bool d3 = (signOfBC * signOfTrig >= 0);
    
    return d1 && d2 && d3;
}

float BandView::getPoint2LineDistance(const Vec2 &point1, const Vec2 &point2, const Vec2 &point) {
    
    float xx = point.x;
    float yy = point.y;
    float x1 = point1.x;
    float y1 = point1.y;
    float x2 = point2.x;
    float y2 = point2.y;
    
    double a, b, c, ang1, ang2, ang, m;
    double result = 0;
    
    //分别计算三条边的长度
    a = sqrt((x1 - xx) * (x1 - xx) + (y1 - yy) * (y1 - yy));
    
    if (a == 0)
        return -1;
    b = sqrt((x2 - xx) * (x2 - xx) + (y2 - yy) * (y2 - yy));
    if (b == 0)
        return -1;
    c = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
    //如果线段是一个点则退出函数并返回距离
    if (c == 0)
    {
        result = a;
        return result;
    }
    //如果点(xx,yy到点x1,y1)这条边短
    if (a < b)
    {
        //如果直线段AB是水平线。得到直线段AB的弧度
        if (y1 == y2)
        {
            if (x1 < x2)
                ang1 = 0;
            else
                ang1 = M_PI;
        }
        else
        {
            m = (x2 - x1) / c;
            if (m - 1 > 0.00001)
                m = 1;
            ang1 = acos(m);
            if (y1 >y2)
                ang1 = M_PI*2  - ang1;//直线(x1,y1)-(x2,y2)与折X轴正向夹角的弧度
        }
        m = (xx - x1) / a;
        if (m - 1 > 0.00001)
            m = 1;
        ang2 = acos(m);
        if (y1 > yy)
            ang2 = M_PI * 2 - ang2;//直线(x1,y1)-(xx,yy)与折X轴正向夹角的弧度
        
        ang = ang2 - ang1;
        if (ang < 0) ang = -ang;
        
        if (ang > M_PI) ang = M_PI * 2 - ang;
        //如果是钝角则直接返回距离
        if (ang > M_PI / 2)
            return a;
        else
            return a * sin(ang);
    }
    else//如果(xx,yy)到点(x2,y2)这条边较短
    {
        //如果两个点的纵坐标相同，则直接得到直线斜率的弧度
        if (y1 == y2)
            if (x1 < x2)
                ang1 = M_PI;
            else
                ang1 = 0;
            else
            {
                m = (x1 - x2) / c;
                if (m - 1 > 0.00001)
                    m = 1;
                ang1 = acos(m);
                if (y2 > y1)
                    ang1 = M_PI * 2 - ang1;
            }
        m = (xx - x2) / b;
        if (m - 1 > 0.00001)
            m = 1;
        ang2 = acos(m);//直线(x2-x1)-(xx,yy)斜率的弧度
        if (y2 > yy)
            ang2 = M_PI * 2 - ang2;
        ang = ang2 - ang1;
        if (ang < 0) ang = -ang;
        if (ang > M_PI) ang = M_PI * 2 - ang;//交角的大小
        //如果是对角则直接返回距离
        if (ang > M_PI / 2)
            return b;
        else
            return b * sin(ang);//如果是锐角，返回计算得到的距离
    }
}

SnapView* BandView::createSnapView(const string &imageName, const Vec2 &position) {
    SnapView* snapView = SnapView::create(imageName);
    snapView->setPosition(position);
    snapView->setOnTouchDown([this, snapView]() {
        this->operateSnapView = snapView;
        bringToFront();
    });
    snapView->setOnTouchMove([this]() {
        updateBands();
    });
    snapView->setOnTouchEnd([this, snapView](float x, float y) {
        if (touchEndedCallback) {
            bool isHangBand = false;
            if (snapView->nextSnapView != nullptr && snapView->preSnapView != nullptr) {
                isHangBand = true;
            }
            if (isHangBand) {
                trigPoints.clear();
                trigPoints.push_back(Vec2(x, y));
                trigPoints.push_back(operateSnapView->preSnapView->getPosition());
                trigPoints.push_back(operateSnapView->nextSnapView->getPosition());
            }
            this->touchEndedCallback(this, x, y, isHangBand);
        }
        this->operateSnapView = nullptr;
    });
    return snapView;
}

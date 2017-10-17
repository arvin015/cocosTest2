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
    valueImage = nullptr;
    keyImage = nullptr;
    touchEndedCallback = nullptr;
    isSegment = true;
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
        
        isHangBand = !isSegment; //是否挂橡皮筋
        
        if(isHangBand) { //挂橡皮筋、需确定keyImage、valueImage
            keyImage = bandLineMap.at(operateSnapImage);
            
            map<ImageView*, ImageView*>::iterator it;
            for(it = bandLineMap.begin(); it != bandLineMap.end(); it++) {
                ImageView* value = it->second;
                if(operateSnapImage == value) {
                    valueImage = it->first;
                    break;
                }
            }
        }
        
    } else { //touch不在钉子上，判断是否在橡皮筋上
        keyImage = checkIsOnBand(touch->getLocation());
        if(keyImage) { //touch在橡皮筋上
            
            valueImage = bandLineMap.at(keyImage);
            
            ImageView* newImage = createSnapImage("select.png", touch->getLocation());
            this->addChild(newImage);
            snapImageList.pushBack(newImage);

            operateSnapImage = newImage;
            
            //先删线段
            map<ImageView*, ImageView*>::iterator it = bandLineMap.find(keyImage);
            if(it != bandLineMap.end()) {
                bandLineMap.erase(it);
            }
            
            bandLineMap.insert(make_pair(keyImage, newImage));
            bandLineMap.insert(make_pair(newImage, valueImage));
            
            if(isSegment) { //一条线段时，先加一条线段
                bandLineMap.insert(make_pair(valueImage, keyImage));
                
                ImageView* temp = keyImage;
                valueImage = temp;
                keyImage = valueImage;
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
        
        if(isHangBand) {
            trigPoints.clear();
            trigPoints.push_back(operateSnapImage->getPosition());
            trigPoints.push_back(keyImage->getPosition());
            trigPoints.push_back(valueImage->getPosition());
        }
        
        if(touchEndedCallback) {
            this->touchEndedCallback(this, x, y, isHangBand);
        }
    }
    
    operateSnapImage = nullptr;
    keyImage = nullptr;
    valueImage = nullptr;

    eventListener->setSwallowTouches(false);
}

void BandView::checkBandIsHanged(const vector<Vec2> &pointList) {
    
    operateSnapImage->removeFromParent();
    
    //去掉与当前操作钉子相关联的两条线段
    map<ImageView*, ImageView*>::iterator it1 = bandLineMap.find(valueImage);
    if(it1 != bandLineMap.end()) {
        bandLineMap.erase(it1);
    }
    map<ImageView*, ImageView*>::iterator it2 = bandLineMap.find(operateSnapImage);
    if(it2 != bandLineMap.end()) {
        bandLineMap.erase(it2);
    }
    
    vector<Vec2> hangPointList = checkBandForHangIsOk(pointList);
    
    if(hangPointList.size() > 0) {

        //添加钉子
        Vector<ImageView*> tempImageList;
        
        for(int i = 0; i < hangPointList.size(); i++) {
            ImageView* newImage = createSnapImage("select.png", hangPointList.at(i));
            tempImageList.pushBack(newImage);
        }
        
        //添加橡皮筋
        ImageView* prevImage = isSegment ? valueImage : keyImage;
        ImageView* nextImage = nullptr;
        
        for(int j = 0; j < hangPointList.size(); j++) {
            
            ImageView* newImage = tempImageList.at(j);
            
            if(j == hangPointList.size() - 1) {
                nextImage = isSegment ? keyImage : valueImage;
            } else {
                nextImage = tempImageList.at(j + 1);
            }
            
            bandLineMap.insert(make_pair(newImage, prevImage));
            bandLineMap.insert(make_pair(nextImage, newImage));
        }
    }
    
    updateBands();
    
    isSegment = false;
}

vector<Vec2> BandView::checkBandForHangIsOk(const vector<Vec2> &pointList) {
    
    vector<Vec2> resultPointList;
    
    if(pointList.size() <= 1) {
        resultPointList = pointList;
    } else {
        map<int, Vec2> distanceList;
        
        for(Vec2 point : pointList) {
            int dis = getPoint2LineDistance(keyImage->getPosition(), valueImage->getPosition(), point);
            log("-------- dis = %d", dis);
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
        
        Vec2 p = distanceList.at(max);
        
        for(Vec2 point : pointList) {
            
            if(point != p) {
                if(checkPointIsInTrig(p, keyImage->getPosition(), valueImage->getPosition(), point)) {
                    resultPointList.push_back(point);
                }
            }
        }
    }
    
    return resultPointList;
}

void BandView::update(const Vec2 &curPosition) {

    //更新位置
    if(operateSnapImage) {
        operateSnapImage->setPosition(curPosition);
    }
    
    //更新橡皮筋
    updateBands();
}

void BandView::updateBands() {
    
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
    
    log("---------- point1.x = %f, point1.y = %f; point2.x = %f, point2.y = %f; point3.x = %f, point3.y = %f; point.x = %f, point.y = %f", point1.x, point1.y, point2.x, point2.y, point3.x, point3.y, point.x, point.y);
    log("---------- d1 = %d, d2 = %d, d2 = %d", d1, d2, d3);
    
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

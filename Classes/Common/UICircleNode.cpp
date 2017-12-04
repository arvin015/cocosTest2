//
// Created by Administrator on 2017/12/2.
//

#include "UICircleNode.h"

USING_NS_CC;
using namespace ui;
using namespace std;

//集合转数组
template <class T>
void vectorToArray(const vector<T> &pointList, T* points) {
    for (int i = 0; i < pointList.size(); i++) {
        points[i] = pointList.at(i);
    }
}

inline float getSlope(float angle) {
    return tan(angle * 3.14 / 180);
}

inline void getCrossPointX(float slope, const Vec2 &point, float &x, float y) {
    x = (y - point.y) * 1.0 / slope + point.x;
}

inline void getCrossPointY(float slope, const Vec2 &point, float x, float &y) {
    y = slope * (x - point.x) + point.y;
}

CircleNode::CircleNode()
: contentNode(nullptr)
, borderNode(nullptr)
, borderColor(Color4F(67.0 / 255, 114.0 / 255, 156.0 / 255, 1))
, divideLineColor(Color3B(131.0, 179.0, 223.0))
, resultColor(Color4F::YELLOW){

}

CircleNode::~CircleNode() {

}

void CircleNode::onEnter() {
    Node::onEnter();

    initPeaks(); //初始化顶点集合

    //创建裁剪Node
    ClippingNode* clippingNode = ClippingNode::create();
    clippingNode->setContentSize(getContentSize());
    clippingNode->setInverted(false);
    this->addChild(clippingNode);

    //创建裁剪Stencil
    DrawNode* stencil = DrawNode::create();
    stencil->drawSolidCircle(getContentSize() / 2, getContentSize().width / 2, 360, 100, 1, 1, Color4F::WHITE);
    clippingNode->setStencil(stencil);

    //创建裁剪内容
    contentNode = Node::create();
    contentNode->setContentSize(getContentSize());
    clippingNode->addChild(contentNode);

    //绘制主要内容
    onDraw();

    //绘制圆圈
    borderNode = DrawNode::create(6);
    borderNode->drawCircle(getContentSize() / 2, getContentSize().width / 2, 4, 120, false, borderColor);
    this->addChild(borderNode);
}

void CircleNode::onExit() {
    Node::onExit();
}

bool CircleNode::init() {
    if (!Node::init()) {
        return false;
    }
    return true;
}

void CircleNode::setValue(int divideNum, int resultNum) {
    this->divideNum = divideNum;
    this->resultNum = resultNum;

    this->perAngle = 360.0 / divideNum;
    resultAngle = resultNum * perAngle;

    if (contentNode) {
        onDraw();
    }
}

void CircleNode::initPeaks() {
    peakList.clear();

    float width = getContentSize().width;
    float height = getContentSize().height;

    //9个点
    peakList.push_back(getContentSize() / 2); //中心点
    peakList.push_back(Vec2(width / 2, height));
    peakList.push_back(Vec2(width, height));
    peakList.push_back(Vec2(width, height / 2));
    peakList.push_back(Vec2(width, 0));
    peakList.push_back(Vec2(width / 2, 0));
    peakList.push_back(Vec2(0, 0));
    peakList.push_back(Vec2(0, height / 2));
    peakList.push_back(Vec2(0, height));
}

void CircleNode::onDraw() {

    contentNode->removeAllChildren();

    //绘制结果份数
    vector<Vec2> pointList;
    getPolyPoints(pointList);
    Vec2* points = new Vec2[pointList.size()];
    vectorToArray(pointList, points);
    DrawNode* resultDraw = DrawNode::create();
    resultDraw->setContentSize(getContentSize());
    resultDraw->drawSolidPoly(points, (int)pointList.size(), resultColor);
    contentNode->addChild(resultDraw);
    delete [] points;

    //创建内容分割线
    float angle = 0; //旋转角度
    for (int i = 0; i < divideNum; i++) {
        Layout* lineLayout = Layout::create();
        lineLayout->setContentSize(Size(2, getContentSize().width / 2));
        lineLayout->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        lineLayout->setPosition(getContentSize() / 2);
        lineLayout->setRotation(angle);
        lineLayout->setBackGroundColor(divideLineColor);
        lineLayout->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
        contentNode->addChild(lineLayout);

        angle += perAngle;
    }
}

void CircleNode::getPolyPoints(vector<Vec2> &pointList) {
    pointList.push_back(peakList[0]);
    pointList.push_back(peakList[1]);

    float width = getContentSize().width;
    float height = getContentSize().height;

    float x = 0, y = 0;
    float ang = (int)resultAngle % 180;
    float k = 0;
    if (ang != 0 && ang != 90 ) {
        k = getSlope(ang > 90 ? 180 - (ang - 90) : 90 - ang);
    }
    Vec2 p = peakList[0];

    if (resultAngle < 45) {
        y = height;
        getCrossPointX(k, p, x, y);
        pointList.push_back(Vec2(x, y));
    } else if (resultAngle == 45) {
        pointList.push_back(peakList[2]);
    } else if (resultAngle > 45 && resultAngle < 90) {
        pointList.push_back(peakList[2]);

        x = width;
        getCrossPointY(k, p, x, y);
        pointList.push_back(Vec2(x, y));
    } else if (resultAngle == 90) {
        pointList.push_back(peakList[2]);
        pointList.push_back(peakList[3]);
    } else if (resultAngle > 90 && resultAngle < 135) {
        pointList.push_back(peakList[2]);

        x = width;
        getCrossPointY(k, p, x, y);
        pointList.push_back(Vec2(x, y));
    } else if (resultAngle == 135) {
        pointList.push_back(peakList[2]);
        pointList.push_back(peakList[4]);
    } else if (resultAngle > 135 && resultAngle < 180) {
        pointList.push_back(peakList[2]);
        pointList.push_back(peakList[4]);

        y = 0;
        getCrossPointX(k, p, x, y);
        pointList.push_back(Vec2(x, y));
    } else if (resultAngle == 180) {
        pointList.push_back(peakList[2]);
        pointList.push_back(peakList[4]);
        pointList.push_back(peakList[5]);
    } else if (resultAngle > 180 && resultAngle < 225) {
        pointList.push_back(peakList[2]);
        pointList.push_back(peakList[4]);

        y = 0;
        getCrossPointX(k, p, x, y);
        pointList.push_back(Vec2(x, y));
    } else if (resultAngle == 225) {
        pointList.push_back(peakList[2]);
        pointList.push_back(peakList[4]);
        pointList.push_back(peakList[6]);
    } else if (resultAngle > 225 && resultAngle < 270) {
        pointList.push_back(peakList[2]);
        pointList.push_back(peakList[4]);
        pointList.push_back(peakList[6]);

        x = 0;
        getCrossPointY(k, p, x, y);
        pointList.push_back(Vec2(x, y));
    } else if (resultAngle == 270) {
        pointList.push_back(peakList[2]);
        pointList.push_back(peakList[4]);
        pointList.push_back(peakList[6]);
        pointList.push_back(peakList[7]);
    } else if (resultAngle > 270 && resultAngle < 315) {
        pointList.push_back(peakList[2]);
        pointList.push_back(peakList[4]);
        pointList.push_back(peakList[6]);

        x = 0;
        getCrossPointY(k, p, x, y);
        pointList.push_back(Vec2(x, y));
    } else if (resultAngle == 315) {
        pointList.push_back(peakList[2]);
        pointList.push_back(peakList[4]);
        pointList.push_back(peakList[6]);
        pointList.push_back(peakList[8]);
    } else if (resultAngle > 315 && resultAngle < 360) {
        pointList.push_back(peakList[2]);
        pointList.push_back(peakList[4]);
        pointList.push_back(peakList[6]);
        pointList.push_back(peakList[8]);

        y = height;
        getCrossPointX(k, p, x, y);
        pointList.push_back(Vec2(x, y));
    } else if (resultAngle == 360) {
        pointList.clear();
        pointList.push_back(peakList[2]);
        pointList.push_back(peakList[4]);
        pointList.push_back(peakList[6]);
        pointList.push_back(peakList[8]);
    }
}

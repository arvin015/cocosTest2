//
// Created by Administrator on 2017/12/2.
//

#include "UICircleNode.h"

USING_NS_CC;
using namespace ui;
using namespace std;

CircleNode::CircleNode()
: contentNode(nullptr) {

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

    //创建圆圈
    ImageView* circleImageView = ImageView::create(circleImage);
    circleImageView->setPosition(getContentSize() / 2);
    this->addChild(circleImageView);
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

void CircleNode::setValue(int divideNum, int resultNum, const string &circleImage) {
    this->divideNum = divideNum;
    this->resultNum = resultNum;
    this->circleImage = circleImage;

    this->perAngle = 360.0 / divideNum;
    resultAngle = resultNum * perAngle;

    log("------------ divideNum = %d, resultNum = %d, resultAngle = %f", divideNum, resultNum, resultAngle);

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
    Vec2* points = new Vec2;
    vectorToArray(pointList, points);
    DrawNode* resultDraw = DrawNode::create();
    resultDraw->setContentSize(getContentSize());
    resultDraw->drawSolidPoly(points, pointList.size(), Color4F::RED);
    contentNode->addChild(resultDraw);
//    delete points;

    //创建内容分割线
    float angle = 0; //旋转角度
    for (int i = 0; i < divideNum; i++) {
        Layout* lineLayout = Layout::create();
        lineLayout->setContentSize(Size(2, getContentSize().width / 2));
        lineLayout->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        lineLayout->setPosition(getContentSize() / 2);
        lineLayout->setRotation(angle);
        lineLayout->setBackGroundColor(Color3B::BLACK);
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

    log("---------------- ang = %f, k = %f", ang, k);

    if (resultAngle < 45) {
        y = height;
        getCrossPoint(k, p, x, y);
        pointList.push_back(Vec2(x, y));
    } else if (resultAngle == 45) {
        pointList.push_back(peakList[2]);
    } else if (resultAngle > 45 && resultAngle < 90) {
        pointList.push_back(peakList[2]);

        x = width;
        getCrossPoint(k, p, x, y);
        pointList.push_back(Vec2(x, y));

        log("------------------45-90 x = %f, y = %f", x, y);

    } else if (resultAngle == 90) {
        pointList.push_back(peakList[2]);
        pointList.push_back(peakList[3]);
    } else if (resultAngle > 90 && resultAngle < 135) {
        pointList.push_back(peakList[2]);

        x = width;
        getCrossPoint(k, p, x, y);
        pointList.push_back(Vec2(x, y));

        log("------------------90-135 x = %f, y = %f", x, y);

    } else if (resultAngle == 135) {
        pointList.push_back(peakList[2]);
        pointList.push_back(peakList[4]);
    } else if (resultAngle > 135 && resultAngle < 180) {
        pointList.push_back(peakList[2]);
        pointList.push_back(peakList[4]);

        y = 0;
        getCrossPoint1(k, p, x, y);
        pointList.push_back(Vec2(x, y));

        log("------------------135-180 x = %f, y = %f", x, y);

    } else if (resultAngle == 180) {
        pointList.push_back(peakList[2]);
        pointList.push_back(peakList[4]);
        pointList.push_back(peakList[5]);
    } else if (resultAngle > 180 && resultAngle < 225) {
        pointList.push_back(peakList[2]);
        pointList.push_back(peakList[4]);

        y = 0;
        getCrossPoint1(k, p, x, y);
        pointList.push_back(Vec2(x, y));

        log("------------------180-225 x = %f, y = %f", x, y);

    } else if (resultAngle == 225) {
        pointList.push_back(peakList[2]);
        pointList.push_back(peakList[4]);
        pointList.push_back(peakList[6]);
    } else if (resultAngle > 225 && resultAngle < 270) {
        pointList.push_back(peakList[2]);
        pointList.push_back(peakList[4]);
        pointList.push_back(peakList[6]);

        x = 0;
        getCrossPoint(k, p, x, y);
        pointList.push_back(Vec2(x, y));

        log("------------------225-270 x = %f, y = %f", x, y);

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
        getCrossPoint(k, p, x, y);
        pointList.push_back(Vec2(x, y));

        log("------------------270-315 x = %f, y = %f", x, y);

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
        getCrossPoint1(k, p, x, y);
        pointList.push_back(Vec2(x, y));

        log("------------------315-360 x = %f, y = %f", x, y);

    } else if (resultAngle == 360) {
        pointList.clear();
        pointList.push_back(peakList[2]);
        pointList.push_back(peakList[4]);
        pointList.push_back(peakList[6]);
        pointList.push_back(peakList[8]);
    }
}

void CircleNode::vectorToArray(const vector<Vec2> &pointList, Vec2* points) {
    for (int i = 0; i < pointList.size(); i++) {
        points[i] = pointList.at(i);
    }
}

float CircleNode::getSlope(float angle) {
    return tan(angle * 3.14 / 180);
}

void CircleNode::getCrossPoint(float slope, const Vec2 &point, float x, float &y) {
     log("slope = %f, x = %f", slope, x);
     y = slope * (x - point.x) + point.y;
}

void CircleNode::getCrossPoint1(float slope, const Vec2 &point, float &x, float y) {
    log("slope = %f, y = %f", slope, y);
    x = (y - point.y) * 1.0 / slope + point.x;
}
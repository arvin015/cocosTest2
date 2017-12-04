//
//  UISquareNode.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/12/4.
//

#include "UISquareNode.h"

USING_NS_CC;
using namespace ui;
using namespace std;

SquareNode::SquareNode()
: borderDraw(nullptr)
, lineDraw(nullptr)
, borderColor(Color4F(67.0 / 255, 114.0 / 255, 156.0 / 255, 1))
, divideLineColor(Color4F(131.0 / 255, 179.0 / 255, 223.0 / 255, 1))
, resultColor(Color4F::YELLOW){
    
}

SquareNode::~SquareNode() {
    
}

void SquareNode::onEnter() {
    Node::onEnter();

    resultDraw = DrawNode::create();
    this->addChild(resultDraw);

    lineDraw = DrawNode::create(1);
    this->addChild(lineDraw);

    borderDraw = DrawNode::create(4);
    Vec2 points[] = {
            Vec2::ZERO,
            Vec2(getContentSize().width, 0),
            Vec2(getContentSize()),
            Vec2(0, getContentSize().height)
    };
    borderDraw->drawPoly(points, 4, true, borderColor);
    this->addChild(borderDraw);

    onDraw();
}

void SquareNode::onExit() {
    Node::onExit();
}

bool SquareNode::init() {
    if (!Node::init()) {
        return false;
    }
    return true;
}

void SquareNode::setValue(int divideNum, int resultNum) {
    this->divideNum = divideNum;
    this->resultNum = resultNum;

    if (borderDraw) {
        onDraw();
    }
}

void SquareNode::onDraw() {
    lineDraw->clear();
    resultDraw->clear();

    float width = getContentSize().width;
    float perWidth = width / divideNum; //每份宽
    float resultWidth = resultNum * perWidth; //结果宽

    //绘制分割线
    for (int i = 0; i < divideNum - 1; i++) {
        float x = perWidth * (i + 1);
        lineDraw->drawSegment(Vec2(x, 1), Vec2(x, getContentSize().height - 1), 1, divideLineColor);
    }

    //绘制结果块
    Vec2 points[] = {
            Vec2::ZERO,
            Vec2(resultWidth, 0),
            Vec2(resultWidth, getContentSize().height),
            Vec2(0, getContentSize().height)
    };
    resultDraw->drawSolidPoly(points, 4, resultColor);
}

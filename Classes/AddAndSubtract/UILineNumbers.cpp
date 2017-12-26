//
//  UILineNumbers.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/12/5.
//

#include "UILineNumbers.h"

USING_NS_CC;
using namespace ui;
using namespace std;

LineNumbers::LineNumbers()
: lastSelectNumBtn(nullptr)
, normalColor(Color3B(153, 204, 255))
, selectColor(Color3B(255, 204, 204))
, enabled(true)
, drawCursor(nullptr) {

}

LineNumbers::~LineNumbers() {

}

void LineNumbers::onEnter() {
    Node::onEnter();

    blockSize = getContentSize().width / totalNum;

    setContentSize(Size(getContentSize().width, blockSize));

    for (int i = minValue; i < totalNum + 1; i++) {
        Button* btn = Button::create("black_frame.png", "black_frame.png", "black_frame.png");
        btn->setAnchorPoint(Vec2::ZERO);
        btn->setPosition(Vec2(blockSize * (i - minValue), 0));
        btn->setContentSize(Size(blockSize, blockSize));
        btn->setScale9Enabled(true);
        btn->setColor(normalColor);
        btn->setTitleFontSize(20);
        btn->setTitleColor(Color3B::BLACK);
        btn->setTitleText(Value(i).asString());
        btn->setTag(i);
        //数字按钮是否可点击
        btn->setEnabled(enabled);
        btn->setSwallowTouches(enabled);
        btn->addClickEventListener([this, btn](Ref* pSender){

            if (btn == lastSelectNumBtn) {
                return;
            }

            if (lastSelectNumBtn) {
                lastSelectNumBtn->setColor(normalColor);
            }
            btn->setColor(selectColor);

            lastSelectNumBtn = btn;

            if (clickBlockCallback) {
                this->clickBlockCallback(btn->getTag());
            }
        });
        this->addChild(btn);
    }

    //绘制边框
    DrawNode* frameDraw = DrawNode::create();
    Vec2 verts[4] = {
            Vec2::ZERO,
            Vec2(0, getContentSize().height),
            Vec2(getContentSize().width, getContentSize().height),
            Vec2(getContentSize().width, 0)
    };
    frameDraw->drawPolygon(verts, 4, Color4F(0.0f, 0.0f, 0.0f, 0.0f), 1, Color4F::BLACK);
    this->addChild(frameDraw);

    //绘制光标
    drawCursor = DrawNode::create();
    drawCursor->setContentSize(Size(blockSize, blockSize));
    drawCursor->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    Vec2 ps[4] = {
            Vec2::ZERO,
            Vec2(0, drawCursor->getContentSize().height),
            Vec2(drawCursor->getContentSize().width, drawCursor->getContentSize().height),
            Vec2(drawCursor->getContentSize().width, 0)
    };
    drawCursor->drawSolidPoly(ps, 4, Color4F(255.0 / 255, 204.0 / 255, 204.0 / 255, 0.6f));
    drawCursor->setVisible(false);
    this->addChild(drawCursor);
}

void LineNumbers::onExit() {
    Node::onExit();
}

bool LineNumbers::init() {
    if (!Node::init()) {
        return false;
    }
    return true;
}

void LineNumbers::setValueRange(int minValue, int maxValue) {
    this->minValue = minValue;
    this->maxValue = maxValue;

    totalNum = maxValue - minValue + 1;
}

void LineNumbers::setBlockColor(const Color3B &normalColor, const Color3B &selectColor) {
    this->normalColor = normalColor;
    this->selectColor = selectColor;
}

void LineNumbers::setBlockSelected(bool isSelect, int num) {
    updateBlockColor(isSelect, num);
    Button* btn = static_cast<Button*>(getChildByTag(num));
    if (btn) {
        btn->setColor(isSelect ? selectColor : normalColor);
        lastSelectNumBtn = btn;
    }
}

void LineNumbers::updateBlockColor(bool isSelect, int num) {
    Button* btn = static_cast<Button*>(getChildByTag(num));
    if (btn) {
        btn->setColor(isSelect ? selectColor : normalColor);
    }
}

void LineNumbers::setNumBtnEnabled(bool enabled) {
    this->enabled = enabled;

    if (getChildrenCount() > 0) {
        for (int i = minValue; i < totalNum; i++) {
            Button* btn = static_cast<Button*>(getChildByTag(i));
            if (btn) {
                btn->setEnabled(enabled);
                btn->setSwallowTouches(enabled);
            }
        }
    }
}

bool LineNumbers::isSelectBlock(int &num) {
    if (lastSelectNumBtn == nullptr) {
        return false;
    }

    num = lastSelectNumBtn->getTag();

    return true;
}

void LineNumbers::setDrawCursorVisible(bool visible) {
    if (drawCursor) {
        drawCursor->setVisible(visible);
    }
}

void LineNumbers::setDrawCursorPosition(float x) {
    int num = getNumByX(x);
    float rx = getXByNum(num);
    if (drawCursor) {
        drawCursor->setPositionX(rx);
    }
}

float LineNumbers::getXByNum(int num) {
    return (num - minValue) * blockSize + blockSize / 2;
}

int LineNumbers::getNumByX(float x) {
    return (int)ceil((x * 1.0 / (totalNum * blockSize) * totalNum));
}

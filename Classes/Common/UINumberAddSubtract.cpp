//
//  UINumberAddSubtract.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/11/22.
//

#include "UINumberAddSubtract.h"

USING_NS_CC;
using namespace ui;
using namespace std;

NumberAddSubtract::NumberAddSubtract()
: subtractSprite(nullptr)
, addSprite(nullptr)
, showText(nullptr)
, borderDraw(nullptr)
, eventListener(nullptr)
, addImageName("button_bar_minus.png")
, substractImageName("button_bar_plus.png")
, textSize(20)
, textColor(Color4B::BLACK)
, textBorderVisible(true)
, textBorderColor(Color3B::BLACK)
, touchEnabled(true)
, curIndex(0)
, selectFlag(-1)
, isLongClick(false)
, isMove(false) {

}

NumberAddSubtract::~NumberAddSubtract() {

}

void NumberAddSubtract::onEnter() {
    Layout::onEnter();

    subtractSprite = Sprite::create(addImageName);
    if (subtractSprite) {
        subtractSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
        subtractSprite->setPosition(Vec2(0, getContentSize().height / 2));
        this->addChild(subtractSprite);
    }

    addSprite = Sprite::create(substractImageName);
    if (addSprite) {
        addSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
        addSprite->setPosition(Vec2(getContentSize().width, getContentSize().height / 2));
        this->addChild(addSprite);
    }

    showText = Text::create();
    showText->setPosition(Vec2(getContentSize() / 2));
    showText->setFontSize(textSize);
    showText->setTextColor(textColor);
    showText->setTextVerticalAlignment(TextVAlignment::CENTER);
    showText->setTextHorizontalAlignment(TextHAlignment::CENTER);
    this->addChild(showText);

    if (textBorderVisible) {
        drawTextBorder();
    }

    if (valueList.size() > 0) {
        setTextContent(valueList[0]);
    }

    eventListener = EventListenerTouchOneByOne::create();
    eventListener->onTouchBegan = CC_CALLBACK_2(NumberAddSubtract::onTouchDown, this);
    eventListener->onTouchMoved = CC_CALLBACK_2(NumberAddSubtract::onTouchMove, this);
    eventListener->onTouchEnded = CC_CALLBACK_2(NumberAddSubtract::onTouchUp, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, this);
}

void NumberAddSubtract::onExit() {

    if (eventListener) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(eventListener);
    }

    unscheduleAllCallbacks();

    Layout::onExit();
}

bool NumberAddSubtract::init() {
    if (!Layout::init()) {
        return false;
    }
    return true;
}

bool NumberAddSubtract::onTouchDown(Touch* t, Event* e) {
    if (!touchEnabled) {
        return false;
    }

    if (getBoundingBox().containsPoint(t->getLocation())) {
        setSwallowTouches(true); //禁止向下传递Touch事件
    }

    Vec2 touchPoint = convertToNodeSpace(t->getLocation());
    if (addSprite->getBoundingBox().containsPoint(touchPoint)) {
        selectFlag = 2;
    } else if (subtractSprite->getBoundingBox().containsPoint(touchPoint)) {
        selectFlag = 1;
    }

    //开始检查是否长按
    if (selectFlag != -1) {
        startTimerForLongClick([this]() {
            isLongClick = true;
            startLongClickWork([this]() {
                updateShowText(selectFlag == 1 ? -1 : 1);
            });
        });
    }

    return true;
}

void NumberAddSubtract::onTouchMove(Touch* t, Event* e) {
    if (selectFlag != -1) { //点中了加减按钮
        if (abs(t->getDelta().x) > 8 || abs(t->getDelta().y) > 8) { //移动了，取消计时
            isMove = true;

            stopTimerForLongClick();
            cancelLongClickWork();
        }
    }
}

void NumberAddSubtract::onTouchUp(Touch* t, Event* e) {

    stopTimerForLongClick();
    cancelLongClickWork();

    if (!isLongClick && !isMove) { //单击
        if (selectFlag != -1) {
            updateShowText(selectFlag == 1 ? -1 : 1);
        }
    }

    setSwallowTouches(false);
    selectFlag = -1;
    isLongClick = false;
    isMove = false;
}

void NumberAddSubtract::setValues(const vector<string> &valueList) {
    this->valueList = valueList;

    if (showText) {
        if (valueList.size() > 0) {
            setTextContent(valueList[0]);
        }
    }
}

void NumberAddSubtract::setTextColor(const Color4B &textColor) {
    this->textColor = textColor;

    if (showText) {
        showText->setTextColor(textColor);
    }
}

void NumberAddSubtract::setTextSize(int textSize) {
    this->textSize = textSize;

    if (showText) {
        showText->setFontSize(textSize);
    }
}

void NumberAddSubtract::setTextBorderVisible(bool visible) {
    this->textBorderVisible = visible;

    if (borderDraw) {
        borderDraw->setVisible(visible);
    }
}

void NumberAddSubtract::setTextBorderColor(const Color4F &borderColor) {
    this->textBorderColor = borderColor;

    if (borderDraw) {
        drawTextBorder();
    }
}

void NumberAddSubtract::setTextContent(const string &content) {
    if (showText) {
        showText->setString(content);
    }
}

void NumberAddSubtract::setAddSubtractImageName(const string &addImageName, const string &subtractImageName) {
    if (addImageName.empty()) {
        this->addImageName = addImageName;
    }

    if (subtractImageName.empty()) {
        this->substractImageName = subtractImageName;
    }

    if (addSprite) {
        addSprite->setTexture(this->addImageName);
    }

    if (subtractSprite) {
        subtractSprite->setTexture(this->substractImageName);
    }
}

void NumberAddSubtract::updateShowText(int calValue) {
    curIndex += calValue;
    if (curIndex < 0) {
        curIndex = (int)valueList.size() - 1;
    } else if (curIndex > valueList.size() - 1) {
        curIndex = 0;
    }
    setTextContent(valueList.at(curIndex));
}

void NumberAddSubtract::drawTextBorder() {

    float width = getContentSize().width - subtractSprite->getContentSize().width - addSprite->getContentSize().width;
    float height = getContentSize().height;

    if (borderDraw == nullptr) {
        borderDraw = DrawNode::create();
        borderDraw->setContentSize(Size(width, height));
        borderDraw->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        borderDraw->setPosition(Vec2(getContentSize() / 2));
        this->addChild(borderDraw);
    }

    borderDraw->clear();
    Vec2 pointList[] = {Vec2(1, 1), Vec2(width - 1, 1), Vec2(width - 1, height - 1), Vec2(1, height - 1)};
    borderDraw->drawPoly(pointList, 4, true, textBorderColor);
}

void NumberAddSubtract::startTimerForLongClick(std::function<void()> onLongClick) {
    scheduleOnce([this, onLongClick](float d){
        if (onLongClick) {
            onLongClick();
        }
    }, 0.4f, "longClickSchedule");
}

void NumberAddSubtract::stopTimerForLongClick() {
    unschedule("longClickSchedule");
}

void NumberAddSubtract::startLongClickWork(function<void()> onReachInterval) {
    schedule([this, onReachInterval](float d){
        if (onReachInterval) {
            onReachInterval();
        }
    }, 0.1f, "longClickWorkSchedule");
}

void NumberAddSubtract::cancelLongClickWork() {
    unschedule("longClickWorkSchedule");
}

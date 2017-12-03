//
//  CommonTest.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/11/22.
//

#include "CommonTest.h"
#include "UINumberAddSubtract.h"
#include "UINumberSelector.h"
#include "UICircleNode.h"

USING_NS_CC;
using namespace ui;
using namespace std;

//获取字符、字符串数组长度模板
template <class T>
int getArrayLen(T &array) {
    return (sizeof(array) / sizeof(array[0]));
}

const string numberValues[] = {
        "一", "二", "三", "四", "五", "六", "七", "八", "九", "十", "十一",
        "十二", "十三", "十四", "十五", "十六", "十七", "十八", "十九", "二十"
};

CommonTest::CommonTest() {

}

CommonTest::~CommonTest() {

}

bool CommonTest::init() {
    if (!BaseLayer::init()) {
        return false;
    }

    vector<string> valueList;
    for (int i = 0; i < getArrayLen(numberValues); i++) {
        valueList.push_back(numberValues[i]);
    }

    NumberAddSubtract* addSubtract = NumberAddSubtract::create();
    addSubtract->setContentSize(Size(170, 40));
    addSubtract->setPosition(Vec2(200, 200));
    addSubtract->setValues(valueList);
    addSubtract->setTextColor(Color4B::RED);
    addSubtract->setTextBorderColor(Color4F::BLUE);
    this->addChild(addSubtract);

    NumberSelector* numberSelector = NumberSelector::create();
    numberSelector->setContentSize(Size(80, 130));
    numberSelector->setPosition(Vec2(400, 200));
    numberSelector->setValues(valueList);
    numberSelector->setBgColor(Color3B::GRAY);
    numberSelector->setBgOpacity(125);
    numberSelector->setSelectItem(5);
    numberSelector->setOnSelectCallback([this](int index, string result){
        log("当前选中了 %s", result.c_str());
    });
    this->addChild(numberSelector);

    Layout* showLayout = Layout::create();
    showLayout->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    showLayout->setContentSize(Size(numberSelector->getContentSize().width, numberSelector->getContentSize().height / 5 + 10));
    showLayout->setBackGroundColor(Color3B::GRAY);
    showLayout->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    showLayout->setBackGroundColorOpacity(100);
    showLayout->setPosition(Vec2(numberSelector->getContentSize() / 2));
    numberSelector->addChild(showLayout);

    CircleNode* circleNode = CircleNode::create();
    circleNode->setContentSize(Size(180, 180));
    circleNode->setPosition(Vec2(400, 400));
    circleNode->setValue(6, 2, "circle_image.png");
    this->addChild(circleNode);

    Button* randBtn = Button::create();
    randBtn->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    randBtn->setPosition(Vec2(circleNode->getContentSize().width + 10, 0));
    randBtn->setTitleColor(Color3B::BLACK);
    randBtn->setTitleText("随机");
    randBtn->setTitleFontSize(24);
    randBtn->addClickEventListener([this, circleNode](Ref* pSender){
        int divideNum, resultNum;
        do {
            divideNum = rand() % 10 + 1;
            resultNum = rand() % 9 + 1;
        } while (divideNum <= resultNum);

        circleNode->setValue(divideNum, resultNum, "circle_image.png");
    });
    circleNode->addChild(randBtn);

    return true;
}

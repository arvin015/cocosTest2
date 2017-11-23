//
//  CommonTest.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/11/22.
//

#include "CommonTest.h"
#include "UINumberAddSubtract.h"

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

    return true;
}
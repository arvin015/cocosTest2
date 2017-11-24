//
//  UINumberSelector.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/11/23.
//

#include "UINumberSelector.h"

USING_NS_CC;
using namespace ui;
using namespace std;

const float itemMargin = 8;

NumberSelector::NumberSelector()
: listView(nullptr)
, textSize(20)
, textColor(Color4B::BLACK)
, bgOpacity(0)
, bgColor(Color3B::WHITE)
, showItemNum(3)
, selectItem(1) {

}

NumberSelector::~NumberSelector() {

}

void NumberSelector::onEnter() {
    Node::onEnter();

    listView = ListView::create();
    listView->setContentSize(getContentSize());
    listView->setScrollBarOpacity(0); //隐藏ScrollBar
    if (!bgImageName.empty()) {
        listView->setBackGroundImage(bgImageName);
    } else {
        listView->setBackGroundColor(bgColor);
        listView->setBackGroundColorType(ListView::BackGroundColorType::SOLID);
        listView->setBackGroundColorOpacity(bgOpacity);
    }
    listView->ScrollView::addEventListener([this](Ref* ref, ScrollView::EventType eventType) {

        if (eventType == ScrollView::EventType::AUTOSCROLL_ENDED) {

            ListView* listView = static_cast<ListView*>(ref);
            Layout* itemLayout = static_cast<Layout*>(listView->getCenterItemInCurrentView());
            int tag = itemLayout->getTag();

            scheduleOnce([this, listView, tag](float dt){
                listView->jumpToItem(tag, Vec2::ANCHOR_MIDDLE, Vec2::ANCHOR_MIDDLE);
            }, 0.1f, "jumpToItemSchedule");

            if (selectCallback) {
                this->selectCallback(tag, valueList.at(tag));
            }
            
        } else {
            
            Layout* topLayout = static_cast<Layout*>(listView->getTopmostItemInCurrentView());
            Layout* centerLayout = static_cast<Layout*>(listView->getCenterItemInCurrentView());
            Layout* bottomLayout = static_cast<Layout*>(listView->getBottommostItemInCurrentView());
            
            if (topLayout) {
                Text* topText = static_cast<Text*>(topLayout->getChildByName("text"));
                if (topText) {
                    topText->setFontSize(textSize);
                    topText->setTextColor(textColor);
                }
            }
            
            if (centerLayout) {
                Text* centerText = static_cast<Text*>(centerLayout->getChildByName("text"));
                if (centerText) {
                    centerText->setFontSize(textSize + 3);
                    centerText->setTextColor(Color4B::BLUE);
                }
            }
            
            if (bottomLayout) {
                Text* bottomText = static_cast<Text*>(bottomLayout->getChildByName("text"));
                if (bottomText) {
                    bottomText->setFontSize(textSize);
                    bottomText->setTextColor(textColor);
                }
            }
        }
    });
    this->addChild(listView);

    float itemHeight = (getContentSize().height - (showItemNum - 1) * itemMargin) / showItemNum;

    //default_item
    Layout* defaultItem = Layout::create();
    defaultItem->setContentSize(Size(getContentSize().width, itemHeight));

    Text* text = Text::create();
    text->setFontSize(textSize);
    text->setTextColor(textColor);
    text->setPosition(Vec2(defaultItem->getContentSize() / 2));
    text->setName("text");
    defaultItem->addChild(text);

    //设置模板
    listView->setItemModel(defaultItem);
    //设置Item间距
    listView->setItemsMargin(itemMargin);
    //设置内大小
    listView->setInnerContainerSize(Size(getContentSize().width, valueList.size() * itemHeight + (showItemNum - 1) * itemMargin));

    //设置数据
    for (int i = 0; i < valueList.size(); i++) {
        Layout* layout = (Layout *) defaultItem->clone();
        layout->setTag(i);
        Text* text = static_cast<Text*>(layout->getChildByName("text"));
        if (text) {
            text->setString(valueList.at(i));
        }
        listView->pushBackCustomItem(layout);
    }
    setSelectItem(selectItem);
}

void NumberSelector::onExit() {
    Node::onExit();
}

bool NumberSelector::init() {
    if (!Node::init()) {
        return false;
    }

    return true;
}

void NumberSelector::setValues(vector<string> valueList) {
    this->valueList = valueList;
}

void NumberSelector::setSelectItem(int selectItem) {
    this->selectItem = selectItem;

    if (listView) {
        listView->jumpToItem(selectItem, Vec2::ANCHOR_MIDDLE, Vec2::ANCHOR_MIDDLE);
    }
}


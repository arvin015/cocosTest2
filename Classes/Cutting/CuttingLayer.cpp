//
//  CuttingLayer.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/12/13.
//

#include "CuttingLayer.h"

USING_NS_CC;
using namespace ui;
using namespace std;

CuttingLayer::CuttingLayer()
: BaseLayer() {

}

CuttingLayer::~CuttingLayer() {

}

void CuttingLayer::onEnter() {
    BaseLayer::onEnter();
}

void CuttingLayer::onExit() {
    BaseLayer::onExit();
}

bool CuttingLayer::init() {
    if (!BaseLayer::init()) {
        return false;
    }

    vector<PolygonPoint> pointList;
    pointList.push_back(PolygonPoint(200, 200));
    pointList.push_back(PolygonPoint(400, 200));
    pointList.push_back(PolygonPoint(400, 400));
    pointList.push_back(PolygonPoint(200, 400));

    createPolygon(pointList);

    RadioButtonGroup* radioButtonGroup = RadioButtonGroup::create();
    radioButtonGroup->addEventListener([this](RadioButton* radioButton, int index, RadioButtonGroup::EventType eventType) {
        if (eventType == RadioButtonGroup::EventType::SELECT_CHANGED) {
            changeMethod((PolygonView::OperateType)index);
        }
    });
    this->addChild(radioButtonGroup);

    RadioButton* radioButton1 = RadioButton::create("mian_button_01_125x54.png", "mian_button_01_125x54s.png");
    radioButton1->setPosition(Vec2(70, 570));
    radioButton1->setTag(1);
    radioButtonGroup->addRadioButton(radioButton1);
    radioButtonGroup->addChild(radioButton1);

    Text* text1 = Text::create();
    text1->setString("切割");
    text1->setFontSize(22);
    text1->setTextColor(Color4B::WHITE);
    text1->setPosition(radioButton1->getContentSize() / 2);
    radioButton1->addChild(text1);

    RadioButton* radioButton2 = RadioButton::create("mian_button_01_125x54.png", "mian_button_01_125x54s.png");
    radioButton2->setPosition(Vec2(70, 500));
    radioButton2->setTag(2);
    radioButtonGroup->addRadioButton(radioButton2);
    radioButtonGroup->addChild(radioButton2);

    Text* text2 = Text::create();
    text2->setString("移动");
    text2->setFontSize(22);
    text2->setTextColor(Color4B::WHITE);
    text2->setPosition(radioButton1->getContentSize() / 2);
    radioButton2->addChild(text2);

    return true;
}

void CuttingLayer::createPolygon(vector<PolygonPoint> pointList) {

    PolygonView* polygonView = PolygonView::create();
    polygonView->setValue(pointList);
    polygonView->setOnCutCallback([this](vector<PolygonPoint> pointList) {
        createPolygon(pointList);
    });
    this->addChild(polygonView);

    polygonList.pushBack(polygonView);
}

void CuttingLayer::changeMethod(PolygonView::OperateType operateType) {
    for (PolygonView* polygonView : polygonList) {
        polygonView->setOperateType(operateType);
    }
}
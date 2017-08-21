//
//  TableViewTest.cpp
//  CocosTest2
//
//  Created by arvin on 2017/8/21.
//
//

#include "TableViewTest.h"
#include "UIDatasetTableView.h"

USING_NS_CC;
using namespace ui;
using namespace std;

TableViewTest::TableViewTest() {

}

TableViewTest::~TableViewTest() {

}

bool TableViewTest::init() {
    if(!BaseLayer::init()) {
        return false;
    }
    
    tableView = DatasetTableView::create();
    tableView->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    tableView->setContentSize(Size(860, 560));
    tableView->setPosition(Vec2(V_WIDTH / 2, V_HEIGHT / 2));
    addChild(tableView);
    
    RadioButtonGroup* rbGroup = RadioButtonGroup::create();
    rbGroup->addEventListener(CC_CALLBACK_3(TableViewTest::onRadioButtonSelect, this));
    addChild(rbGroup);
    
    float startX = 85;
    float startY = V_HEIGHT - 40;
    float rbHeight = 54;
    float rbWidth = 125;
    float padding = 20;
    
    for(int i = 0; i < 6; i++) {
        RadioButton* rbBtn = RadioButton::create("mian_button_01_125x54.png", "mian_button_01_125x54s.png");
        rbBtn->setContentSize(Size(rbWidth, rbHeight));
        rbBtn->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
        rbBtn->setPosition(Vec2(startX + i * (padding + rbWidth), startY));
        rbBtn->setTag(i + 1);
        rbGroup->addRadioButton(rbBtn);
        addChild(rbBtn);
        
        if(i == 0) {
            rbGroup->setSelectedButton(rbBtn); //选中会触发选中事件
//            rbGroup->setSelectedButtonWithoutEvent(rbBtn); //选中不会触发选中事件
        }
    }
    
    return true;
}

void TableViewTest::onRadioButtonSelect(RadioButton* radioButton, int index, RadioButtonGroup::EventType eventType) {
    if(eventType == RadioButtonGroup::EventType::SELECT_CHANGED) {
        
        int tag = radioButton->getTag();
        
        datasetList.clear();
        for(int i = 0; i < tag; i++) {
            DatasetInfo info;
            info.img = "HelloWorld.png";
            info.title = StringUtils::format("年级 %d", tag);
            datasetList.push_back(info);
        }
        
        tableView->setData(datasetList);
    }
}

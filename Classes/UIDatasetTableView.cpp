//
//  UIDatasetTableView.cpp
//  CocosTest2
//
//  Created by arvin on 2017/8/18.
//
//

#include "UIDatasetTableView.h"
#include "ui/UIText.h"
#include "ui/UIImageView.h"
#include "DatasetInfo.h"

USING_NS_CC;
using namespace ui;
using namespace std;

DatasetTableView::DatasetTableView()
: listener(nullptr) {

}

DatasetTableView::~DatasetTableView() {

}

void DatasetTableView::onEnter() {
    CommonTableView::onEnter();
}

void DatasetTableView::onExit() {

    CommonTableView::onExit();
}

bool DatasetTableView::init() {
    
    if(!CommonTableView::init()) {
        return false;
    }
    
    setBasicData(280, 250, 10, 10, 3, 560);
    
    setTableViewImpl(this);
    
    return true;
}

void DatasetTableView::layoutItem(Layout* itemLayout) {
    
    ImageView* img = ImageView::create();
    img->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    img->setPosition(Vec2(itemLayout->getContentSize().width / 2, itemLayout->getContentSize().height - 80));
    img->setTag(100);
    img->setScale(0.5f);
    itemLayout->addChild(img);
    
    Text* text = Text::create();
    text->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    text->setPosition(Vec2(70, itemLayout->getContentSize().height - 14));
    text->setFontSize(30);
    text->setTextColor(Color4B::BLACK);
    text->setTag(200);
    itemLayout->addChild(text);
}

void DatasetTableView::setItemData(Layout* itemLayout, long itemIndex, int row, bool isSelected) {
    
    DatasetInfo info = datasetList[itemIndex];
    
    if(isSelected) {
        itemLayout->setBackGroundImage("tbg_pic_01c.png");
    } else {
        itemLayout->setBackGroundImage("tbg_pic_01.png");
    }
    
    ImageView* img = static_cast<ImageView*>(itemLayout->getChildByTag(100));
    if(img) {
        img->loadTexture(info.img);
    }
    
    Text* text = static_cast<Text*>(itemLayout->getChildByTag(200));
    if(text) {
        text->setString(info.title);
    }
}

void DatasetTableView::onGridItemClicked(int selectIndex) {
    if(listener) {
        this->listener(selectIndex);
    }
}

void DatasetTableView::setData(vector<DatasetInfo> datasetList) {

    this->datasetList = datasetList;
    
    setItemCounts((int)datasetList.size());
}


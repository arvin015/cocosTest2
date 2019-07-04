//
// Created by Administrator on 2019/7/4.
//

#include "ClassScrollView.h"
#include "ui/UIText.h"

USING_NS_CC;
using namespace ui;
using namespace std;

const int SpaceY = 5; //间距
Size RowSize;
Size ClassSize;
const int RowTextSize = 26;
const int ClassTextSize = 24;

ClassScrollView* ClassScrollView::create(const Size &containerSize) {
    auto classScrollView = new ClassScrollView(containerSize);
    if (classScrollView && classScrollView->init()) {
        classScrollView->autorelease();
        return classScrollView;
    }
    CC_SAFE_RELEASE(classScrollView);
    return nullptr;
}

ClassScrollView::ClassScrollView(const Size &containerSize)
: totalY(0) {
    this->containerSize = containerSize;

    RowSize = Size(containerSize.width, 60);
    ClassSize = Size(containerSize.width, 60);
}

ClassScrollView::~ClassScrollView() {

}

void ClassScrollView::onEnter() {
    base::onEnter();
}

void ClassScrollView::onExit() {
    base::onExit();
}

bool ClassScrollView::init() {
    if (!base::init()) {
        return false;
    }

    setInnerContainerSize(containerSize);

    return true;
}

void ClassScrollView::setData(const map<int, vector<string>> &dataMap) {
    this->dataMap = dataMap;
    reload();
}

void ClassScrollView::addClass(int row, const string &className) {

}

void ClassScrollView::removeClass(int index) {

}

ImageView* ClassScrollView::createRowUI(const string &title) {
    auto img = ImageView::create();
    img->setContentSize(RowSize);
    auto titleText = Text::create();
    titleText->setFontSize(RowTextSize);
    titleText->setPosition(Vec2(RowSize / 2));
    titleText->setTextColor(Color4B::BLACK);
    titleText->setName("titleText");
    titleText->setString(title);
    img->addChild(titleText);
    return img;
}

Node* ClassScrollView::createClassUI() {

}

void ClassScrollView::addRow(int row) {
    auto imageView = createRowUI(StringUtils::format("第%d行", row+1));
    imageView->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    imageView->setPosition(Vec2(containerSize.width / 2, totalY));
    addChild(imageView);

    totalY += imageView->getContentSize().height + SpaceY;
}

void ClassScrollView::updateCell(int index, bool isAdd) {

}

void ClassScrollView::reload() {
    removeAllChildren();

    float innerHeight;
    for (auto it = dataMap.begin(); it != dataMap.end(); it++) {
        innerHeight += RowSize.height + SpaceY;
        vector<string> classes = it->second;
        innerHeight += classes.size() * ClassSize.height + (classes.size() - 1) * SpaceY;
    }
    log("---innerHeight = %f", innerHeight);
    setInnerContainerSize(Size(containerSize.width, innerHeight));

    totalY = getInnerContainerSize().height;

    for (auto it = dataMap.begin(); it != dataMap.end(); it++) {
        int row = it->first;
        vector<string> classes = it->second;
        addRow(row);
    }
}

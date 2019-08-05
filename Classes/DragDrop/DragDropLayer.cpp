//
// Created by mac_arvin on 2019-08-02.
//

#include "DragDropLayer.h"
#include "UIDragNode.h"
#include "PacToast.h"
#include "ui/UIImageView.h"

USING_NS_CC;
using namespace ui;
using namespace std;

const string ImgStrs[] = {
   "img_animal_dog.png",
   "img_animal_rabbit.png",
   "img_animal_tortoise.png"
};

const string NameStrs[] = {
   "小狗",
   "小兔子",
   "小乌龟"
};

DragDropLayer::DragDropLayer() : base()
, curDragNode(nullptr)
, deleteBtn(nullptr) {

}

DragDropLayer::~DragDropLayer() {

}

void DragDropLayer::onEnter() {
    base::onEnter();
}

void DragDropLayer::onExit() {
    base::onExit();
}

bool DragDropLayer::init() {
    if (!base::init()) {
        return false;
    }

    deleteBtn = Button::create("button_bar_delete.png");
    deleteBtn->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
    deleteBtn->setPosition(Vec2(V_WIDTH - 100, 500));
    addChild(deleteBtn);

    auto scrollView = ScrollView::create();
    scrollView->setContentSize(Size(250, 105));
    scrollView->setPosition(Vec2(30, 80));
    scrollView->setDirection(ScrollView::Direction::HORIZONTAL);
    scrollView->setScrollBarEnabled(false);
    addChild(scrollView);
    auto drawNode = DrawNode::create();
    drawNode->drawRect(scrollView->getPosition(), scrollView->getPosition() + scrollView->getContentSize(), Color4F::BLUE);
    addChild(drawNode);

    float innerWidth = 0;
    for (int i = 0; i < 3; i++) {

        Size size = Size(102, 102);

        auto dragNode = DragNode::create();
        dragNode->setContentSize(size);
        dragNode->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
        dragNode->setPosition(Vec2(i * (size.width + 5), size.height / 2));
        dragNode->setOnDragEndListener([this, dragNode](Vec2 worldVec) {
            this->curDragNode = dragNode;
            dragEndHandle(worldVec);
        });
        dragNode->setName(NameStrs[i]);
        scrollView->addChild(dragNode);

        auto questionImg = ImageView::create("tbg_pic_01.png");
        questionImg->setPosition(Vec2(size / 2));
        questionImg->setContentSize(size);
        questionImg->setScale9Enabled(true);
        dragNode->addChild(questionImg);
        auto sp = Sprite::create(ImgStrs[i]);
        sp->setPosition(Vec2(size / 2));
        questionImg->addChild(sp);

        innerWidth += size.width + (i == 0 ? 0 : 5);
    }
    scrollView->setInnerContainerSize(Size(innerWidth, scrollView->getContentSize().height));

    return true;
}

void DragDropLayer::dragEndHandle(const Vec2 &worldVec) {
    if (deleteBtn->getBoundingBox().containsPoint(deleteBtn->getParent()->convertToNodeSpace(worldVec))) {
        PacToast::makeText("删除" + curDragNode->getName());
    }
}

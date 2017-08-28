//
// Created by Administrator on 2017/8/27.
//

#include "DragImageView.h"

USING_NS_CC;
using namespace ui;
using namespace std;

DragImageView::DragImageView()
: eventListener(nullptr)
, deleteBtn(nullptr)
, isMoveEnabled(true)
, isSelected(false)
, isMove(false) {

}

DragImageView::~DragImageView() {

}

void DragImageView::onEnter() {
    ImageView::onEnter();

    eventListener = EventListenerTouchOneByOne::create();
    eventListener->onTouchBegan = CC_CALLBACK_2(DragImageView::onTouchBegan, this);
    eventListener->onTouchMoved = CC_CALLBACK_2(DragImageView::onTouchMoved, this);
    eventListener->onTouchEnded = CC_CALLBACK_2(DragImageView::onTouchEnded, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, this);
}
void DragImageView::onExit() {

    if(eventListener) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(eventListener);
    }

    ImageView::onExit();
}

bool DragImageView::init() {
    if(!ImageView::init()) {
        return false;
    }
    
    deleteBtn = Button::create("button_bar_delete.png");
    deleteBtn->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    deleteBtn->setPosition(Vec2(100, 100));
    deleteBtn->setVisible(false);
    deleteBtn->addClickEventListener([this](Ref* pSender){
        this->deleteCallback(); //抛出去
        this->removeFromParent();
    });
    this->addChild(deleteBtn);

    return true;
}

bool DragImageView::init(const std::string &imageFileName, TextureResType texType)
{
    bool bRet = true;
    do
    {
        if (!init())
        {
            bRet = false;
            break;
        }

        this->loadTexture(imageFileName, texType);
    } while (0);
    return bRet;
}

DragImageView* DragImageView::create(const string &filename) {
    DragImageView *widget = new (std::nothrow) DragImageView;
    if (widget && widget->init(filename))
    {
        widget->autorelease();
        return widget;
    }
    CC_SAFE_DELETE(widget);
    return nullptr;
}

void DragImageView::setDeleteBtnVisible(bool visible) {
    if(deleteBtn) {
        deleteBtn->setVisible(visible);
    }
}

void DragImageView::bringToFront() {
    Vector<Node*> nodeList = getParent()->getChildren();
    Node* node = nodeList.at(getParent()->getChildrenCount() - 1); //获取最上面的那个Node
    this->getParent()->reorderChild(this, node->getLocalZOrder() + 1);
}

bool DragImageView::onTouchBegan(Touch* touch, Event* event) {
    
    if(!isMoveEnabled) {
        return false;
    }

    if(touch->getID() != 0) {
        return false;
    }

    if(getBoundingBox().containsPoint(touch->getLocation())) { //选中了
        bringToFront(); //置顶
        
        isSelected = true;
        eventListener->setSwallowTouches(true);
    } else { //未选中
        setDeleteBtnVisible(false);
    }
    
    return true;
}

void DragImageView::onTouchMoved(Touch* touch, Event* event) {
    if(touch->getID() != 0) {
        return;
    }

    if(isSelected) {
        if(abs(touch->getDelta().x) > 1.0f || abs(touch->getDelta().y) > 1.0f) { //判定为移动
            isMove = true;
            this->setPosition(getPosition() + touch->getDelta());
        }
        
    }
}

void DragImageView::onTouchEnded(Touch* touch, Event* event) {
    if(isSelected) {

        if(isMove) { //移动状态
            this->moveEndedCallback(); //抛出去
            isMove = false;
        } else { //点击状态，显示删除按钮
            if(deleteBtn) {
                setDeleteBtnVisible(true);
            }
        }
        
        eventListener->setSwallowTouches(false);
        isSelected = false;
    }
}

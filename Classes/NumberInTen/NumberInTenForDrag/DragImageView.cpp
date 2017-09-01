//
// Created by Administrator on 2017/8/27.
//

#include "DragImageView.h"
#include "CommonUtils.h"

USING_NS_CC;
using namespace ui;
using namespace std;

DragImageView::DragImageView()
: eventListener(nullptr)
, deleteBtn(nullptr)
, showNumText(nullptr)
, showCountText(nullptr)
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
    deleteBtn->setPosition(Vec2(58, 58));
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

void DragImageView::addShowNum(int showNum) {
    showNumText = Text::create();
    showNumText->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    showNumText->setPosition(Vec2(getContentSize().width / 2, getContentSize().height));
    showNumText->setFontSize(18);
    showNumText->setTextColor(Color4B(255, 133, 136, 255));
    showNumText->setString(Value(showNum).asString());
    this->addChild(showNumText);
    
    setIsMoveEnabled(false); //不可移动
}

void DragImageView::addShowCountNum(int countNum, int opacity) {
    showCountText = Text::create();
    showCountText->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    showCountText->setPosition(Vec2(getContentSize().width / 2, 0));
    showCountText->setFontSize(18);
    showCountText->setTextColor(Color4B(91, 155, 213, 255));
    showCountText->setString(Value(countNum).asString());
    showCountText->setOpacity(opacity);
    this->addChild(showCountText);
}

int DragImageView::getShowNum() {
    if(showNumText) {
        showNumText->getString();
    }
    
    return -1;
}

void DragImageView::setIsTranslucent(bool isTranslucent) {
    if(isTranslucent) {
        setOpacity(127); //半透明
    } else {
        setOpacity(255); //不透明
    }
}

void DragImageView::bringToFront() {
    Vector<Node*> nodeList = getParent()->getChildren();
    Node* node = nodeList.at(getParent()->getChildrenCount() - 1); //获取最上面的那个Node
    this->getParent()->reorderChild(this, node->getLocalZOrder() + 1);
}

void DragImageView::fromJson(const rapidjson::Value &json) {
    if(!json.IsObject()) {
        return;
    }
    
    float x = json["x"].GetFloat();
    float y = json["y"].GetFloat();
    string image = json["image"].GetString();
    int showNum = json["showNum"].GetInt();
    int countNum = json["countNum"].GetInt();
    int rectType = json["rectType"].GetInt();
    
    setPosition(Vec2(x, y));
    loadTexture(image);
    setName(image);
    setTag(rectType);
    
    //是否显示数字
    if(showNum != -1) {
        addShowNum(showNum);
    }
    //是否显示统计数字
    if(countNum != -1) {
        addShowCountNum(countNum);
    }
}

void DragImageView::toJson(rapidjson::Document &obj) {

    obj.SetObject();
    
    //x, y, image, showNum, countNum
    int showNum = showNumText ? Value(showNumText->getString()).asInt() : -1;
    int countNum = showCountText ? Value(showCountText->getString()).asInt() : -1;
    
    obj.AddMember("x", getPositionX(), obj.GetAllocator());
    obj.AddMember("y", getPositionY(), obj.GetAllocator());
    obj.AddMember("image", rapidjson::Value(getName().c_str(), obj.GetAllocator()), obj.GetAllocator());
    obj.AddMember("showNum", showNum, obj.GetAllocator());
    obj.AddMember("countNum", countNum, obj.GetAllocator());
    obj.AddMember("rectType", getTag(), obj.GetAllocator());
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

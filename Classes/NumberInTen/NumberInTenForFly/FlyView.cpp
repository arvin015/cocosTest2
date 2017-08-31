//
//  FlyView.cpp
//  CocosTest2
//
//  Created by arvin on 2017/8/31.
//
//

#include "FlyView.h"
#include "ui/UIText.h"
#include "ui/UIImageView.h"
#include "UISpinner.h"
#include "PacToast.h"

USING_NS_CC;
using namespace ui;
using namespace std;

const int BEE_FLY_AREA_HEIGHT = 80;

//根据编号获取X坐标
inline float getXByNum(int num) {
    return (num - 1) * FLY_GRID_WIDTH + FLY_GRID_WIDTH / 2;
}

//根据X坐标获取对应的编号
inline int getNumByX(float x) {
    return (int)ceil((x * 1.0 / (10 * FLY_GRID_WIDTH) * 10));
}

FlyView::FlyView()
: eventListener(nullptr)
, tipView(nullptr)
, beeSprite(nullptr)
, deleteBtn(nullptr)
, isMove(false)
, isSelect(false)
, spinner(nullptr)
, selectNum(-1)
, beginBtn(nullptr)
, endBtn(nullptr)
, isClickEnabled(true)
, arrowImage(nullptr) {
    
}

FlyView::~FlyView() {
    
}

void FlyView::onEnter() {
    Layout::onEnter();
    
    eventListener = EventListenerTouchOneByOne::create();
    eventListener->onTouchBegan = CC_CALLBACK_2(FlyView::onTouchBegan, this);
    eventListener->onTouchMoved = CC_CALLBACK_2(FlyView::onTouchMoved, this);
    eventListener->onTouchEnded = CC_CALLBACK_2(FlyView::onTouchEnded, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, this);
}

void FlyView::onExit() {
    
    if(eventListener) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(eventListener);
    }
    
    Layout::onExit();
}

bool FlyView::init() {
    if(!Layout::init()) {
        return false;
    }
    
    setContentSize(Size(FLY_GRID_WIDTH * 10, FLY_GRID_HEIGHT));
    
    //创建提示
    tipView = Layout::create();
    tipView->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    tipView->setPosition(Vec2(0, getContentSize().height + BEE_FLY_AREA_HEIGHT));
    tipView->setBackGroundImage("tip_text_bg.png");
    tipView->setBackGroundImageScale9Enabled(true);
    this->addChild(tipView);
    
    Text* tipText = Text::create();
    tipText->setTextColor(Color4B::WHITE);
    tipText->setFontSize(24);
    tipText->setString("請點按格子選擇蜜蜂飛舞的起點和終點");
    
    tipView->addChild(tipText);
    tipView->setContentSize(Size(tipText->getContentSize().width + 8, tipText->getContentSize().height + 6));
    tipView->requestDoLayout();
    
    tipText->setPosition(Vec2(tipView->getContentSize().width / 2, tipView->getContentSize().height / 2));
    
    //创建删除
    deleteBtn = Button::create("button_bar_delete.png");
    deleteBtn->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    deleteBtn->setPosition(Vec2(getContentSize().width, getContentSize().height));
    this->addChild(deleteBtn);
    
    //左边指示框
    Sprite* leftSprite = Sprite::create("end.png");
    leftSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
    leftSprite->setPosition(Vec2(0, getContentSize().height / 2));
    this->addChild(leftSprite);
    
    //右边指示箭头
    Sprite* rightSprite = Sprite::create("head.png");
    rightSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    rightSprite->setPosition(Vec2(getContentSize().width, getContentSize().height / 2));
    this->addChild(rightSprite);
    
    //创建数字按钮
    for(int i = 0; i < 10; i++) {
        Button* numBtn = Button::create("black_frame.png", "black_frame.png", "black_frame.png");
        numBtn->setContentSize(Size(FLY_GRID_WIDTH, FLY_GRID_HEIGHT));
        numBtn->setScale9Enabled(true);
        numBtn->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
        numBtn->setPosition(Vec2(i * FLY_GRID_WIDTH, getContentSize().height / 2));
        numBtn->setTitleText(Value(i + 1).asString());
        numBtn->setTitleColor(Color3B::BLACK);
        numBtn->setTitleFontSize(26);
        numBtn->setColor(Color3B(153, 204, 255));
        numBtn->setTag(i + 1);
        //考虑到示范模式，放弃Button点击事件
        numBtn->setEnabled(false);
        numBtn->setSwallowTouches(false);
        this->addChild(numBtn);
    }
    
    //创建蜜蜂
    beeSprite = Sprite::create("bee.png");
    beeSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    beeSprite->setPosition(Vec2(FLY_GRID_WIDTH / 2, getContentSize().height + 20));
    this->addChild(beeSprite);
    
    //创建起点
    beginBtn = Button::create("image_begin.png");
    beginBtn->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    beginBtn->setVisible(false);
    beginBtn->setTag(-1); //beginBtn关联的数字
    beginBtn->addClickEventListener([this](Ref* pSender){
        this->onClickNumHandle(beginBtn->getTag());
    });
    this->addChild(beginBtn);
    
    //创建终点
    endBtn = Button::create("image_end.png");
    endBtn->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    endBtn->setVisible(false);
    endBtn->setTag(-1); //endBtn关联的数字
    endBtn->addClickEventListener([this](Ref* pSender){
        this->onClickNumHandle(endBtn->getTag());
    });
    this->addChild(endBtn);
    
    //绘制边框
    DrawNode* frameDraw = DrawNode::create();
    Vec2 verts[4];
    verts[0] = Vec2::ZERO;
    verts[1] = Vec2(0, getContentSize().height);
    verts[2] = Vec2(getContentSize().width, getContentSize().height);
    verts[3] = Vec2(getContentSize().width, 0);
    frameDraw->drawPolygon(verts, 4, Color4F(0.0f, 0.0f, 0.0f, 0.0f), 1, Color4F::BLACK);
    this->addChild(frameDraw);
    
    return true;
}

bool FlyView::onTouchBegan(Touch* touch, Event* event) {
    
    if(!isClickEnabled) {
        return false;
    }
    
    if(touch->getID() != 0) {
        return false;
    }
    
    if(getBoundingBox().containsPoint(touch->getLocation())) {
        isSelect = true;
    }
    
    return true;
}

void FlyView::onTouchMoved(Touch* touch, Event* event) {
    
    if(touch->getID() != 0) {
        return;
    }
    
    if(isSelect) {
        if(abs(touch->getDelta().x) > 2.0f || abs(touch->getDelta().y) > 2.0f) {
            isMove = true;
        }
    }
}

void FlyView::onTouchEnded(Touch* touch, Event* event) {
    
    if(isSelect && !isMove) { //点击事件
        float x = convertToNodeSpace(touch->getLocation()).x;
        onClickNumHandle(getNumByX(x));
    }
    
    isSelect = false;
    isMove = false;
}

void FlyView::onClickNumHandle(int num) {
    if(spinner == nullptr) {
        
        vector<string> list;
        list.push_back("起點");
        list.push_back("終點");
        
        spinner = Spinner::create();
        spinner->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
        spinner->setOnSpinnerSelectedListener(this);
        spinner->setItemFontColor(Color4B::WHITE);
        spinner->setItemFontSize(22);
        spinner->setItemBackgroundColor(Color3B(255, 124, 128), Color3B::ORANGE);
        spinner->setItemDividerColor(Color3B::WHITE);
        spinner->setParameters(list, FLY_GRID_WIDTH + 10, 30);
        this->addChild(spinner);
    }
    
    spinner->setVisible(true);
    
    selectNum = num;
    
    spinner->setPosition(Vec2(getXByNum(selectNum), -10));
}

void FlyView::onItemSelected(int index, const string &content) {
    Button* curBtn = nullptr;
    if(index == 0) {
        curBtn = beginBtn;
    } else if(index == 1) {
        curBtn = endBtn;
    }
    
    //去掉一种情况，起点和终点都存在，并且当前点击的数字下面已经有起点或终点
    if(existB_E() && (beginBtn->getTag() == selectNum || endBtn->getTag()== selectNum)) {
        if(index == 0) {
            PacToast::makeText("已經存在起點了");
        } else {
            PacToast::makeText("已經存在終點了");
        }
        return;
    }
    
    //判断起点或终点是否关联了当前点击数字，有则隐藏
    if(beginBtn->getTag() == selectNum) {
        beginBtn->setVisible(false);
        beginBtn->setTag(-1);
    }
    if(endBtn->getTag() == selectNum) {
        endBtn->setVisible(false);
        endBtn->setTag(-1);
    }
    
    if(curBtn && selectNum != -1) {
        
        float x = getXByNum(selectNum);
        
        curBtn->setVisible(true);
        curBtn->setTag(selectNum);
        curBtn->setPosition(Vec2(x, -10));
        
        //设置起点时，蜜蜂移动到起点位置
        if(index == 0) {
            beeSprite->stopAllActions();
            auto moveAction = MoveTo::create(0.5f, Vec2(x, beeSprite->getPositionY()));
            beeSprite->runAction(moveAction);
        }
        
        selectNum = -1;
    }
}

void FlyView::setClickEnabled(bool clickEnabled) {
    this->isClickEnabled = clickEnabled;
}

void FlyView::dance() {
    if(!existB_E()) {
        PacToast::makeText("請檢查起點和終點是否都已設定");
        return;
    }
    
    //设置数字不可点击了
    setClickEnabled(false);
    
    //隐藏提示图片
    tipView->setVisible(false);
    
    int beginNum = beginBtn->getTag();
    int endNum = endBtn->getTag();
    
    //创建指示箭头
    addDirectArrow(beginNum, endNum);
    
    //执行动画---蜜蜂和箭头同时执行
    float playTime = abs(beginNum - endNum) * 0.4f; //动画播放时间
    //蜜蜂动画
    auto beeAction = Sequence::create(MoveTo::create(playTime, Vec2(getXByNum(endNum), beeSprite->getPositionY())), DelayTime::create(0.5f),  CallFunc::create([this, beginNum, endNum](){
        
        this->unschedule("updateColor");
        
        beginBtn->setVisible(false);
        endBtn->setVisible(false);
    }), NULL);
    //箭头动画
    auto arrowAction = TargetedAction::create(arrowImage, ScaleTo::create(playTime, 1.0f));
    //播放同步动画
    auto spawnAction = Spawn::create(beeAction, arrowAction, NULL);
    beeSprite->runAction(spawnAction);
    
    //数字格子变色
    updateColor(beginNum);
    schedule([this](float dt){
        this->updateColor(getNumByX(beeSprite->getPositionX()));
    }, playTime / abs(beginNum - endNum), "updateColor");

}

void FlyView::updateColor(int num) {
    Button* numBtn = static_cast<Button*>(getChildByTag(num));
    if(numBtn) {
        numBtn->setColor(Color3B(255, 204, 204));
    }
}

void FlyView::addDirectArrow(int beginNum, int endNum) {
    
    Vec2 position = Vec2(getXByNum(beginNum), getContentSize().height + 13);
    Size size = Size(abs(beginNum - endNum) * FLY_GRID_WIDTH, 20);
    
    arrowImage = ImageView::create("blue_arrow.png");
    arrowImage->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    arrowImage->setPosition(position);
    arrowImage->setContentSize(size);
    arrowImage->setScale9Enabled(true);
    arrowImage->setScaleX(0);
    if(beginNum < endNum) { //从左到右
        arrowImage->setCapInsets(Rect(10, 0, 10, 10));
    } else { //从右到左
        arrowImage->setCapInsets(Rect(size.width - 10, 0, 10, 10));
        arrowImage->setRotation(180);
    }
    this->addChild(arrowImage);
}

bool FlyView::existB_E() {
    return beginBtn->isVisible() && endBtn->isVisible();
}

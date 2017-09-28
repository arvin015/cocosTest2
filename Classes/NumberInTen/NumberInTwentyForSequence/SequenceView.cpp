//
//  SequenceView.cpp
//  CocosTest2
//
//  Created by arvin on 2017/9/27.
//
//

#include "SequenceView.h"
#include "ui/UIText.h"
#include "ui/UIImageView.h"
#include "UISpinner.h"
#include "PacToast.h"

USING_NS_CC;
using namespace ui;
using namespace std;

const int DIRECT_ARROW_MOVE_BY = 20; //指向箭头跳跃距离

const string Children[] = {"child_1.png", "child_2.png", "child_3.png", "child_4.png", "child_5.png", "child_6.png", "child_7.png", "child_8.png", "child_9.png", "child_10.png", "child_11.png", "child_12.png"};

//根据编号获取X坐标
float getXByNum1(int num) {
    return (num - 1) * SEQUENCE_GRID_WIDTH + SEQUENCE_GRID_WIDTH / 2;
}

//根据X坐标获取对应的编号
int getNumByX1(float x, int totalNum) {
    return (int)ceil((x * 1.0 / (totalNum * SEQUENCE_GRID_WIDTH) * totalNum));
}

//随机一组数据
std::vector<int> randVector(int num) {
    std::vector<int> result;
    result.clear();
    result.reserve(num);
    
    srand((int)time(0));
    
    for(int i = 0; i < num; i++) {
        result.push_back(i);
    }
    
    int p1;
    int p2;
    int temp;
    
    while(--num) {
        p1 = num;
        p2 = rand() % num;
        temp = result[p1];
        result[p1] = result[p2];
        result[p2] = temp;
    }
    return result;
}

SequenceView::SequenceView()
: eventListener(nullptr)
, tipView(nullptr)
, arrowSprite(nullptr)
, deleteBtn(nullptr)
, isMove(false)
, isSelect(false)
, spinner(nullptr)
, selectNum(-1)
, beginBtn(nullptr)
, endBtn(nullptr)
, isClickEnabled(true)
, arrowImage(nullptr)
, totalNum(10){
    
}

SequenceView::~SequenceView() {
    
}

void SequenceView::onEnter() {
    Layout::onEnter();
    
    eventListener = EventListenerTouchOneByOne::create();
    eventListener->onTouchBegan = CC_CALLBACK_2(SequenceView::onTouchBegan, this);
    eventListener->onTouchMoved = CC_CALLBACK_2(SequenceView::onTouchMoved, this);
    eventListener->onTouchEnded = CC_CALLBACK_2(SequenceView::onTouchEnded, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, this);
}

void SequenceView::onExit() {
    
    if(eventListener) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(eventListener);
    }
    
    Layout::onExit();
}

SequenceView* SequenceView::create(int totalNum, int targetNum) {
    SequenceView* view = new (std::nothrow)SequenceView();
    if(view && view->init(totalNum, targetNum)) {
        view->autorelease();
        return view;
    }
    CC_SAFE_FREE(view);
    
    return nullptr;
}

bool SequenceView::init(int totalNum, int targetNum) {
    if(!Layout::init()) {
        return false;
    }
    
    this->totalNum = totalNum;
    this->targetNum = targetNum;
    
    setContentSize(Size(SEQUENCE_GRID_WIDTH * totalNum, SEQUENCE_GRID_HEIGHT));
    
    //创建提示
    tipView = Layout::create();
    tipView->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    tipView->setPosition(Vec2(0, getContentSize().height + 80));
    tipView->setBackGroundImage("tip_text_bg.png");
    tipView->setBackGroundImageScale9Enabled(true);
    this->addChild(tipView);
    
    Text* tipText = Text::create();
    tipText->setTextColor(Color4B::WHITE);
    tipText->setFontSize(24);
    tipText->setString("請點按小朋友選擇數數的起點和終點");
    
    tipView->addChild(tipText);
    tipView->setContentSize(Size(tipText->getContentSize().width + 8, tipText->getContentSize().height + 6));
    tipView->requestDoLayout();
    
    tipText->setPosition(Vec2(tipView->getContentSize().width / 2, tipView->getContentSize().height / 2));
    
    //创建删除
    deleteBtn = Button::create("button_bar_delete.png");
    deleteBtn->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    deleteBtn->setPosition(Vec2(getContentSize().width, getContentSize().height));
    this->addChild(deleteBtn);
    
    //创建一组随机数
    rndNumbers = randVector(totalNum);
    
    //创建对象队列
    for(int i = 0; i < totalNum; i++) {
        Sprite* objSprite = Sprite::create(Children[rndNumbers[i]]);
        objSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
        objSprite->setPosition(Vec2(i * SEQUENCE_GRID_WIDTH, getContentSize().height / 2));
        objSprite->setTag(i + 1);
        this->addChild(objSprite);
        childList.pushBack(objSprite);
        
        Text* numText = Text::create();
        numText->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        numText->setPosition(Vec2(objSprite->getContentSize().width / 2, objSprite->getContentSize().height + 5));
        numText->setTextColor(Color4B(255, 133, 136, 255));
        numText->setFontSize(20);
        numText->setScale(0);
        objSprite->addChild(numText);
        numList.pushBack(numText);
    }
    
    //指向箭头
    arrowSprite = Sprite::create("direct_arrow.png");
    arrowSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    arrowSprite->setPosition(Vec2(getXByNum1(totalNum - targetNum), SEQUENCE_GRID_HEIGHT + 10));
    this->addChild(arrowSprite);
    
    //创建起点
    beginBtn = Button::create("image_begin.png");
    beginBtn->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    beginBtn->setVisible(false);
    beginBtn->setTag(-1); //beginBtn关联的数字
    beginBtn->setPositionY(-10);
    beginBtn->addClickEventListener([this](Ref* pSender){
        this->onClickObjHandle(beginBtn->getTag());
    });
    this->addChild(beginBtn);
    
    //创建终点
    endBtn = Button::create("image_end.png");
    endBtn->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    endBtn->setVisible(false);
    endBtn->setTag(-1); //endBtn关联的数字
    endBtn->setPositionY(-10);
    endBtn->addClickEventListener([this](Ref* pSender){
        this->onClickObjHandle(endBtn->getTag());
    });
    this->addChild(endBtn);
    
    return true;
}

bool SequenceView::onTouchBegan(Touch* touch, Event* event) {
    
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

void SequenceView::onTouchMoved(Touch* touch, Event* event) {
    
    if(touch->getID() != 0) {
        return;
    }
    
    if(isSelect) {
        if(abs(touch->getDelta().x) > 2.0f || abs(touch->getDelta().y) > 2.0f) {
            isMove = true;
        }
    }
}

void SequenceView::onTouchEnded(Touch* touch, Event* event) {
    
    if(isSelect && !isMove) { //点击事件
        float x = convertToNodeSpace(touch->getLocation()).x;
        onClickObjHandle(getNumByX1(x, totalNum));
    }
    
    isSelect = false;
    isMove = false;
}

void SequenceView::onClickObjHandle(int num) {
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
        spinner->setParameters(list, SEQUENCE_GRID_WIDTH + 10, 30);
        Director::getInstance()->getRunningScene()->addChild(spinner);
    }
    
    spinner->setVisible(true);
    
    selectNum = num;
    
    spinner->setPosition(convertToWorldSpace(Vec2(getXByNum1(selectNum), -10)));
}

void SequenceView::onItemSelected(int index, const string &content) {
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
        
        float x = getXByNum1(selectNum);
        
        curBtn->setVisible(true);
        curBtn->setTag(selectNum);
        curBtn->setPositionX(x);
        
        selectNum = -1;
    }
}

void SequenceView::setClickObjEnabled(bool clickEnabled) {
    this->isClickEnabled = clickEnabled;
}

bool SequenceView::checkCanDance() {
    if(!existB_E()) {
        PacToast::makeText("請檢查起點和終點是否都已設定");
        return false;
    }
    
    return true;
}

void SequenceView::dance() {

    //设置数字不可点击了
    setClickObjEnabled(false);
    
    //隐藏提示图片
    tipView->setVisible(false);
    
    //隐藏起点终点
    beginBtn->setVisible(false);
    endBtn->setVisible(false);
    
    int beginNum = beginBtn->getTag();
    int endNum = endBtn->getTag();
    
    //创建指示箭头
    addDirectArrow(beginNum, endNum);
    
    //执行动画---数数和箭头同时执行
    float playTime = (abs(beginNum - endNum) + 1) * 0.4f; //动画播放时间
    //指向箭头动画
    auto easeBounceOutAction = EaseBounceOut::create(MoveBy::create(0.5f, Vec2(0, DIRECT_ARROW_MOVE_BY)));
    auto diectArrowAction = TargetedAction::create(arrowSprite, easeBounceOutAction);
    //箭头动画
    auto arrowAction = TargetedAction::create(arrowImage, ScaleTo::create(playTime, 1.0f));
    //数字动画
    Vector<FiniteTimeAction*> numActionList;
    int b = beginNum > endNum ? endNum : beginNum;
    int e = beginNum > endNum ? beginNum : endNum;
    int count = 1;
    for(int i = b; i < e + 1; i++) {
        Text* numText = numList.at(beginNum > endNum ? (e - count) : (i - 1));
        if(numText) {
            numText->setString(Value(count).asString());
            auto action = Sequence::create(ScaleTo::create(0.4f, 1), NULL);
            auto targetAction = TargetedAction::create(numText, action);
            numActionList.pushBack(targetAction);
            count++;
        }
    }
    auto numAction = Sequence::create(numActionList);
    //播放同步动画
    auto spawnAction = Spawn::create(numAction, arrowAction, NULL);
    //播放动画
    arrowImage->runAction(Sequence::create(diectArrowAction, spawnAction, CallFunc::create([this, beginNum, endNum](){
//        beginBtn->setVisible(false);
//        endBtn->setVisible(false);
    }), NULL));
}

void SequenceView::fromJson(const rapidjson::Value &json) {
    int begin = json["begin"].GetInt();
    int end = json["end"].GetInt();
    
    if(begin != -1) {
        beginBtn->setVisible(true);
        beginBtn->setTag(begin);
        
        float x = getXByNum1(begin);
        
        beginBtn->setPositionX(x);
    }
    
    if(end != -1) {
        endBtn->setVisible(true);
        endBtn->setTag(end);
        endBtn->setPositionX(getXByNum1(end));
    }
    
    //替换对象图片
    if(json.HasMember("rndNumbers")) {
        
        rndNumbers.clear();
        
        const rapidjson::Value &randArray = json["rndNumbers"];
        
        for(rapidjson::SizeType i = 0; i < randArray.Size(); i++) {
            rndNumbers.push_back(randArray[i].GetInt());
            
            Sprite* objSprite = childList.at(i);
            if(objSprite) {
                objSprite->setTexture(Children[randArray[i].GetInt()]);
            }
        }
    }
    
    //舞动处理
    bool isDanced = json["isDanced"].GetBool();
    if(isDanced) {
        addDirectArrow(begin, end);
        beginBtn->setVisible(false);
        endBtn->setVisible(false);
        setClickObjEnabled(false);
        tipView->setVisible(false);
        arrowSprite->setPositionY(arrowSprite->getPositionY() + DIRECT_ARROW_MOVE_BY);
        
        int b = begin > end ? end : begin;
        int e = begin > end ? begin : end;
        
        int count = 1;
        for(int i = b; i < e + 1; i++) {
            Text* numText = numList.at(begin > end ? (e - count) : (i - 1));
            if(numText) {
                numText->setScale(1);
                numText->setString(Value(count).asString());
                count++;
            }
        }
    }
}

void SequenceView::toJson(rapidjson::Document &json) {
    json.AddMember("begin", beginBtn->getTag(), json.GetAllocator());
    json.AddMember("end", endBtn->getTag(), json.GetAllocator());
    
    rapidjson::Value randArray;
    randArray.SetArray();
    for(int num : rndNumbers) {
        randArray.PushBack(num, json.GetAllocator());
    }
    json.AddMember("rndNumbers", randArray, json.GetAllocator());
}

void SequenceView::addDirectArrow(int beginNum, int endNum) {
    
    Vec2 position = Vec2(getXByNum1(beginNum), -20);
    Size size = Size(abs(beginNum - endNum) * SEQUENCE_GRID_WIDTH, 20);
    
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

bool SequenceView::existB_E() {
    return beginBtn->isVisible() && endBtn->isVisible();
}

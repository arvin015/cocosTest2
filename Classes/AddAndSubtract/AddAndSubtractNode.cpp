//
//  AddAndSubtractNode.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/12/5.
//

#include "AddAndSubtractNode.h"
#include "UILineNumbers.h"
#include "CatSprite.h"
#include "PacToast.h"

USING_NS_CC;
using namespace ui;
using namespace std;

AddAndSubtractNode::AddAndSubtractNode()
: lineNumbers(nullptr)
, catSprite(nullptr)
, calText(nullptr)
, minValue(1)
, maxValue(18)
, beginNum(-1)
, resultNum(-1)
, LittleAnimTime(0.5f)
, BigAnimTime(1.5f)
, DelayTimeLong(0.5f)
, DelayTimeShort(0.2f){

}

AddAndSubtractNode::~AddAndSubtractNode() {

}

void AddAndSubtractNode::onEnter() {
    Node::onEnter();

    lineNumbers = LineNumbers::create();
    lineNumbers->setContentSize(getContentSize());
    lineNumbers->setValueRange(minValue, maxValue);
    lineNumbers->setOnClickBlockCallback([this](int num) {
        responseBlockClick(num);
    });
    this->addChild(lineNumbers);

    setContentSize(Size(getContentSize().width, lineNumbers->getContentSize().height));
}

void AddAndSubtractNode::onExit() {
    Node::onExit();
}

bool AddAndSubtractNode::init() {
    if (!Node::init()) {
        return false;
    }
    return true;
}

bool AddAndSubtractNode::checkCanDance() {
    if (!lineNumbers->isSelectBlock(beginNum)) {
        PacToast::makeText("請選中一個數字");
        return false;
    }
    if (!isSetResult(resultNum)) {
        PacToast::makeText("請拖拉小貓到結果位置");
        return false;
    }

    return true;
}

void AddAndSubtractNode::dance() {

    lineNumbers->setNumBtnEnabled(false);
    catSprite->setIsTouchEnabled(false);

    bool isAdd = beginNum < resultNum; //是否加法

    //变色
    lineNumbers->updateBlockColor(true, resultNum);

    //小箭头
    if (isAdd) { //加法
        for (int i = beginNum; i < resultNum; i++) {
            float x = lineNumbers->getXByNum(i);
            float y = getContentSize().height;
            DrawNode* arrowNode = createArrow(Size(lineNumbers->getBlockSize(), lineNumbers->getBlockSize()), isAdd);
            arrowNode->setPosition(Vec2(x, y));
            this->addChild(arrowNode);
            playScaleAnim(arrowNode, (i - beginNum + 1) * (LittleAnimTime + DelayTimeShort), nullptr);
        }
    } else { //减法
        for (int i = beginNum; i > resultNum; i--) {
            float x = lineNumbers->getXByNum(i);
            float y = getContentSize().height;
            DrawNode* arrowNode = createArrow(Size(lineNumbers->getBlockSize(), lineNumbers->getBlockSize()), isAdd);
            arrowNode->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
            arrowNode->setPosition(Vec2(x, y));
            this->addChild(arrowNode);
            playScaleAnim(arrowNode, (beginNum - i + 1) * (LittleAnimTime + DelayTimeShort), nullptr);
        }
    }

    scheduleOnce([this, isAdd](float d) { //所有小箭头动画播放完毕
        //创建大箭头
        float x = isAdd ? lineNumbers->getXByNum(beginNum) : lineNumbers->getXByNum(resultNum);
        DrawNode* bigArrowNode = createArrow(Size(abs(resultNum - beginNum) * lineNumbers->getBlockSize(), 10 * lineNumbers->getBlockSize()), isAdd);
        bigArrowNode->setPosition(x, getContentSize().height);
        this->addChild(bigArrowNode);
        playJumpAnim(bigArrowNode, [this, bigArrowNode, isAdd]() { //动画结束回调
            addCalText(bigArrowNode, Vec2(bigArrowNode->getContentSize().width / 2, 115), abs(beginNum - resultNum), isAdd);
            playScaleAnim(calText, DelayTimeShort, [this]() { //文本动画结束，舞动完成
//                lineNumbers->setNumBtnEnabled(false);
//                catSprite->setIsTouchEnabled(false);
            });
        }); //播放动画
    }, (abs(beginNum - resultNum) + 1) * (LittleAnimTime + DelayTimeShort) + DelayTimeLong, "arrowSchedule");
}

void AddAndSubtractNode::fromJson(const rapidjson::Value &json) {
    if (json.IsObject()) {
        beginNum = json["beginNum"].GetInt();
        resultNum = json["resultNum"].GetInt();

        if (beginNum != -1) {
            lineNumbers->setBlockSelected(true, beginNum);
            responseBlockClick(beginNum);
        }
        if (resultNum != -1) {
            if (catSprite) {
                catSprite->setPositionX(lineNumbers->getXByNum(resultNum));
                catSprite->setTag(resultNum);
            }
        }
        
        if (json["isDanced"].GetBool()) {
            LittleAnimTime = 0.001f;
            BigAnimTime = 0.001f;
            DelayTimeLong = 0;
            DelayTimeShort = 0;
            dance();
        }
    }
}


void AddAndSubtractNode::toJson(rapidjson::Document &json) {

    int beginNum, resultNum;
    if (!lineNumbers->isSelectBlock(beginNum)) {
        beginNum = -1;
    }
    if (!isSetResult(resultNum)) {
        resultNum = -1;
    }

    json.AddMember("beginNum", beginNum, json.GetAllocator());
    json.AddMember("resultNum", resultNum, json.GetAllocator());
}

void AddAndSubtractNode::setValueRange(int minValue, int maxValue) {
    this->minValue = minValue;
    this->maxValue = maxValue;
}

void AddAndSubtractNode::responseBlockClick(int num) {

    float x = lineNumbers->getXByNum(num);

    if (catSprite == nullptr) {
        catSprite = CatSprite::create("cat.png");
        catSprite->setPosition(Vec2(x, lineNumbers->getContentSize().height + 40));
        catSprite->setOnTouchUpCallback([this](float x){
            int num = lineNumbers->getNumByX(x);
            float rx = lineNumbers->getXByNum(num);

            int resultNum;
            lineNumbers->isSelectBlock(resultNum);
            if (num == resultNum) { //弹回
                catSprite->setPositionX(lineNumbers->getXByNum(catSprite->getTag()));
                return;
            }

            catSprite->setPositionX(rx);
            catSprite->setTag(num);
        });
        this->addChild(catSprite);
    } else {
        catSprite->playAnim(x);
    }
    catSprite->setTag(num);
}

bool AddAndSubtractNode::isSetResult(int &num) {
    if (catSprite == nullptr) {
        return false;
    }

    num = catSprite->getTag();

    int resultNum;
    lineNumbers->isSelectBlock(resultNum);
    if (num == resultNum) {
        return false;
    }

    return true;
}

DrawNode* AddAndSubtractNode::createArrow(const Size &contentSize, bool isAdd) {
    DrawNode* drawNode = DrawNode::create();
    drawNode->setContentSize(contentSize);
    Vec2 origin = Vec2::ZERO;
    Vec2 control = Vec2(contentSize.width / 2, contentSize.height / 2);
    Vec2 destination = Vec2(contentSize.width, 0);
    drawNode->drawQuadBezier(origin, control, destination, 100, Color4F::BLUE);
    return drawNode;
}

void AddAndSubtractNode::addCalText(Node* parent, const Vec2 &position, int num, bool isAdd) {
    calText = Text::create();
    calText->setPosition(position);
    calText->setFontSize(22);
    calText->setTextColor(Color4B::BLACK);
    calText->setString(StringUtils::format("%s%d", isAdd ? "+" : "-", num));
    calText->setTextHorizontalAlignment(TextHAlignment::CENTER);
    calText->setTextVerticalAlignment(TextVAlignment::CENTER);
    parent->addChild(calText);
}

void AddAndSubtractNode::playScaleAnim(Node* targetNode, float delayTime, function<void()> animEndCallback) {
    targetNode->setScale(0);
    auto scaleAnim = Sequence::create(DelayTime::create(delayTime), ScaleTo::create(LittleAnimTime, 1), CallFunc::create([this, animEndCallback](){
        if (animEndCallback) {
            animEndCallback();
        }
    }), NULL);
    targetNode->runAction(scaleAnim);
}

void AddAndSubtractNode::playJumpAnim(Node* targetNode, function<void()> animEndCallback) {
    targetNode->setPositionY(targetNode->getPositionY() + 60);
    auto anim = Sequence::create(EaseBounceOut::create(MoveBy::create(BigAnimTime, Vec2(0, -60))), CallFunc::create([this, animEndCallback]() {
        if (animEndCallback) {
            animEndCallback();
        }
    }), NULL);
    targetNode->runAction(anim);
}

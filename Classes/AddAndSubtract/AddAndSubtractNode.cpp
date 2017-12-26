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
#include "CommonUtils.h"

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
, DelayTimeShort(0.2f)
, totalDancedNum(2)
, isRecord(false) {

}

AddAndSubtractNode::~AddAndSubtractNode() {

}

void AddAndSubtractNode::onEnter() {
    Node::onEnter();

    createLineNumber();

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
    if (totalDancedNum <= 0) {
        PacToast::makeText("無法再作答下去");
        return false;
    }
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

    lineNumbers->setDrawCursorVisible(false); //隐藏光标
    lineNumbers->setNumBtnEnabled(false); //舞动了，数字不可点击了

    bool isAdd = beginNum < resultNum; //是否加法

    //记录舞动过的计算步骤
    CalStep step = CalStep(beginNum, resultNum, isAdd ? 1 : 2, true);
    recordCalStepList.push_back(step);

    //变色
    lineNumbers->updateBlockColor(true, resultNum);

    //小箭头
    if (isAdd) { //加法
        for (int i = beginNum; i < resultNum; i++) {
            DrawNode* arrowNode = createArrow(Size(lineNumbers->getBlockSize(), lineNumbers->getBlockSize() / 2), isAdd);
            arrowNode->setPosition(Vec2(lineNumbers->getXByNum(i), getContentSize().height));
            this->addChild(arrowNode);
            if (!isRecord) { //新活动播放动画
                playScaleAnim(arrowNode, (i - beginNum + 1) * (LittleAnimTime + DelayTimeShort), LittleAnimTime, nullptr);
            }
        }
    } else { //减法
        for (int i = beginNum; i > resultNum; i--) {
            DrawNode* arrowNode = createArrow(Size(lineNumbers->getBlockSize(), lineNumbers->getBlockSize() / 2), isAdd);
            arrowNode->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
            arrowNode->setPosition(Vec2(lineNumbers->getXByNum(i), 0));
            this->addChild(arrowNode);
            if (!isRecord) { //新活动播放动画
                playScaleAnim(arrowNode, (beginNum - i + 1) * (LittleAnimTime + DelayTimeShort), LittleAnimTime, nullptr);
            }
        }
    }
    
    //创建大箭头
    float x = lineNumbers->getXByNum(beginNum);
    DrawNode* bigArrowNode = createArrow(Size(abs(resultNum - beginNum) * lineNumbers->getBlockSize(), 5 * lineNumbers->getBlockSize()), isAdd);
    if (!isAdd) {
        bigArrowNode->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
    }
    bigArrowNode->setPosition(x, isAdd ? getContentSize().height : 0);
    this->addChild(bigArrowNode);

    //创建文本
    addCalText(bigArrowNode, Vec2(bigArrowNode->getContentSize().width / 2, isAdd ? 115 : 90), abs(beginNum - resultNum), isAdd);
    
    //新活动播放动画
    if (!isRecord) {
        float littleArrowPlayAnimTime = (abs(beginNum - resultNum) + 1) * (LittleAnimTime + DelayTimeShort); //所有小箭头播放完用时
        playScaleAnim(bigArrowNode, littleArrowPlayAnimTime + DelayTimeLong, BigAnimTime, nullptr); //播放大箭头
        float bigArrowPlayAnimTime = DelayTimeLong + BigAnimTime; //大箭头播放完用时
        playScaleAnim(calText, littleArrowPlayAnimTime + bigArrowPlayAnimTime + DelayTimeShort, LittleAnimTime, [this](){ //舞动结束
        });
    }
    
    //舞动结束后处理
    lineNumbers->setBlockSelected(true, resultNum); //将结果值设为开始值
    
    totalDancedNum--; //总舞动次数减一
    if (totalDancedNum <= 0) {
        catSprite->setIsTouchEnabled(false); //禁止小猫移动
    }
}

void AddAndSubtractNode::fromJson(const rapidjson::Value &json) {
    
    log("---AddAndSubtractNode::fromJson = %s", getStringFromJson(json).c_str());

    reset(); //重置

    if (json.IsObject()) {

        if (!json.HasMember("calStep")) return;

        const rapidjson::Value &stepArray = json["calStep"];
        if (!stepArray.IsArray()) return;

        isRecord = true;

        for (int i = 0; i < stepArray.Size(); i++) {
            const rapidjson::Value &stepObj = stepArray[i];
            if (stepObj.IsObject()) {
                CalStep calStep;
                calStep.fromJson(stepObj);

                beginNum = calStep.num1;
                resultNum = calStep.num2;

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
                
                if (calStep.isDanced) {
                    dance();
                }
            }
        }

        isRecord = false;
    }
}

void AddAndSubtractNode::toJson(rapidjson::Document &json) {

    recordCalStep();

    if (recordCalStepList.size() < 1) return;

    rapidjson::Value stepArray(rapidjson::kArrayType);

    for (CalStep calStep : recordCalStepList) {
        rapidjson::Document stepObj(&json.GetAllocator());
        stepObj.SetObject();
        calStep.toJson(stepObj);
        stepArray.PushBack(stepObj, json.GetAllocator());
    }

    json.AddMember("calStep", stepArray, json.GetAllocator());

    log("---AddAndSubtractNode::toJson = %s", getStringFromJson(json).c_str());
}

void AddAndSubtractNode::setValueRange(int minValue, int maxValue) {
    this->minValue = minValue;
    this->maxValue = maxValue;
}

void AddAndSubtractNode::recordCalStep() {
    if (totalDancedNum <= 0) return;

    int bNum = -1;
    int rNum = -1;
    
    bool hasB = lineNumbers->isSelectBlock(bNum);
    bool hasR = isSetResult(rNum);
    
    if (!hasB && !hasR) return;

    if (resultNum == rNum) return; //舞动过一次，没有再移动小猫

    //记录未舞动的计算步骤
    CalStep step = CalStep(bNum, rNum, bNum > rNum ? 2 : 1, false);
    recordCalStepList.push_back(step);
}

void AddAndSubtractNode::createLineNumber() {

    lineNumbers = LineNumbers::create();
    lineNumbers->setContentSize(getContentSize());
    lineNumbers->setValueRange(minValue, maxValue);
    lineNumbers->setOnClickBlockCallback([this](int num) {
        responseBlockClick(num);
    });
    this->addChild(lineNumbers);
}

void AddAndSubtractNode::responseBlockClick(int num) {

    float x = lineNumbers->getXByNum(num);

    if (catSprite == nullptr) {
        catSprite = CatSprite::create("cat.png");
        catSprite->setPosition(Vec2(x, lineNumbers->getContentSize().height + 40));
        catSprite->setOnTouchDownCallback([this](float x){
            lineNumbers->setDrawCursorVisible(true);
            lineNumbers->setDrawCursorPosition(x);
        });
        catSprite->setOnTouchMoveCallback([this](float x){
            lineNumbers->setDrawCursorPosition(x);
        });
        catSprite->setOnTouchUpCallback([this](float x){
            int num = lineNumbers->getNumByX(x);
            float rx = lineNumbers->getXByNum(num);

            int resultNum;
            lineNumbers->isSelectBlock(resultNum);
            if (num == resultNum) { //弹回
                catSprite->setPositionX(lineNumbers->getXByNum(catSprite->getTag()));
                lineNumbers->setDrawCursorVisible(false); //隐藏光标
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
    DrawNode* drawNode = DrawNode::create(4);
    drawNode->setContentSize(contentSize);
    Vec2 origin = isAdd ? Vec2::ZERO : Vec2(0, contentSize.height);
    Vec2 control = Vec2(contentSize.width / 2, isAdd ? contentSize.height : 0);
    Vec2 destination = Vec2(contentSize.width, isAdd ? 0 : contentSize.height);
    drawNode->drawQuadBezier(origin, control, destination, 100, Color4F::BLUE);
    int n = contentSize.width / lineNumbers->getBlockSize();
    if (isAdd) {
        drawNode->drawLine(destination, destination + Vec2(n == 1 ? -3 : -3 + 1.0 / n * 12 , n == 1 ? 6 : 6 - 1.0 / n * 6), Color4F::BLUE);
        drawNode->drawLine(destination, destination + Vec2(-6, 2), Color4F::BLUE);
    } else {
        drawNode->drawLine(origin, Vec2(n == 1 ? 3 : 3 - 1.0 / n * 12 , origin.y - (n == 1 ? 6 : 6 + 1.0 / n * 6)), Color4F::BLUE);
        drawNode->drawLine(origin, Vec2(6, origin.y - 2), Color4F::BLUE);
    }
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

void AddAndSubtractNode::playScaleAnim(Node* targetNode, float delayTime, float playAnimTime, function<void()> animEndCallback) {
    targetNode->setScale(0);
    auto scaleAnim = Sequence::create(DelayTime::create(delayTime), ScaleTo::create(playAnimTime, 1), CallFunc::create([this, animEndCallback](){
        if (animEndCallback) {
            animEndCallback();
        }
    }), NULL);
    targetNode->runAction(scaleAnim);
}

void AddAndSubtractNode::reset() {

    removeAllChildrenWithCleanup(true);

    if (catSprite) {
        catSprite = nullptr;
    }
    if (calText) {
        calText = nullptr;
    }

    beginNum = -1;
    resultNum = -1;
    totalDancedNum = 2;
    isRecord = false;
    recordCalStepList.clear();

    createLineNumber();
}

//
//  AddAndSubtractNode.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/12/5.
//

#ifndef AddAndSubtractNode_h
#define AddAndSubtractNode_h

#include <stdio.h>
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "json/document.h"

struct CalStep { //计算步骤实体
    int num1 = -1; //值1
    int num2 = -1; //值2
    int calType = 0; //计算类型，1：加，2：减
    bool isDanced = false; //是否舞动

    CalStep() {}

    CalStep(int num1, int num2, int calType, bool isDanced) {
        this->num1 = num1;
        this->num2 = num2;
        this->calType = calType;
        this->isDanced = isDanced;
    }

    void fromJson(const rapidjson::Value &json) {
        this->num1 = json.HasMember("num1") ? json["num1"].GetInt() : -1;
        this->num2 = json.HasMember("num2") ? json["num2"].GetInt() : -1;
        this->calType = json.HasMember("calType") ? json["calType"].GetInt() : 0;
        this->isDanced = json.HasMember("isDanced") ? json["isDanced"].GetBool() : false;
    }

    void toJson(rapidjson::Document &json) {
        if (!json.IsObject()) return;
        json.AddMember("num1", num1, json.GetAllocator());
        json.AddMember("num2", num2, json.GetAllocator());
        json.AddMember("calType", calType, json.GetAllocator());
        json.AddMember("isDanced", isDanced, json.GetAllocator());
    }
};

class LineNumbers;
class CatSprite;

class AddAndSubtractNode : public cocos2d::Node {

public:

    AddAndSubtractNode();
    virtual ~AddAndSubtractNode();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();

    CREATE_FUNC(AddAndSubtractNode);

    /**
     * 设置值范围
     */
    void setValueRange(int minValue, int maxValue);

    /**
     * 检查是否可舞动
     */
    bool checkCanDance();

    /**
     * 舞动
     */
    void dance();
    
    /**
     * fromJson
     */
    void fromJson(const rapidjson::Value &json);

    /**
     * toJson
     */
    void toJson(rapidjson::Document &json);

private:

    /**
     * 创建数字块
     */
    void createLineNumber();

    /**
     * 响应方块点击处理
     */
    void responseBlockClick(int num);

    /**
     * 是否设定了结果
     */
    bool isSetResult(int &num);

    /**
     * 记录未舞动的计算步骤
     */
    void recordCalStep();

    /**
     * 创建指向箭头
     */
    cocos2d::DrawNode* createArrow(const cocos2d::Size &contentSize, bool isAdd);

    /**
     * 创建加减文本
     */
    void addCalText(cocos2d::Node* parent, const cocos2d::Vec2 &position, int num, bool isAdd);

    /**
     * 播放缩放动画
     */
    void playScaleAnim(cocos2d::Node* targetNode, float delayTime, float playAnimTime, std::function<void()> animEndCallback);

    /**
     * 重设
     */
    void reset();
    
private:
    LineNumbers* lineNumbers;
    CatSprite* catSprite;
    cocos2d::ui::Text* calText;

    int minValue, maxValue;

    int beginNum, resultNum;
    
    float LittleAnimTime; //小箭头播放时间
    float BigAnimTime; //大箭头播放时间
    float DelayTimeLong; //长延时时间
    float DelayTimeShort; //短延时时间
    
    int totalDancedNum; //可舞动总次数
    std::vector<CalStep> recordCalStepList; //记录计算步骤
    bool isRecord; //是否旧活动
};

#endif /* AddAndSubtractNode_h */

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
     * 响应方块点击处理
     */
    void responseBlockClick(int num);

    /**
     * 是否设定了结果
     */
    bool isSetResult(int &num);

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
    void playScaleAnim(cocos2d::Node* targetNode, float delayTime, std::function<void()> animEndCallback);

    /**
     * 播放跳跃动画
     */
    void playJumpAnim(cocos2d::Node* targetNode, std::function<void()> animEndCallback);

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
};

#endif /* AddAndSubtractNode_h */

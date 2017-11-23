//
//  UINumberAddSubtract.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/11/22.
//

#ifndef UINumberAddSubtract_h
#define UINumberAddSubtract_h

#include <stdio.h>
#include "cocos2d.h"
#include "ui/UIText.h"
#include "ui/UILayout.h"

class NumberAddSubtract : public cocos2d::ui::Layout {

public:

    NumberAddSubtract();
    virtual ~NumberAddSubtract();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();

    CREATE_FUNC(NumberAddSubtract);

    /**
     * 设置值
     */
    void setValues(const std::vector<std::string> &valueList);

    /**
     * 设置文本颜色
     */
    void setTextColor(const cocos2d::Color4B &textColor);

    /**
     * 设置文本大小
     */
    void setTextSize(int textSize);

    /**
     * 设置是否需要文本边框
     */
    void setTextBorderVisible(bool visible);

    /**
     * 设置文本边框颜色
     */
    void setTextBorderColor(const cocos2d::Color4F &borderColor);

    /**
     * 设置文本内容
     */
    void setTextContent(const std::string &content);

    /**
     * 设置加减图片
     */
    void setAddSubtractImageName(const std::string &addImageName, const std::string &subtractImageName);

    /**
     * 是否可Touch
     */
    void setIsTouchEnabled(bool enabled) {
        this->touchEnabled = enabled;
    }

private:

    bool onTouchDown(cocos2d::Touch* t, cocos2d::Event* e);
    void onTouchMove(cocos2d::Touch* t, cocos2d::Event* e);
    void onTouchUp(cocos2d::Touch* t, cocos2d::Event* e);

    /**
     * 改变文本显示
     */
    void updateShowText(int calValue);

    /**
     * 绘制文本边框
     */
    void drawTextBorder();

    /**
     * 开始计时---检测是否长按
     */
    void startTimerForLongClick(std::function<void()> onLongClick);

    /**
     * 取消计时---检测是否长按
     */
    void stopTimerForLongClick();

    /**
     * 开始长按操作
     */
    void startLongClickWork(std::function<void()> onReachInterval);

    /**
     * 取消长按操作
     */
    void cancelLongClickWork();

private:

    cocos2d::EventListenerTouchOneByOne* eventListener;

    cocos2d::Sprite* subtractSprite;
    cocos2d::Sprite* addSprite;
    cocos2d::ui::Text* showText;
    cocos2d::DrawNode* borderDraw;

    std::vector<std::string> valueList;
    std::string addImageName;
    std::string substractImageName;
    bool textBorderVisible;
    cocos2d::Color4F textBorderColor;
    int textSize;
    cocos2d::Color4B textColor;

    bool isLongClick; //是否长按
    bool isMove; //是否移动了
    bool touchEnabled;
    int curIndex;
    int selectFlag; //选中加或减标识，1：减；2：加
};

#endif /* UINumberAddSubtract_h */

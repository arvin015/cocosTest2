//
//  UILineNumbers.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/12/5.
//

#ifndef UILineNumbers_h
#define UILineNumbers_h

#include <stdio.h>
#include "cocos2d.h"
#include "ui/CocosGUI.h"

class LineNumbers : public cocos2d::Node {

public:

    typedef std::function<void(int)> OnClickBlockCallback; //点击方块回调

    LineNumbers();
    virtual ~LineNumbers();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();

    CREATE_FUNC(LineNumbers);

    /**
     * 设置值范围
     */
    void setValueRange(int minValue, int maxValue);

    /**
     * 设置未选中、选中颜色
     */
    void setBlockColor(const cocos2d::Color3B &normalColor, const cocos2d::Color3B &selectColor);

    /**
     * 设置指定块选中或未选中
     */
    void setBlockSelected(bool isSelect, int num);

    /**
     * 更新指定块颜色
     */
    void updateBlockColor(bool isSelect, int num);

    /**
     * 设置数字按钮是否可点击
     */
    void setNumBtnEnabled(bool enabled);

    /**
     * 获取是否选中
     */
    bool isSelectBlock(int &num);

    /**
     * 设置点击方块回调
     */
    void setOnClickBlockCallback(OnClickBlockCallback clickBlockCallback) {
        this->clickBlockCallback = clickBlockCallback;
    }

    /**
     * 设置光标显示隐藏
     */
    void setDrawCursorVisible(bool visible);

    /**
     * 设置光标的位置
     */
    void setDrawCursorPosition(float x);

    /**
     * 根据编号获取X坐标
     */
    float getXByNum(int num);

    /**
     * 根据X坐标获取对应的编号
     */
    int getNumByX(float x);

    /**
     * 获取每个方块大小
     */
    float getBlockSize() {
        return blockSize;
    }

private:

    int minValue, maxValue;
    int totalNum; //总块数
    cocos2d::Color3B normalColor;
    cocos2d::Color3B selectColor;
    bool enabled; //数字块是否可点击
    float blockSize; //块大小

    cocos2d::DrawNode* drawCursor; //绘制光标

    cocos2d::ui::Button* lastSelectNumBtn; //上一个被选中的数字按钮

    OnClickBlockCallback clickBlockCallback;

};

#endif /* UILineNumbers_h */

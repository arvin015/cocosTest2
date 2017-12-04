//
//  UISquareNode.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/12/4.
//

#ifndef UISquareNode_h
#define UISquareNode_h

#include <stdio.h>
#include "cocos2d.h"
#include "ui/CocosGui.h"

class SquareNode : public cocos2d::Node {

public:
    
    SquareNode();
    virtual ~SquareNode();
    
    virtual void onEnter();
    virtual void onExit();
    
    virtual bool init();
    
    CREATE_FUNC(SquareNode);

    /**
     * 设置值
     */
    void setValue(int divideNum, int resultNum);

    /**
     * 设置边框颜色
     */
    void setBorderColor(const cocos2d::Color4F &borderColor) {
        this->borderColor = borderColor;
    }

    /**
     * 设置分割线颜色
     */
    void setDivideLineColor(const cocos2d::Color4F &divideLineColor) {
        this->divideLineColor = divideLineColor;
    }

    /**
     * 设置结果块颜色
     */
    void setResultColor(const cocos2d::Color4F &resultColor) {
        this->resultColor = resultColor;
    }
    
private:

    /**
     * 绘制
     */
    void onDraw();

private:

    cocos2d::DrawNode* borderDraw;
    cocos2d::DrawNode* lineDraw;
    cocos2d::DrawNode* resultDraw;

    int divideNum, resultNum;
    cocos2d::Color4F borderColor, divideLineColor, resultColor;

};

#endif /* UISquareNode_h */

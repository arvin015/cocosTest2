//
// Created by Administrator on 2017/12/2.
//

#ifndef PROJ_ANDROID_STUDIO_UICIRCLENODE_H
#define PROJ_ANDROID_STUDIO_UICIRCLENODE_H

#include <stdio.h>
#include "cocos2d.h"
#include "ui/CocosGUI.h"

class CircleNode : public cocos2d::Node {

public:

    CircleNode();
    virtual ~CircleNode();

    virtual void onEnter();
    virtual void onExit();

    virtual bool init();

    CREATE_FUNC(CircleNode);

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
    void setDivideLineColor(const cocos2d::Color3B &divideLineColor) {
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
     * 初始化顶点集合
     */
    void initPeaks();

    /**
     * 绘制主要内容
     */
    void onDraw();

    /**
     * 获取结果份数围成多边形点集合
     */
    void getPolyPoints(std::vector<cocos2d::Vec2> &pointList);

private:

    cocos2d::DrawNode* borderNode; //圆边框
    cocos2d::Node* contentNode;

    int divideNum;
    int resultNum;

    std::vector<cocos2d::Vec2> peakList; //顶点集合
    float perAngle; //每份角度
    float resultAngle; //结果角度
    cocos2d::Color3B divideLineColor; //分割块颜色
    cocos2d::Color4F borderColor, resultColor; //边框颜色、结果块颜色
};


#endif //PROJ_ANDROID_STUDIO_UICIRCLENODE_H

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
    void setValue(int divideNum, int resultNum, const std::string &circleImage);

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

    /**
     * 集合转数组
     */
    void vectorToArray(const std::vector<cocos2d::Vec2> &pointList, cocos2d::Vec2* points);

    /**
     * 已知角求斜率
     */
    float getSlope(float angle);

    /**
     * 求相交点
     */
    void getCrossPoint(float slope, const cocos2d::Vec2 &point, float x, float &y);

    /**
     * 求相交点
     */
    void getCrossPoint1(float slope, const cocos2d::Vec2 &point, float &x, float y);

private:

    int divideNum;
    int resultNum;
    std::string circleImage;

    std::vector<cocos2d::Vec2> peakList; //顶点集合
    float perAngle; //每份角度
    float resultAngle; //结果角度

    cocos2d::Node* contentNode;
};


#endif //PROJ_ANDROID_STUDIO_UICIRCLENODE_H

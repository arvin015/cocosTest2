//
//  UIDrawNodeEx.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2018/12/3.
//

#ifndef UIDrawNodeEx_h
#define UIDrawNodeEx_h

#include <stdio.h>
#include "cocos2d.h"

class DrawNodeEx : public cocos2d::DrawNode {

public:

    DrawNodeEx(GLfloat defaultLineWidth = 2.0f);
    virtual ~DrawNodeEx();

    /**
     * 创建
     */
    static DrawNodeEx* create(GLfloat defaultLineWidth = 2.0f);
    
    /**
     * 初始化
     */
    virtual bool init();

    /**
     * 绘制空心圆弧
     */
    void drawArc(const cocos2d::Vec2 &center, float radius, float startAngle, float sweepAngle, unsigned int segments, const cocos2d::Color4F &color, bool drawLineToCenter = true);
    
    /**
     * 绘制实心圆弧
     */
    void drawSolidArc(const cocos2d::Vec2 &center, float radius, float startAngle, float sweepAngle, unsigned int segments, const cocos2d::Color4F &color, float borderWidth, const cocos2d::Color4F &borderColor);

    /**
     * 绘制带边框的实心圆
     */
    void drawSolidCircleWithBorder(const cocos2d::Vec2 &center, float radius, unsigned int segments, const cocos2d::Color4F &color, float borderWidth, const cocos2d::Color4F &borderColor);

    /**
     * 绘制虚线
     */
    void drawDashLine(const cocos2d::Vec2 &start, const cocos2d::Vec2 &end, float radius, float dashWidth, float spanWidth, const cocos2d::Color4F &color);
};

#endif /* UIDrawNodeEx_h */

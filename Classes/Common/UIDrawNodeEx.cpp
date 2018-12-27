//
//  UIDrawNodeEx.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2018/12/3.
//

#include "UIDrawNodeEx.h"

USING_NS_CC;
using namespace std;

UIDrawNodeEx::UIDrawNodeEx(GLfloat defaultLineWidth)
: DrawNode(defaultLineWidth) {
}

UIDrawNodeEx::~UIDrawNodeEx() {
}

UIDrawNodeEx* UIDrawNodeEx::create(GLfloat defaultLineWidth) {
    auto drawNodeEx = new (std::nothrow) UIDrawNodeEx(defaultLineWidth);
    if (drawNodeEx && drawNodeEx->init()) {
        drawNodeEx->autorelease();
    } else {
        CC_SAFE_DELETE(drawNodeEx);
    }
    return drawNodeEx;
}

bool UIDrawNodeEx::init() {
    return DrawNode::init();
}

void UIDrawNodeEx::drawArc(const Vec2 &center, float radius, float startAngle, float sweepAngle, unsigned int segments, const Color4F &color, bool drawLineToCenter) {
    
    float coef = CC_DEGREES_TO_RADIANS(sweepAngle) / segments;

    Vec2 *vertices = new (std::nothrow) Vec2[segments];
    if (!vertices)
        return;

    float startA = CC_DEGREES_TO_RADIANS(startAngle);

    for (unsigned int i = 0; i < segments; i++) {
        float rads = i * coef;
        GLfloat j = radius * cosf(startA + rads) + center.x;
        GLfloat k = radius * sinf(startA + rads) + center.y;

        vertices[i].x = j;
        vertices[i].y = k;
    }

    if (drawLineToCenter) {
        vertices[segments].x = center.x;
        vertices[segments].y = center.y;
        drawPoly(vertices, segments + 1, true, color);
    } else {
        drawPoly(vertices, segments, false, color);
    }

    CC_SAFE_DELETE_ARRAY(vertices);
}

void UIDrawNodeEx::drawSolidArc(const Vec2 &center, float radius, float startAngle, float sweepAngle, unsigned int segments, const Color4F &color, float borderWidth, const Color4F &borderColor) {
    
    float coef = CC_DEGREES_TO_RADIANS(sweepAngle) / segments;
    
    Vec2 *vertices = new (std::nothrow) Vec2[segments];
    if (!vertices)
        return;
    
    float startA = CC_DEGREES_TO_RADIANS(startAngle);
    
    vertices[0].x = center.x;
    vertices[0].y = center.y;
    
    for (unsigned int i = 1; i <= segments; i++) {
        float rads = i * coef;
        GLfloat j = radius * cosf(startA + rads) + center.x;
        GLfloat k = radius * sinf(startA + rads) + center.y;
        
        vertices[i].x = j;
        vertices[i].y = k;
    }
    drawPolygon(vertices, segments + 1, color, borderWidth, borderColor);
    
    CC_SAFE_DELETE_ARRAY(vertices);
}

void UIDrawNodeEx::drawSolidCircleWithBorder(const Vec2 &center, float radius, unsigned int segments, const Color4F &color, float borderWidth, const Color4F &borderColor) {

    float coef = 2 * M_PI / segments;

    Vec2 *vertices = new (std::nothrow) Vec2[segments];
    if (!vertices)
        return;

    for (unsigned int i = 0; i < segments; i++) {
        float rads = i * coef;
        GLfloat j = radius * cosf(rads) + center.x;
        GLfloat k = radius * sinf(rads) + center.y;

        vertices[i].x = j;
        vertices[i].y = k;
    }
    drawPolygon(vertices, segments, color, borderWidth, borderColor);

    CC_SAFE_DELETE_ARRAY(vertices);
}

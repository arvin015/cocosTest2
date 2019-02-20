//
//  UIDrawNodeEx.cpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2018/12/3.
//

#include "UIDrawNodeEx.h"

USING_NS_CC;
using namespace std;

DrawNodeEx::DrawNodeEx(GLfloat defaultLineWidth)
: DrawNode(defaultLineWidth) {
}

DrawNodeEx::~DrawNodeEx() {
}

DrawNodeEx* DrawNodeEx::create(GLfloat defaultLineWidth) {
    auto drawNodeEx = new (std::nothrow) DrawNodeEx(defaultLineWidth);
    if (drawNodeEx && drawNodeEx->init()) {
        drawNodeEx->autorelease();
    } else {
        CC_SAFE_DELETE(drawNodeEx);
    }
    return drawNodeEx;
}

bool DrawNodeEx::init() {
    return DrawNode::init();
}

void DrawNodeEx::drawArc(const Vec2 &center, float radius, float startAngle, float sweepAngle, unsigned int segments, const Color4F &color, bool drawLineToCenter) {
    
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

void DrawNodeEx::drawSolidArc(const Vec2 &center, float radius, float startAngle, float sweepAngle, unsigned int segments, const Color4F &color, float borderWidth, const Color4F &borderColor) {
    
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

void DrawNodeEx::drawSolidCircleWithBorder(const Vec2 &center, float radius, unsigned int segments, const Color4F &color, float borderWidth, const Color4F &borderColor) {

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

inline float getAngle(const Vec2 &p1, const Vec2 &p2) {
    float angle;
    if (fabsf(p1.x - p2.x) < 1e-4f) {
        angle = M_PI_2;
    } else {
        angle = atanf((p2.y - p1.y) / (p2.x - p1.x));
    }
    return angle;
}

void DrawNodeEx::drawDashLine(const Vec2 &start, const Vec2 &end, float radius, float dashWidth, float spanWidth, const Color4F &color) {
    float totalL = start.distance(end);
    int num = totalL / (dashWidth + spanWidth); //画线条数
    float angle = getAngle(start, end);
    float offsetX = cosf(angle);
    float offsetY = sinf(angle);
    if (start.x > end.x) {
        offsetX = -offsetX;
        offsetY = -offsetY;
    }
    for (int i = 0; i < num; i++) {
        float len1 = (dashWidth + spanWidth) * i;
        float len2 = i == num - 1 ? totalL : len1 + dashWidth;
        float x1 = start.x + offsetX * len1;
        float y1 = start.y + offsetY * len1;
        float x2 = start.x + offsetX * len2;
        float y2 = start.y + offsetY * len2;

        drawSegment(Vec2(x1, y1), Vec2(x2, y2), radius, color);
    }
}



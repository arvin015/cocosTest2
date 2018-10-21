//
//  PolygonView.hpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2018/10/18.
//

#ifndef PolygonView_h
#define PolygonView_h

#include <stdio.h>
#include "cocos2d.h"
#include "Polygon.h"

namespace FoldPaper {

    enum TouchType {
        RORATE,
        MOVE,
        NONE
    };

    class PolygonView : public cocos2d::Node {

    public:

        typedef std::function<void()> OnTouchEndCallback;

        PolygonView();
        ~PolygonView();

        virtual void onEnter();
        virtual void onExit();

        virtual bool init();

        CREATE_FUNC(PolygonView);
        
        void initView();

    public:

        /**
         * 创建方形
         * @param centerPoint
         * @param length
         */
        void createSquare(const cocos2d::Vec2 &centerPoint, float width, float height);

        /**
         * 创建规则的多边形
         * @param centerPoint
         * @param edge
         * @param radius
         */
        void createRegularPolygonWithRadius(const cocos2d::Vec2 &centerPoint, int edge, float radius);

        /**
         * 创建规则的多边形
         * @param centerPoint
         * @param edge
         * @param edgeLength
         */
        void createRegularPolygonWithEdgeLength(const cocos2d::Vec2 &centerPoint, int edge, float edgeLength);

        /**
         * 更新多边形填充色
         * @param fillColor
         */
        void updatePolygonColor(const cocos2d::Color4F &fillColor);

        /**
         * 设置多边形是否选中
         * @param visible
         */
        void setPolygonSelectedState(bool isSelected);

        /**
         * 平移多边形，实际平移PolygonDrawNode
         */
        void movePolygon(const cocos2d::Vec2 &deltaPos);

        /**
         * 旋转多边形，实际旋转PolygonDrawNode
         */
        void rotatePolygon(float deltaDegree);

        /**
         * 获取世界坐标
         */
        cocos2d::Vec2 getPolygonViewWorldPoint(const cocos2d::Vec2 &nodePoint);

        /**
         * 获取多边形内坐标
         */
        cocos2d::Vec2 getPolygonViewNodePoint(const cocos2d::Vec2 &worldPoint);

        /**
         * 设置TouchEnd回调
         */
        void setOnTouchEndCallback(OnTouchEndCallback touchEndCallback) {
            this->touchEndCallback = touchEndCallback;
        }

    private:

        bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
        void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
        void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

        /**
         * 检测是否是旋转多边形
         * @param pos
         * @return
         */
        bool checkIsRotatePolygon(const cocos2d::Vec2 &pos);

        /**
         * 绘制
         */
        void onDraw();

        /**
         * 置顶
         */
        void bringToFront();

    public:

        cocos2d::DrawNode* polygonDrawNode;
        Polygon polygon;

    private:
        cocos2d::EventListenerTouchOneByOne* touchListener;

        OnTouchEndCallback touchEndCallback;

        cocos2d::Vector<cocos2d::Sprite*> rotateSpriteList; //顶点旋转Sprite
        cocos2d::Color4F fillColor; //多边形填充色

        bool isTouchEnabled;
        bool isSelected; //多边形是否选中
        TouchType touchType; //Touch操作类型
    };
}

#endif /* PolygonView_h */

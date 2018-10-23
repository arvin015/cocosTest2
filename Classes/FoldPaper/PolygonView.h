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

        typedef std::function<void()> OnSelectCallback;
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
         * 检测两个多边形是否需要吸附
         * @param otherPolygon
         * @param minDistance
         * @param needAttach
         * @return
         */
        bool checkIsCloseEnough(PolygonView* otherPolygon, float minDistance, bool needAttach = false);

        /**
         * 添加子多边形
         * @param polygonView
         */
        void addChildPolygonView(PolygonView* polygonView);

        /**
         * 设置父多边形
         * @param polygonView
         */
        void setParentPolygonView(PolygonView* polygonView) {
            this->parentPolygonView = polygonView;
        }

        /**
         * 清除所有子多边形
         */
        void removeAllChildPolygon();

        /**
         * 加入依赖
         * @param parent
         */
        void attach(PolygonView* parent);

        /**
         * 解除依赖
         */
        void detach();

        /**
         * 获取世界坐标
         */
        cocos2d::Vec2 getPolygonViewWorldPoint(const cocos2d::Vec2 &nodePoint);

        /**
         * 获取多边形内坐标
         */
        cocos2d::Vec2 getPolygonViewNodePoint(const cocos2d::Vec2 &worldPoint);

        /**
         * 设置选中回调
         * @param selectCallback
         */
        void setOnSelectCallback(OnSelectCallback selectCallback) {
            this->selectCallback = selectCallback;
        }

        /**
         * 设置TouchEnd回调
         */
        void setOnTouchEndCallback(OnTouchEndCallback touchEndCallback) {
            this->touchEndCallback = touchEndCallback;
        }

        /**
         * 置顶
         */
        void bringToFront();

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

    public:

        cocos2d::DrawNode* polygonDrawNode;
        Polygon polygon;

        PolygonView* parentPolygonView;
        cocos2d::Vector<PolygonView*> childPolygonViewList;

    private:
        cocos2d::EventListenerTouchOneByOne* touchListener;

        OnSelectCallback selectCallback;
        OnTouchEndCallback touchEndCallback;

        cocos2d::Vector<cocos2d::Sprite*> rotateSpriteList; //顶点旋转Sprite
        cocos2d::Color4F fillColor; //多边形填充色

        bool isTouchEnabled;
        bool isSelected; //多边形是否选中
        bool isMove; //是否移动了
        TouchType touchType; //Touch操作类型
    };
}

#endif /* PolygonView_h */

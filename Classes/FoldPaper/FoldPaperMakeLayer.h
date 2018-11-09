//
// Created by mac_arvin on 2018/11/8.
//

#ifndef PROJ_ANDROID_STUDIO_FOLDPAPERMAKELAYER_H
#define PROJ_ANDROID_STUDIO_FOLDPAPERMAKELAYER_H

#include "cocos2d.h"
#include "PolygonView.h"

namespace FoldPaper {

    enum PolygonType {
        SQUARE,
        POLYGON
    };

    class FoldPaperMakeLayer : public cocos2d::Layer {

    public:

        typedef std::function<void(bool)> CheckCanFoldCallback;

        FoldPaperMakeLayer();
        ~FoldPaperMakeLayer();

        virtual void onEnter();
        virtual void onExit();

        virtual bool init();

        CREATE_FUNC(FoldPaperMakeLayer);

    public:

        /**
         * 创建多边形
         * @param polygonType
         * @param centerPoint
         * @param edge
         * @param width
         * @param height
         */
        void createPolygonView(PolygonType polygonType, const cocos2d::Vec2 &centerPoint, int edge, float width, float height);

        /**
         * 响应颜色点击事件
         * @param color
         */
        void responseColorClick(const cocos2d::Color4F &color);

        /**
         * 响应折叠按钮点击事件
         * @param isChecked
         */
        void responseFoldClick(bool isChecked);

        /**
         * 响应删除按钮点击事件
         */
        void responseDelClick();

        void setCheckCanFoldCallback(CheckCanFoldCallback callback) {
            this->checkCanFoldCallback = callback;
        }

    private:

        virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
        virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
        virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

        /**
         * 检测是否可折叠
         */
        bool checkCanFold();

        /**
         * 更改所有多边形的位置
         * @param delta
         */
        void updateAllPolygonsPosition(const cocos2d::Vec2 &delta);

        /**
         * 检测吸附
         */
        void attachPolygons(PolygonView* polygonView);

        /**
         * 检测所有多边形是否存在重叠
         * @return
         */
        bool isExitOverlap();

        /**
         * 初始化图表
         */
        void initGraph();

        /**
         * 构建图表
         */
        void buildGraph();

        /**
         * 获取根多边形---相交多边形最多的
         * @return
         */
        PolygonView* getRootPolygonView();

        /**
         * 获取树长度
         * @param rootPolygon
         * @return
         */
        int getTreeNum(PolygonView* rootPolygon);

        /**
         * 多边形对齐原点
         */
        void align2Origin();

        /**
         * 设置所有多边形是否可Touch
         * @param touchEnabled
         */
        void setPolygonsTouchEnabled(bool touchEnabled);

        /**
         * 获取最大的order
         * @return
         */
        int getMaxOrder();

    public:
        cocos2d::Vector<PolygonView*> polygonViewList;

    private:
        cocos2d::EventListenerTouchOneByOne* touchListener;
        std::map<PolygonView*, cocos2d::Vector<PolygonView*>> graph;
        cocos2d::Node* doContainerNode;
        PolygonView* selectedPolygonView; //当前选中的多边形
        PolygonView* rootPolygonView; //根多边形
        bool isTouchEnabled;
        int ids;
        CheckCanFoldCallback checkCanFoldCallback; //检查是否可Fold回调
    };
}

#endif //PROJ_ANDROID_STUDIO_FOLDPAPERMAKELAYER_H

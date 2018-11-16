//
// Created by mac_arvin on 2018/11/8.
//

#ifndef PROJ_ANDROID_STUDIO_FOLDPAPERMAKELAYER_H
#define PROJ_ANDROID_STUDIO_FOLDPAPERMAKELAYER_H

#include "cocos2d.h"
#include "PolygonView.h"

namespace FoldPaper {

    const float PHI = 0.5f * (1.0f + sqrtf(5));

    inline float getRadiusByEdge(int edge, float edgeLength) {
            return edgeLength / (2.0f * sin(3.14 / edge));
    }

    enum PolygonType {
        SQUARE, //正方形、长方形
        POLYGON //规则的多边形
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
         * 生产多面体
         * @param side
         * @param edgeLength
         * @param height
         */
        void createPolygonViewFromPrism(int side, float edgeLength);

        /**
         * 生产立方体
         * @param edgeLength
         */
        void createPolygonViewFromCube(float edgeLength);

        /**
         * 创建多边形
         * @param polygonType
         * @param centerPoint
         * @param edge
         * @param edgeLength
         * @param height
         * @param faceType
         */
        void createPolygonView(int polygonType, const cocos2d::Vec2 &centerPoint, int edge, float edgeLength, float height, int faceType = 0);

    public:
        /**
         * 响应颜色点击事件
         * @param color
         */
        void responseColorClick(const cocos2d::Color4F &color);
        
        /**
         * 响应贴图点击
         * @param textureId
         */
        void responseTextureClick(const std::string &textureName);

        /**
         * 响应折叠按钮点击事件
         * @param isChecked
         */
        void responseFoldClick(bool isChecked);

        /**
         * 响应删除按钮点击事件
         */
        void responseDelClick();

        /**
         * 响应重置点击事件
         */
        void responseResetClick();

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
        bool isExistOverlap();

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

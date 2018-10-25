//
//  FoldPaperLayer.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2018/10/18.
//

#ifndef FoldPaperLayer_h
#define FoldPaperLayer_h

#include <stdio.h>
#include "BaseLayer.h"

namespace FoldPaper {

    enum PolygonType {
        SQUARE,
        POLYGON
    };

    class PolygonView;

    class FoldPaperLayer : public BaseLayer {
        
    public:
        
        FoldPaperLayer();
        ~FoldPaperLayer();
        
        virtual void onEnter();
        virtual void onExit();
        
        virtual bool init();
        
        CREATE_FUNC(FoldPaperLayer);

    private:

        virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
        virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
        virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

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
         * 更改所有多边形的位置
         * @param delta
         */
        void updateAllPolygonsPosition(const cocos2d::Vec2 &delta);

        /**
         * 检测吸附
         */
        void attachPolygons(PolygonView* polygonView);

        /**
         * 检测是否可折叠
         */
        bool checkCanFold();

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
         * 获取最大的order
         * @return
         */
        int getMaxOrder();

    private:
        cocos2d::EventListenerTouchOneByOne* touchListener;
        cocos2d::Vector<PolygonView*> polygonViewList;
        std::map<PolygonView*, cocos2d::Vector<PolygonView*>> graph;
        cocos2d::Node* doContainerNode;
        PolygonView* selectedPolygonView; //当前选中的多边形
        PolygonView* rootPolygonView; //根多边形
        cocos2d::ui::CheckBox* foldBtn;
        cocos2d::ui::Button* delBtn;
        int ids;
    };
}

#endif /* FoldPaperLayer_h */

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
         * 检测吸附
         */
        void attachPolygons(PolygonView* polygonView);

        /**
         * 检测是否可折叠
         */
        bool checkCanFold();

        /**
         * 获取树顶多边形---只有子多边形，没有父多边形
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
        cocos2d::Node* doContainerNode;
        PolygonView* selectedPolygonView; //当前选中的多边形
        cocos2d::ui::CheckBox* foldBtn;
        int ids;
    };
}

#endif /* FoldPaperLayer_h */

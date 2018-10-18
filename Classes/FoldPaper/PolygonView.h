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

    class PolygonView : public cocos2d::Node {

    public:
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
         *
         * @param centerPoint
         * @param length
         */
        void createSquare(const cocos2d::Vec2 &centerPoint, float width, float height);

        /**
         * 创建规则的多边形
         *
         * @param centerPoint
         * @param edge
         * @param radius
         */
        void createRegularPolygonWithRadius(const cocos2d::Vec2 &centerPoint, int edge, float radius);

        /**
         * 创建规则的多边形
         *
         * @param centerPoint
         * @param edge
         * @param edgeLength
         */
        void createRegularPolygonWithEdgeLength(const cocos2d::Vec2 &centerPoint, int edge, float edgeLength);

    private:
        cocos2d::DrawNode* polygonDrawNode;
        Polygon polygon;
    };
}

#endif /* PolygonView_h */

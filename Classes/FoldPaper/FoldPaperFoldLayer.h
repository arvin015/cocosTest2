//
// Created by mac_arvin on 2018/11/8.
//

#ifndef PROJ_ANDROID_STUDIO_FOLDPAPERFOLDLAYER_H
#define PROJ_ANDROID_STUDIO_FOLDPAPERFOLDLAYER_H

#include "cocos2d.h"
#include "CC3DLayer.h"
#include "cMultiTouch.h"
#include "Polygon3D.h"
#include "DrawNode3D.h"

namespace FoldPaper {

    inline cocos2d::Vec2 GLNormalized(const cocos2d::Vec2 &v) {
        cocos2d::Size size = cocos2d::Size(1024, 768);
        return cocos2d::Vec2((2.0f * v.x / size.width), (2.0f * v.y / size.height));
    }

    class FoldPaperFoldLayer : public cocos2d::Layer {

    public:
        FoldPaperFoldLayer();
        ~FoldPaperFoldLayer();

        virtual void onEnter();
        virtual void onExit();

        virtual bool init();

        CREATE_FUNC(FoldPaperFoldLayer);

    public:

        /**
         * 响应折叠按钮点击事件
         * @param isChecked
         */
        void responseFoldClick(bool isChecked);

        /**
         * 设置数据
         */
        void setData(std::vector<Polygon3D*> polygon3DList);

    private:

        virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
        virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
        virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

        virtual void update(float dt);

        /**
         * 转换成3D坐标，并居中对齐
         */
        void alignToOrigin();

        /**
         * 建立多边形之间的关系
         */
        void buildRelation();

        /**
         * 设置需折叠的多边形
         */
        void setFoldPolygon();

        /**
         * 折叠
         */
        void fold(float dt);

        /**
         * 绘制3D
         */
        void draw3D();

    private:
        cocos2d::EventListenerTouchOneByOne* touchListener;
        CC3DLayer* cc3dLayer;
        cocos2d::Vec3 camtarget, camoffset;
        cocos2d::Quaternion camquat;
        cMultiTouch multitouch;
        bool isFolding;
        bool isFoldOver;
        std::vector<Polygon3D*> polygon3DList;
        std::vector<Polygon3D*>::iterator mIter;
        Polygon3D* rootPolygon3D; //被吸附最多的作为根多边形
        Polygon3D* foldingPolygon3D;
        DrawNode3D* drawPolygon3D;
    };
}

#endif //PROJ_ANDROID_STUDIO_FOLDPAPERFOLDLAYER_H

//
//  VolumeCutfillLayer.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2019/2/12.
//

#ifndef VolumeCutfillLayer_h
#define VolumeCutfillLayer_h

#include <stdio.h>
#include "BaseLayer.h"
#include "cMultiTouch.h"
#include "CC3DLayer.h"
#include "BasicData.h"
#include "UIDrawNodeEx.h"
#include "Volume3D.h"
#include "ui/UIButton.h"

//namespace VolumeCutfill {

    class Volume3D;

    class VolumeCutfillLayer : public BaseLayer {

    public:

        VolumeCutfillLayer();
        virtual ~VolumeCutfillLayer();

        virtual void onEnter();
        virtual void onExit();

        virtual bool init();

        CREATE_FUNC(VolumeCutfillLayer);

        virtual bool shouldWhiteBg() {return false;};

        void reset();

    private:

        virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
        virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
        virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

        void initDefaultSolid();

        Volume3D* createVolume3D(const std::vector<VertexInfo> &vecs);

        VertexInfo getStartPoint(const cocos2d::Vec2 &point);

        bool getEndPoint(const cocos2d::Vec2 &point, const cocos2d::Vec2 &startPoint, VertexInfo &rs);

        /**
         * 切割
         */
        void cutSolid(Volume3D* volume3D, const VertexInfo &ver1, const VertexInfo &ver2);

        /**
         * 获取切割分离移位方向
         */
        Volume3D::TRANS_DIRECTION getTransDirection(const cocos2d::Vec2 &p1, const cocos2d::Vec2 &p2);

    private:

        cocos2d::EventListenerTouchOneByOne* touchListener;
        CC3DLayer* cc3DLayer;
        cocos2d::DirectionLight* light;
        cocos2d::Vec3 camtarget, camoffset;
        cocos2d::Quaternion camquat;
        cMultiTouch multitouch;
        bool canRotateCamera;

        DrawNodeEx* drawNodeEx;

        Volume3D* curVolume3D;
        cocos2d::Vector<Volume3D*> volume3DList;

        VertexInfo startVertex;

        cocos2d::ui::Button* resetBtn;
    };
//}

#endif /* VolumeCutfillLayer_h */

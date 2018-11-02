//
//  Test3DLayer.hpp
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2018/10/30.
//

#ifndef Test3DLayer_h
#define Test3DLayer_h

#include <stdio.h>
#include "BaseLayer.h"
#include "CC3DLayer.h"
#include "cMultiTouch.h"

namespace Test3D {

    class Test3DLayer : public BaseLayer {

    public:
        Test3DLayer();
        ~Test3DLayer();

        virtual void onEnter();
        virtual void onExit();

        virtual bool init();
        CREATE_FUNC(Test3DLayer);

        virtual bool shouldWhiteBg() {return false;}

    private:

        virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
        virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
        virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

        virtual void update(float dt);

        void play3DAnim(cocos2d::Sprite3D* s3d, std::function<void()> callback);
        void foldPaper();

        cMultiTouch multitouch;
        cocos2d::EventListenerTouchOneByOne* touchListener;
        Camera* camera;
        Vec3 camtarget, camoffset;
        Quaternion camquat;
        cocos2d::Sprite3D* rootS3d;
        cocos2d::Sprite3D* redS3d;
        cocos2d::Sprite3D* blueS3d;
        cocos2d::Sprite3D* greenS3d;
        cocos2d::Sprite3D* orangeS3d;
        cocos2d::Sprite3D* grayS3d;
        cocos2d::Vec3 dstPos;
        cocos2d::Vec3 rootOriginalPos;
        float curTime;
        bool isPlaying;
        bool isStepByStep;
        ui::Button* foldBtn;
        ui::Button* stepFoldBtn;
        std::queue<cocos2d::Sprite3D*> foldQueue;
    };
}

#endif /* Test3DLayer_h */

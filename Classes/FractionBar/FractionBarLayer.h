//
//  FractionBarLayer.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2018/12/26.
//

#ifndef FractionBarLayer_h
#define FractionBarLayer_h

#include <stdio.h>
#include "BaseLayer.h"

namespace FractionBar {

    class FractionBarView;

    class FractionBarLayer : public BaseLayer {

    public:

        FractionBarLayer();
        virtual ~FractionBarLayer();

        virtual void onEnter();
        virtual void onExit();

        virtual bool init();

        CREATE_FUNC(FractionBarLayer);

        /**
         * 添加棒条
         */
        void createFractionBar(int bottomValue);

    private:

        /**
         * 检测是否碰撞
         */
        void checkIsCrashed(FractionBarView* target);

        /**
         * 检测是否吸附
         */
        void checkIsAttach(FractionBarView* target);

        /**
         * 设置锁精灵显示隐藏
         */
        void setLockSpriteVisible(bool visible);
        
        /**
         * 锁精灵变化
         */
        void updateLockSprite();

        /**
         * 绘制碰撞红框
         */
        void onDraw();

    private:

        cocos2d::EventListenerTouchOneByOne* touchListener;

        cocos2d::Node* doContainer; //主操作容器
        cocos2d::DrawNode* borderDraw; //绘制棒条碰撞边框
        cocos2d::Sprite* lockSprite; //临时锁精灵

        FractionBarView* curFractionBarView; //操作棒条

        int ids;

        cocos2d::Vector<FractionBarView*> fractionBarViewList;
        std::map<FractionBarView*, cocos2d::Vector<FractionBarView*>> fractionBarViewMap; //发生碰撞map
    };
}
#endif /* FractionBarLayer_h */

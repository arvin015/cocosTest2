//
//  FractionBarView.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2018/12/26.
//

#ifndef FractionBarView_h
#define FractionBarView_h

#include <stdio.h>
#include "cocos2d.h"
#include "ui/UILayout.h"
#include "ui/UIText.h"
#include "MathUtils.h"

namespace FractionBar {

    class FractionBarView : public cocos2d::Node {

    public:

        typedef std::function<void(int id, int type, float x, float y)> OnTouchCallback;
        typedef std::function<void()> OnSelectCallback;

        FractionBarView();
        virtual ~FractionBarView();

        virtual void onEnter();

        virtual void onExit();

        static FractionBarView *create(const cocos2d::Size &size, const cocos2d::Vec2 &position,
                                       const cocos2d::Color3B &color, int left, int top,
                                       int bottom);

        virtual bool init(const cocos2d::Size &size, const cocos2d::Vec2 &position,
                          const cocos2d::Color3B &color, int left, int top, int bottom);

        virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
        virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
        virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

        /**
         * 设置棒条是否选中
         */
        void setBarSelected(bool isSelected);

        /**
         * 检测两个棒条是否碰撞
         */
        bool isIntersect(FractionBarView* other);

        /**
         * 检测两个棒条是否吸附
         */
        bool isCloseEnough(FractionBarView* other, float &dx, float &dy, bool &isCrash);

        /**
         * 获取当前锁精灵纹理
         */
        std::string getLockSpriteTexture() {
            return lockSprite->getTexture()->getPath();
        }
        
        /**
         * 置顶
         */
        void bringToFront();

        void setOnTouchCallback(OnTouchCallback touchCallback) {
            this->touchCallback = touchCallback;
        }

        void setOnSelectCallback(OnSelectCallback selectCallback) {
            this->selectCallback = selectCallback;
        }

    private:

        /**
         * 绘制边框
         */
        void drawBorder(bool isSelected);

    public:

        int barId;

    private:

        cocos2d::EventListenerTouchOneByOne* touchListener;

        cocos2d::ui::Layout *block;
        cocos2d::ui::Text *leftText;
        cocos2d::ui::Text *topText;
        cocos2d::ui::Text *bottomText;
        cocos2d::ui::Layout *divideLine;
        cocos2d::Sprite *lockSprite;
        cocos2d::DrawNode *borderDraw;

        bool isTouchEnabled;
        bool isSelected;
        bool isLockSelected;
        bool isLocked;
        bool isMoved;

        OnTouchCallback touchCallback;
        OnSelectCallback selectCallback;
    };
}

#endif /* FractionBarView_h */

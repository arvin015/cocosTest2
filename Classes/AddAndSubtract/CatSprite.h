//
//  CatSprite.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2017/12/5.
//

#ifndef CatSprite_h
#define CatSprite_h

#include <stdio.h>
#include "cocos2d.h"

class CatSprite : public cocos2d::Sprite {
    
public:
    
    typedef std::function<void(float)> OnTouchUpCallback;
    
    CatSprite();
    virtual ~CatSprite();
    
    virtual void onEnter();
    virtual void onExit();
    
    virtual bool init();
    
    CREATE_FUNC(CatSprite);

    static CatSprite* create(const std::string &imageName);
    bool init(const std::string &imageName);

    /**
     * 设置是否可Touch
     */
    void setIsTouchEnabled(bool isTouchEnabled) {
        this->isTouchEnabled = isTouchEnabled;
    }

    /**
     * 播放移位动画
     */
    void playAnim(float toX);

    /**
     * 设置Touch结束回调
     */
    void setOnTouchUpCallback(OnTouchUpCallback touchUpCallback) {
        this->touchUpCallback = touchUpCallback;
    }

private:

    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
private:

    cocos2d::EventListenerTouchOneByOne* eventListener;

    bool isTouchEnabled; //是否可Touch
    bool isSelect; //是否选中

    OnTouchUpCallback touchUpCallback;
};

#endif /* CatSprite_h */
